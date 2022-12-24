//Elkulator 0.5 by Tom Walker
//UEF handling
//JJ #include <allegro.h>

#include "gbConfig.h"

#ifndef use_lib_no_tape

#include "elk.h"
//portESP32 #include <zlib.h>
#include <stdio.h>
#include <stdlib.h> //Para exit
#include "uef.h"
#include "ula.h"

#define HIGHTONE 0x40

int blocks=0;
int tapelcount,tapellatch,pps;
int intone=0;
//portESP32 gzFile *uef;

int inchunk=0,chunkid=0,chunklen=0;
int chunkpos=0,chunkdatabits=8;
float chunkf;

void openuef(char *fn)
{
      int c;
//portESP32       if (uef)
//JJ         gzclose(uef);
//JJ      uef=gzopen(fn,"rb"); //no tengo gzip
//portESP32       if (!uef) return;
      //JJfor (c=0;c<12;c++)
          //JJ gzgetc(uef);
      inchunk=chunklen=chunkid=0;
      tapellatch=(1000000/(1200/10))/64;
      tapelcount=0;
      pps=120;
}

int ueffileopen()
{
//portESP32         if (!uef)
//portESP32            return 0;
        return 1;
}

void polltape()
{
        int c;
        unsigned long templ;
        float *tempf;
        unsigned char temp;
//portESP32         if (!uef)
//portESP32            return;
        if (!inchunk)
        {
//                printf("%i ",gztell(uef));
//JJ                gzread(uef,&chunkid,2);
//JJ                gzread(uef,&chunklen,4);
//JJ                if (gzeof(uef))
//JJ                {
//JJ                        gzseek(uef,12,SEEK_SET);
//JJ//JJ                        gzread(uef,&chunkid,2);
//JJ//JJ                        gzread(uef,&chunklen,4);
//JJ                }
                inchunk=1;
                chunkpos=0;
//                printf("Chunk ID %04X len %i\n",chunkid,chunklen);
        }
        switch (chunkid)
        {
                case 0x000: /*Origin*/
//JJ                for (c=0;c<chunklen;c++)
//JJ                    gzgetc(uef);
                inchunk=0;
                return;

                case 0x005: /*Target platform*/
//JJ                for (c=0;c<chunklen;c++)
//JJ                    gzgetc(uef);
                inchunk=0;
                return;

                case 0x100: /*Raw data*/
                chunklen--;
                if (!chunklen)
                {
                        inchunk=0;
//                        blocks++;
//                        textprintf(screen,font,0,0,7,"%02X",blocks);
                }
//JJ                receive(gzgetc(uef));//no tengo gzip
                return;

                case 0x104: /*Defined data*/
                if (!chunkpos)
                {
//JJ                        chunkdatabits=gzgetc(uef);
//JJ                        gzgetc(uef);
//JJ                        gzgetc(uef);
                        chunklen-=3;
                        chunkpos=1;
                }
                else
                {
                        chunklen--;
                        if (chunklen<=0)
                           inchunk=0;
//JJ                        temp=gzgetc(uef);
//                        printf("%i : %i %02X\n",gztell(uef),chunklen,temp);
                        if (chunkdatabits==7) receive(temp&0x7F);
                        else                  receive(temp);
                }
                return;

                case 0x110: /*High tone*/
                if (!intone)
                {
//JJ                        templ=gzgetc(uef); templ|=(gzgetc(uef)<<8);
//                        printf("High tone %04X\n",templ);
                        if (templ>20)
                        {
                                intula(HIGHTONE);
                                intone=6;
                        }
                        else
                           inchunk=0;
                }
                else
                {
                        intone--;
                        if (intone==0)
                        {
                                inchunk=0;
                        }
                }
                return;

                case 0x111: /*High tone with dummy byte*/
                if (!intone)
                {
                        intula(HIGHTONE);
                        intone=3;
                }
                else
                {
                        if (intone==4)
                           intula(HIGHTONE);
                        intone--;
                        if (intone==0 && inchunk==2)
                        {
                                inchunk=0;
//JJ                                gzgetc(uef); gzgetc(uef);
//JJ                                gzgetc(uef); gzgetc(uef);
                        }
                        else if (!intone)
                        {
                                inchunk=2;
                                intone=4;
                                receive(0xAA);
                        }
                }
                return;

                case 0x112: /*Gap*/
                if (!intone)
                {
                        intone=3;
                }
                else
                {
                        intone--;
                        if (intone==0)
                        {
                                inchunk=0;
//JJ                                gzgetc(uef); gzgetc(uef);
                        }
                }
                return;

                case 0x113: /*Float baud rate*/
//JJ                templ=gzgetc(uef);
//JJ                templ|=(gzgetc(uef)<<8);
//JJ                templ|=(gzgetc(uef)<<16);
//JJ                templ|=(gzgetc(uef)<<24);
//JJ                tempf=&templ; //convert long unsigned int * to float*
//JJ                tapellatch=(1000000/((*tempf)/10))/64;
//JJ                pps=(*tempf)/10;
                inchunk=0;
                return;

                case 0x116: /*Float gap*/
                if (!chunkpos)
                {
//JJ                        templ=gzgetc(uef);
//JJ                        templ|=(gzgetc(uef)<<8);
//JJ                        templ|=(gzgetc(uef)<<16);
//JJ                        templ|=(gzgetc(uef)<<24);
//JJ                        tempf=&templ;  //convert long unsigned int * to float*
                        chunkf=*tempf;
//                        printf("Gap %f\n",chunkf);
                        chunkpos=1;
                }
                else
                {
//                        printf("Gap now %f\n",chunkf);
                        chunkf-=((float)1/(float)pps);
                        if (chunkf<=0) inchunk=0;
                }
                return;

                case 0x114: /*Security waves*/
                case 0x115: /*Polarity change*/
//                default:
                for (c=0;c<chunklen;c++)
//JJ                    gzgetc(uef);
                inchunk=0;
                return;

//116 : float gap
//113 : float baud rate

        }
//JJ        allegro_exit();
        #ifdef use_lib_log_serial
         printf("Bad chunk ID %04X length %i\n",chunkid,chunklen);
        #endif 
        exit(-1);
}

void closeuef()
{
//JJ        if (uef)
//JJ           gzclose(uef);
}

#endif
