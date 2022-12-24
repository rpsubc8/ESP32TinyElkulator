//Elkulator 0.6 by Tom Walker
//1770 emulator
#include "gbConfig.h"

#include <stdio.h>
#include <stdlib.h> //Para exit
//JJ #include <allegro.h>
#include "elk.h"
#include "6502.h"
#include "1770.h"

#include "gbGlobals.h"
//#ifdef use_lib_disk_flash_test
// #include "dataFlash/gbdsk.h"
//#endif

#define SIDES 2
#define TRACKS 80
#define SECTORS 16
#define SECTORSIZE 256

int changed[2];

//JJ char serr[256];
//JJ int discin[4]; //No se usa

//JJ int output;
//#ifdef use_lib_disk_flash
// unsigned char discs[1][SIDES][SECTORS*TRACKS][SECTORSIZE];
//#else

#ifdef use_lib_disk_flash_test
#else
 unsigned char discs[2][SIDES][SECTORS*TRACKS][SECTORSIZE];
#endif 
//#endif 

int endcommand=0;
int curside,curdisc;
int nmi,oldnmi,nmilock;
int discint;

int sectorpos;
struct
{
        unsigned char track,sector,data,command,control,status;
        int curtrack,cursector;
} wd1770;

void loaddisc(char *fn, int drv)
{
//#ifdef use_lib_disk_flash
// return;
//#endif

#ifdef use_lib_disk_flash_test
#else
     
        char s[80];
        FILE *f=fopen(fn,"rb");
        int c=0,d,e,ff;
        if (!f) return;
        changed[drv]=0;
        while (fn[c]) c++; /*Find end of filename*/
        while (fn[c]!='.') c--; /*Find file extension*/
        #ifdef use_lib_log_serial
         printf("Disco fin %d\n",c);
        #endif 
        //fflush(stdout);
        if (fn[c+1]=='a' || fn[c+1]=='A') /*Test file extension*/
        {                                 /*.ADF - ADFS file*/
         #ifdef use_lib_log_serial
          printf("disco adf\n");
         #endif 
         //fflush(stdout);
                for (c=0;c<80*16*256;c++)
                    discs[drv][0][c>>8][c&255]=getc(f);
        }
        else if (fn[c+1]=='s' || fn[c+1]=='S') /*.SSD - Single sided DFS file*/
        {
         #ifdef use_lib_log_serial
          printf("disco ssd\n");
         #endif 
         //fflush(stdout);             
                for (c=0;c<80;c++)
                {
                        for (d=0;d<10;d++)
                        {
                                for (e=0;e<256;e++)
                                {
                                        discs[drv][0][(c<<4)+d][e]=getc(f);
                                }
                        }
                }
        }
        else if (fn[c+1]=='d' || fn[c+1]=='D') /*.DSD - Double sided DFS file*/
        {
         #ifdef use_lib_log_serial
          printf("disco dsd\n");
         #endif 
         //fflush(stdout);                     
                for (c=0;c<80;c++)
                {
                        for (ff=0;ff<2;ff++)
                        {
                                for (d=0;d<10;d++)
                                {
                                        for (e=0;e<256;e++)
                                        {
                                                discs[drv][ff][(c<<4)+d][e]=getc(f);
                                        }
                                }
                        }
                }
        }
        fclose(f);
 #endif        
}

void savedisc(char *fn, int drv)
{
//#ifdef use_lib_disk_flash
// return;
//#endif
     
#ifdef use_lib_disk_flash_test
#else     
        char s[80];
        FILE *f=fopen(fn,"wb");
        int c=0,d,e,ff;
        if (!f) return;
        while (fn[c]) c++; /*Find end of filename*/
        while (fn[c]!='.') c--; /*Find file extension*/
        if (fn[c+1]=='a' || fn[c+1]=='A') /*Test file extension*/
        {                                 /*.ADF - ADFS file*/
                for (c=0;c<80*16*256;c++)
                    putc(discs[drv][0][c>>8][c&255],f);
        }
        else if (fn[c+1]=='s' || fn[c+1]=='S') /*.SSD - Single sided DFS file*/
        {
                for (c=0;c<80;c++)
                {
                        for (d=0;d<10;d++)
                        {
                                for (e=0;e<256;e++)
                                {
                                        putc(discs[drv][0][(c<<4)+d][e],f);
                                }
                        }
                }
        }
        else if (fn[c+1]=='d' || fn[c+1]=='D') /*.DSD - Double sided DFS file*/
        {
                for (c=0;c<80;c++)
                {
                        for (ff=0;ff<2;ff++)
                        {
                                for (d=0;d<10;d++)
                                {
                                        for (e=0;e<256;e++)
                                        {
                                                putc(discs[drv][ff][(c<<4)+d][e],f);
                                        }
                                }
                        }
                }
        }
        fclose(f);
 #endif        
}

void reset1770()
{
        wd1770.control=0xFF;
        wd1770.status=0x80;
        discint=0;
        nmi=0;
}

void set1770poll(int c)
{
        discint=c;
}

void start1770command()
{
        wd1770.status|=1;
        endcommand=0;
        switch (wd1770.command>>4)
        {
                case 0: wd1770.track=wd1770.curtrack=0; wd1770.status&=~1; wd1770.status|=4; break; /*Restore*/
                case 1: wd1770.track=wd1770.curtrack=wd1770.data; wd1770.status&=~5; if (!wd1770.curtrack) wd1770.status|=4; break; /*Seek*/
                case 5: wd1770.curtrack++; wd1770.track=wd1770.curtrack; wd1770.status&=~5; break; /*Step in*/
                case 8: wd1770.status=0x83; sectorpos=0; wd1770.curtrack=wd1770.track; wd1770.cursector=wd1770.sector; break; /*Read sector*/
                case 0xA: wd1770.status=0x83; sectorpos=0; wd1770.curtrack=wd1770.track; wd1770.cursector=wd1770.sector; changed[curdisc]=1; break; /*Write sector*/
                default:
                #ifdef use_lib_log_serial
                 printf("Bad 1770 command %01X\n",wd1770.command>>4);
                #endif 
                dumpregs();
                exit(-1);
        }
}

void write1770(unsigned short addr, unsigned char val)
{
#ifdef use_lib_nodisk
#else     
        switch (addr)
        {
                case 0xFCC0: /*Control register*/
                wd1770.control=val;
                if (val&0x20) reset1770();
                if (val&1) curdisc=0;
                if (val&2) curdisc=1;
                if (val&4) curside=1; else curside=0;
                return;
                case 0xFCC4: /*Command*/
                if (wd1770.status&1 && (val>>4)!=0xD) return;
                wd1770.command=val;
                start1770command();
                return;
                case 0xFCC5: /*Track register*/
                wd1770.track=val;
                return;
                case 0xFCC6: /*Sector register*/
                wd1770.sector=val;
                return;
                case 0xFCC7: /*Data register*/
                wd1770.status&=~2;
                nmi&=~2;
                wd1770.data=val;
                if ((wd1770.command>>4)==0xA)
                {
                 #ifdef use_lib_disk_flash_test
                 #else
                  discs[curdisc][curside][(wd1770.curtrack*16)+wd1770.cursector][sectorpos++]=val;
                 #endif 
                 if (sectorpos==256)
                 {
                  wd1770.status=0x80;
                  wd1770.command=0;
                 }
                 else
                 {
                  wd1770.status|=2;
                 }
                }
                return;
        }
 #endif        
}

unsigned char read1770(unsigned short addr)
{
#ifdef use_lib_disk_flash
 return 0;
#endif

#ifdef use_lib_nodisk         
#else
        switch (addr)
        {
         case 0xFCC0: //Control
          return wd1770.control;
         case 0xFCC4: //Status register
          return wd1770.status;
         case 0xFCC5: //Track register
          return wd1770.track;
         case 0xFCC6: //Sector register
          return wd1770.sector;
         case 0xFCC7: //Data register
          if ((wd1770.command>>4)==8)
          {
           #ifdef use_lib_disk_flash_test
            unsigned int ofsDsk= sectorpos + (wd1770.cursector * 256) + (wd1770.curtrack * 16 * 256) + (curside * 80 * 16 * 256);
            //if (ofsDsk >= gb_list_dsk_end[0])
            if (ofsDsk >= gb_ptr_dsk_end[gb_dsk_id_cur])
            {
             wd1770.data= 0x5A;
            }
            else
            {
             //wd1770.data= gb_list_dsk_data[0][ofsDsk];
             wd1770.data= gb_ptr_dsk_data[gb_dsk_id_cur][ofsDsk];
            }
            sectorpos++;                       
           #else
            wd1770.data=discs[curdisc][curside][(wd1770.curtrack*16)+wd1770.cursector][sectorpos++];
           #endif 
           if (sectorpos==256)
           {
            wd1770.status=0x80;
            wd1770.command=0;
           }
          }
          return wd1770.data; 
         default:
          return 0; 
        }
 #endif        
}
