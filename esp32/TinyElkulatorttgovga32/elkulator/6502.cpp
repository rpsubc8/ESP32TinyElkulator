//Elkulator 0.5 by Tom Walker
//6502 emulation
#include "gbConfig.h"

//JJ #include <allegro.h>
#include <stdio.h>
#include <stdlib.h> //Para malloc
//portESP32 #include <dir.h>

#include "elk.h"
#include <stdio.h>
#include <string.h>
#include "6502.h"
#include "1770.h"
#include "ula.h"
#include "uef.h"

#include "gbGlobals.h"

//#include "dataFlash/gbsna.h"

#ifdef use_lib_os_flash
 #include "dataFlash/gbos.h"
 #include "dataFlash/gbrom.h"
#endif 

//int nmi,oldnmi,nmilock;
//JJ int tapewcount;
//JJ void dumpregs();

//JJ int tapelcount,tapellatch;
int countit=0;

int output2=0;

int cycles=0;
int extline;
int output=0;
int ins=0;

//portESP32void dumpram()
//portESP32{
//portESP32        FILE *f=fopen("ram.dmp","wb");
//portESP32        fwrite(ram,65536,1,f);
//portESP32        fclose(f);
//portESP32}

#ifdef use_lib_rom_flash
 unsigned char gb_rom_null[0x4000];
 
 void AsignarROMBasic()
 {
  int c;
  #ifdef use_lib_log_serial
   printf("Asignar ROM flash BASIC\n");
  #endif 
  memset(gb_rom_null,0,0x4000);
  
  for (int i=0;i<16;i++)
  {
   gb_array_rom[i]= gb_rom_null;
  }
  gb_array_rom[10-1]= (unsigned char *)gb_rom_basic; //10 x 0x4000
  gb_array_rom[11-1]= (unsigned char *)gb_rom_basic; //11 x 0x4000
  #ifdef use_lib_dfs
  #else
   gb_array_rom[15-1]= (unsigned char *)gb_rom_adfs;  //15 x 0x4000
  #endif 
  gb_array_rom[3-1]= (unsigned char *)gb_rom_dfs;    //3 x 0x4000
  
  for (c=0x80;c<0x0C0;c++)
  {
   //mem[c]=(unsigned char *)(rom+((c&0x3F)<<8));
   mem[c]=(unsigned char *)gb_rom_null;
   //printf ("c:%d\n",((c&0x3F)<<8));
  }
  //fflush(stdout);
  //exit(-1);
 }
#endif 

void initmem()
{
        int c;
        //portESP32 ram=(unsigned char *)malloc(0x10000);
        #ifdef use_lib_rom_flash
        #else
         rom=(unsigned char *)malloc(0x40000);
        #endif 
        memset(ram,0,0x8000);
        for (c=0x00;c<0x080;c++) mem[c]=(unsigned char *)(ram+(c<<8));
        //Hay que tener en cuenta flash BASIC
        #ifdef use_lib_rom_flash
         AsignarROMBasic();
        #else
         for (c=0x80;c<0x0C0;c++) mem[c]=(unsigned char *)(rom+((c&0x3F)<<8));
        #endif 
        
        //for (c=0xC0;c<0x100;c++) 
        //{
        // printf ("c:%d\n",((c&0x3F)<<8));
        //}
        //fflush(stdout);
        //Si usa os en flash apunta a el
        for (c=0xC0;c<0x100;c++) mem[c]=(unsigned char *)(os+((c&0x3F)<<8));
                
        for (c=0x00;c<0x080;c++) memstat[c]=0;
        for (c=0x80;c<0x100;c++) memstat[c]=1;
        memstat[0xFC]=2;
        memstat[0xFE]=2;
//        atexit(dumpram);
}

void remaketables()
{
        int c;
        for (c=0x00;c<0x080;c++) mem[c]=(unsigned char *)(ram+(c<<8));
        #ifdef use_lib_rom_flash
         AsignarROMBasic();
        #else
         for (c=0x80;c<0x0C0;c++) mem[c]=(unsigned char *)(rom+((c&0x3F)<<8));
        #endif 
        for (c=0xC0;c<0x100;c++) mem[c]=(unsigned char *)(os+((c&0x3F)<<8));
        
        //for (c=0xC0;c<0x100;c++)
        //{
        // printf ("c:%d\n",((c&0x3F)<<8));            
        //}
        //fflush(stdout);
        
        
        for (c=0x00;c<0x080;c++) memstat[c]=0;
        for (c=0x80;c<0x100;c++) memstat[c]=1;
        memstat[0xFC]=2;
        memstat[0xFE]=2;
}

#ifdef use_lib_rom_flash
#else
void loadroms()
{
 FILE *f;
 #ifdef use_lib_os_flash
  //os = (unsigned char *)gb_rom_os;
  //for (int i=0;i<100;i++)
  //{
  // printf("%02X,",os[i]);
  //}
  #ifdef use_lib_log_serial
   printf("Load rom flash\n");
  #endif 
  //fflush(stdout);
 #else
  //JJ FILE *f=fopen("os","rb");
  f=fopen("os","rb");
  fread(os,16384,1,f);
  fclose(f);
  #ifdef use_lib_log_serial
   printf("Load rom disk\n");
  #endif 
  //fflush(stdout);  
 #endif 
  
        f=fopen("basic.rom","rb");
        fread(&rom[10*0x4000],16384,1,f);
        fclose(f);
  #ifdef use_lib_log_serial
   printf("Load basic.rom disk\n");
  #endif 
  //fflush(stdout);          
        
        f=fopen("basic.rom","rb");
        fread(&rom[11*0x4000],16384,1,f);
        fclose(f);

  #ifdef use_lib_log_serial
   printf("Load basic.rom disk\n");
  #endif 
  //fflush(stdout);                  
/*        f=fopen("adfs.rom","rb");
        fread(&rom[15*0x4000],16384,1,f);
        fclose(f);*/
}
#endif


void loadcart(char *fn)
{
        FILE *f=fopen(fn,"rb");
        #ifdef use_lib_rom_flash
        #else
         fread(&rom[12*0x4000],16384,1,f);
        #endif 
        fclose(f);
}

void loadcart2(char *fn)
{
        FILE *f=fopen(fn,"rb");
        #ifdef use_lib_rom_flash
        #else
         fread(&rom[13*0x4000],16384,1,f);
        #endif 
        fclose(f);
}

void unloadcart()
{
 #ifdef use_lib_rom_flash     
 #else
  memset(&rom[12*0x4000],0,16384);
  memset(&rom[13*0x4000],0,16384);
 #endif 
}

void updateadfs()
{
 #ifdef use_lib_rom_flash     
 #else
        FILE *f;
        memset(&rom[3*0x4000],0,16384);
        memset(&rom[15*0x4000],0,16384);
        if (plus3ena)
        {
                if (adfsena)
                {
                        f=fopen("adfs.rom","rb");
                        fread(&rom[15*0x4000],16384,1,f);
                        fclose(f);
                }
                if (dfsena)
                {
                        f=fopen("dfs.rom","rb");
                        fread(&rom[3*0x4000],16384,1,f);
                        fclose(f);
                }
        }
 #endif        
}

unsigned char readmeml(unsigned short addr)
{//6 veces
        //gb_cont_loop++;
        switch (addr&0xFFF8)
        {
                case 0xFC40: return 0xFF; /*HDD - why you'd want one on an elk is beyond me*/
                case 0xFC70: case 0xFC71: case 0xFC72: return 0xFF; /*+1 - not implemented*/
                case 0xFCC0: return read1770(addr);
                case 0xFCE0: case 0xFCE8: return 0xFF; /*The Tube - doesn't exist for elk apparently*/
//                case 0xFE00: case 0xFE08: return readula(addr);
        }
        if ((addr&0xFF00)==0xFE00)
        {
/*                if (cycles&1)
                   cycles--;
                else
                   cycles-=2;*/
                return readula(addr);
        }
        if (addr>=0x8000 && addr<0xC000) return readkeys(addr);
        if ((addr&0xFF00)==0xFC00) return 0;//JJ return;
        if ((addr&0xFF00)==0xFE00) return 0;//JJ return;
//JJ        allegro_exit();
        #ifdef use_lib_log_serial
         printf("Error : Bad read from %04X\n",addr);
        #endif 
        dumpregs();
        exit(-1);
}

unsigned char writememl(unsigned short addr, unsigned char val, int line)
{//8 a 143 veces
//         gb_cont_loop++;
        //int c; //No se usa
        switch (addr&0xFFF8)
        {
                case 0xFC40: return 0; //JJ return; //HDD
                case 0xFC70: case 0xFC71: case 0xFC72: return 0; //JJ return; //+1
                case 0xFCC0: write1770(addr,val); return 0; //JJ return;
                case 0xFCE0: case 0xFCE8: return 0; //return; ///The toob
//                case 0xFE00: case 0xFE08: writeula(addr,val); return;
        }
        if ((addr&0xFF00)==0xFE00)
        {
/*                if (cycles&1)
                   cycles--;
                else
                   cycles-=2;*/
//                printf("ULA write %04X %02X %i\n",addr,val,line);
                writeula(addr,val);
                return 0; //JJ return;
        }
        if (!(addr&0x8000))
        {
                ram[addr]=val;
                return 0; //JJ return;
        }
        if (addr<0xC000) return 0;//JJ return;
//JJ        allegro_exit();
        #ifdef use_lib_log_serial
         printf("Error : Bad write to %04X data %02X\n",addr,val);
        #endif 
        dumpregs();
        exit(-1);
}

inline unsigned char readmem(unsigned short a)
{//29694 veces
 //gb_cont_loop++;
 //printf("readmem:%08X\n",a);
 //fflush(stdout);
 //printf("memstat:%d\n",memstat[(a>>8)&0xFF]);
 //fflush(stdout); 
 
        if (!(a&0x8000))
        {
                if (!(ula.mode&4) && extline<256 && cycles>48)
                   cycles=48;
                else
                   cycles--;
        }
        return ((memstat[(a>>8)&0xFF]==2)?readmeml(a):mem[(a>>8)&0xFF][a&0xFF]);
}

inline void writemem(unsigned short a, unsigned char b)
{//1927 veces
 // gb_cont_loop++;
        if (!(a&0x8000))
        {
                if (!(ula.mode&4) && extline<256 && cycles>48)
                   cycles=48;
                else
                   cycles--;
        }
        if (memstat[(a>>8)&0xFF]==0) mem[(a>>8)&0xFF][a&0xFF]=b; else if (memstat[(a>>8)&0xFF]==2) writememl(a,b,extline);
}

//#define readmem(a) ((memstat[(a>>8)&0xFF]==2)?readmeml(a):mem[(a>>8)&0xFF][a&0xFF])
//#define writemem(a,b) if (memstat[(a>>8)&0xFF]==0) mem[(a>>8)&0xFF][a&0xFF]=b; else if (memstat[(a>>8)&0xFF]==2) writememl(a,b,lines)
#define getw() (readmem(pc)|(readmem(pc+1)<<8)); pc+=2

void reset6502()
{//4 veces
     //gb_cont_loop++;
        pc=readmem(0xFFFC)|(readmem(0xFFFD)<<8);
        p.i=1;
        nmi=oldnmi=nmilock=0;
 #ifdef use_lib_log_serial        
  printf("reset6502\n");
 #endif 
//fflush(stdout);
}

void dumpregs()
{
 #ifdef use_lib_log_serial        
  printf("6502 registers :\n");
  printf("A=%02X X=%02X Y=%02X S=01%02X PC=%04X\n",a,x,y,s,pc);
  printf("Status : %c%c%c%c%c%c\n",(p.n)?'N':' ',(p.v)?'V':' ',(p.d)?'D':' ',(p.i)?'I':' ',(p.z)?'Z':' ',(p.c)?'C':' ');
  printf("%i instructions executed\n",ins);
 #endif 
}

#define setzn(v) p.z=!(v); p.n=(v)&0x80

/*inline void push(unsigned char v)
{
        writemem(0x100+s,v);
        s--;
}

inline unsigned char pull()
{
        s++;
        return readmem(0x100+s);
}*/
#define push(v) writemem(0x100+(s--),v)
#define pull()  readmem(0x100+(++s))

#define polltime(c) { cycles-=c; }

/*ADC/SBC temp variables*/
unsigned short tempw;
int tempv,hc,al,ah;
unsigned char tempb;

#define ADC(temp)       if (!p.d)                            \
                        {                                  \
                                tempw=(a+temp+(p.c?1:0));        \
                                p.v=(!((a^temp)&0x80)&&((a^tempw)&0x80));  \
                                a=tempw&0xFF;                  \
                                p.c=tempw&0x100;                  \
                                setzn(a);                  \
                        }                                  \
                        else                               \
                        {                                  \
                                ah=0;        \
                                tempb=a+temp+(p.c?1:0);                            \
                                if (!tempb)                                      \
                                   p.z=1;                                          \
                                al=(a&0xF)+(temp&0xF)+(p.c?1:0);                            \
                                if (al>9)                                        \
                                {                                                \
                                        al-=10;                                  \
                                        al&=0xF;                                 \
                                        ah=1;                                    \
                                }                                                \
                                ah+=((a>>4)+(temp>>4));                             \
                                if (ah&8) p.n=1;                                   \
                                p.v=(((ah << 4) ^ a) & 128) && !((a ^ temp) & 128);   \
                                p.c=0;                                             \
                                if (ah>9)                                        \
                                {                                                \
                                        p.c=1;                                     \
                                        ah-=10;                                  \
                                        ah&=0xF;                                 \
                                }                                                \
                                a=(al&0xF)|(ah<<4);                              \
                        }

#define SBC(temp)       if (!p.d)                            \
                        {                                  \
                                tempw=a-(temp+(p.c?0:1));    \
                                tempv=(short)a-(short)(temp+(p.c?0:1));            \
                                p.v=((a^(temp+(p.c?0:1)))&(a^(unsigned char)tempv)&0x80); \
                                p.c=tempv>=0;\
                                a=tempw&0xFF;              \
                                setzn(a);                  \
                        }                                  \
                        else                               \
                        {                                  \
                                hc=0;                               \
                                p.z=p.n=0;                            \
                                if (!((a-temp)-((p.c)?0:1)))            \
                                   p.z=1;                             \
                                al=(a&15)-(temp&15)-((p.c)?0:1);      \
                                if (al&16)                           \
                                {                                   \
                                        al-=6;                      \
                                        al&=0xF;                    \
                                        hc=1;                       \
                                }                                   \
                                ah=(a>>4)-(temp>>4);                \
                                if (hc) ah--;                       \
                                if ((a-(temp+((p.c)?0:1)))&0x80)        \
                                   p.n=1;                             \
                                p.v=(((a-(temp+((p.c)?0:1)))^temp)&128)&&((a^temp)&128); \
                                p.c=1; \
                                if (ah&16)                           \
                                {                                   \
                                        p.c=0; \
                                        ah-=6;                      \
                                        ah&=0xF;                    \
                                }                                   \
                                a=(al&0xF)|((ah&0xF)<<4);                 \
                        }

void exec6502(int lines, int cpl)
{
        unsigned char opcode;
        unsigned short addr=0;
        //unsigned char temp,temp2;
        unsigned char temp; //temp2 no se usa
        int tempi;
        signed char offset;
        //int c; //No se usa
        int line=0;
        while (lines>=0)
        {
                extline=line;
//                if (lines==0) cycles+=(cpl>>1);
//                else
                cycles+=cpl;
                while (cycles>0)
                {
                        opcode=readmem(pc); pc++;
                        switch (opcode)
                        {
                                case 0x00: /*BRK*/
                                readmem(pc);
                                pc++;
                                push(pc>>8);
                                push(pc&0xFF);
                                temp=0x30;
                                if (p.c) temp|=1; if (p.z) temp|=2;
                                if (p.d) temp|=8; if (p.v) temp|=0x40;
                                if (p.n) temp|=0x80;
                                push(temp);
                                pc=readmem(0xFFFE)|(readmem(0xFFFF)<<8);
                                p.i=1;
                                polltime(7);
                                break;

                                case 0x01: /*ORA (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(temp-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                a|=readmem(addr);
                                setzn(a);
                                polltime(6);
                                break;

                                case 0x05: /*ORA zp*/
                                addr=readmem(pc); pc++;
                                a|=readmem(addr);
                                setzn(a);
                                polltime(3);
                                break;

                                case 0x06: /*ASL zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&0x80;
                                temp<<=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(5);
                                break;

                                case 0x08: /*PHP*/
                                readmem(pc);
                                temp=0x30;
                                if (p.c) temp|=1; if (p.z) temp|=2;
                                if (p.i) temp|=4; if (p.d) temp|=8;
                                if (p.v) temp|=0x40; if (p.n) temp|=0x80;
                                push(temp);
                                polltime(3);
                                break;

                                case 0x09: /*ORA imm*/
                                a|=readmem(pc); pc++;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x0A: /*ASL A*/
                                readmem(pc);
                                p.c=a&0x80;
                                a<<=1;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x0B: /*ANC imm*/
                                a&=readmem(pc); pc++;
                                setzn(a);
                                p.c=p.n;
                                polltime(2);
                                break;

                                case 0x0D: /*ORA abs*/
                                addr=getw();
                                a|=readmem(addr);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x0E: /*ASL abs*/
                                addr=getw();
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&0x80;
                                temp<<=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x10: /*BPL*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (!p.n)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0x11: /*ORA (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a|=readmem(addr+y);
                                setzn(a);
                                polltime(5);
                                break;

                                case 0x15: /*ORA zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                a|=readmem((addr+x)&0xFF);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x16: /*ASL zp,x*/
                                addr=(readmem(pc)+x)&0xFF; pc++;
                                readmem(addr);
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&0x80;
                                temp<<=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x18: /*CLC*/
                                readmem(pc);
                                p.c=0;
                                polltime(2);
                                break;

                                case 0x19: /*ORA abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a|=readmem(addr+y);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x1D: /*ORA abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                a|=readmem(addr+x);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x1E: /*ASL abs,x*/
                                addr=getw();
                                readmem(addr);
                                addr+=x;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&0x80;
                                temp<<=1;
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(7);
                                break;

                                case 0x20: /*JSR*/
                                addr=getw(); pc--;
                                readmem(0x100+s);
                                push(pc>>8);
                                push(pc);
                                pc=addr;
                                polltime(6);
                                break;

                                case 0x21: /*AND (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                a&=readmem(addr);
                                setzn(a);
                                polltime(6);
                                break;

                                case 0x24: /*BIT zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                p.z=!(a&temp);
                                p.v=temp&0x40;
                                p.n=temp&0x80;
                                polltime(3);
                                break;

                                case 0x25: /*AND zp*/
                                addr=readmem(pc); pc++;
                                a&=readmem(addr);
                                setzn(a);
                                polltime(3);
                                break;

                                case 0x26: /*ROL zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&0x80;
                                temp<<=1;
                                if (tempi) temp|=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(5);
                                break;

                                case 0x28: /*PLP*/
                                readmem(pc);
                                readmem(0x100+s);
                                temp=pull();
                                p.c=temp&1; p.z=temp&2;
                                p.i=temp&4; p.d=temp&8;
                                p.v=temp&0x40; p.n=temp&0x80;
                                polltime(4);
                                break;

                                case 0x29: /*AND*/
                                a&=readmem(pc); pc++;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x2A: /*ROL A*/
                                readmem(pc);
                                tempi=p.c;
                                p.c=a&0x80;
                                a<<=1;
                                if (tempi) a|=1;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x2C: /*BIT abs*/
                                addr=getw();
                                temp=readmem(addr);
                                p.z=!(a&temp);
                                p.v=temp&0x40;
                                p.n=temp&0x80;
                                polltime(4);
                                break;

                                case 0x2D: /*AND abs*/
                                addr=getw();
                                a&=readmem(addr);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x2E: /*ROL abs*/
                                addr=getw();
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&0x80;
                                temp<<=1;
                                if (tempi) temp|=1;
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(6);
                                break;

                                case 0x30: /*BMI*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (p.n)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0x31: /*AND (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a&=readmem(addr+y);
                                setzn(a);
                                polltime(5);
                                break;

                                case 0x35: /*AND zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                a&=readmem((addr+x)&0xFF);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x36: /*ROL zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                addr+=x; addr&=0xFF;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&0x80;
                                temp<<=1;
                                if (tempi) temp|=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x38: /*SEC*/
                                readmem(pc);
                                p.c=1;
                                polltime(2);
                                break;

                                case 0x39: /*AND abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a&=readmem(addr+y);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x3D: /*AND abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                a&=readmem(addr+x);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x3E: /*ROL abs,x*/
                                addr=getw();
                                readmem(addr);
                                addr+=x;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&0x80;
                                temp<<=1;
                                if (tempi) temp|=1;
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(7);
                                break;

                                case 0x40: /*RTI*/
                                readmem(pc);
                                readmem(0x100+s);
                                temp=pull();
                                p.c=temp&1; p.z=temp&2;
                                p.i=temp&4; p.d=temp&8;
                                p.v=temp&0x40; p.n=temp&0x80;
                                pc=pull();
                                pc|=(pull()<<8);
                                polltime(6);
                                nmilock=0;
//                                if (output) exit(-1);
                                break;

                                case 0x41: /*EOR (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                a^=readmem(addr);
                                setzn(a);
                                polltime(6);
                                break;

                                case 0x45: /*EOR zp*/
                                addr=readmem(pc); pc++;
                                a^=readmem(addr);
                                setzn(a);
                                polltime(3);
                                break;

                                case 0x46: /*LSR zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&1;
                                temp>>=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(5);
                                break;

                                case 0x48: /*PHA*/
                                readmem(pc);
                                push(a);
                                polltime(3);
                                break;

                                case 0x49: /*EOR*/
                                a^=readmem(pc); pc++;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x4A: /*LSR A*/
                                readmem(pc);
                                p.c=a&1;
                                a>>=1;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x4C: /*JMP*/
                                addr=getw();
                                pc=addr;
                                polltime(3);
                                break;

                                case 0x4D: /*EOR abs*/
                                addr=getw();
                                a^=readmem(addr);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x4E: /*LSR abs*/
                                addr=getw();
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&1;
                                temp>>=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x50: /*BVC*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (!p.v)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0x51: /*EOR (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a^=readmem(addr+y);
                                setzn(a);
                                polltime(5);
                                break;

                                case 0x55: /*EOR zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                a^=readmem((addr+x)&0xFF);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x56: /*LSR zp,x*/
                                addr=(readmem(pc)+x)&0xFF; pc++;
                                readmem(addr);
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&1;
                                temp>>=1;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x58: /*CLI*/
                                readmem(pc);
                                p.i=0;
                                polltime(2);
                                break;

                                case 0x59: /*EOR abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a^=readmem(addr+y);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x5D: /*EOR abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                a^=readmem(addr+x);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x5E: /*LSR abs,x*/
                                addr=getw();
                                readmem(addr);
                                addr+=x;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                p.c=temp&1;
                                temp>>=1;
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(7);
                                break;

                                case 0x60: /*RTS*/
                                readmem(pc);
                                readmem(0x100+s);
                                pc=pull();
                                pc|=(pull()<<8);
                                readmem(pc);
                                pc++;
                                polltime(6);
                                break;

                                case 0x61: /*ADC (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                temp=readmem(addr);
                                ADC(temp);
                                polltime(6);
                                break;

                                case 0x65: /*ADC zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                ADC(temp);
                                polltime(3);
                                break;

                                case 0x66: /*ROR zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&1;
                                temp>>=1;
                                if (tempi) temp|=0x80;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(5);
                                break;

                                case 0x68: /*PLA*/
                                readmem(pc);
                                readmem(0x100+s);
                                a=pull();
                                setzn(a);
                                polltime(4);
                                break;

                                case 0x69: /*ADC imm*/
                                temp=readmem(pc); pc++;
                                ADC(temp);
                                polltime(2);
                                break;

                                case 0x6A: /*ROR A*/
                                readmem(pc);
                                tempi=p.c;
                                p.c=a&1;
                                a>>=1;
                                if (tempi) a|=0x80;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x6C: /*JMP ()*/
                                addr=getw();
                                pc=readmem(addr);
                                if ((addr&0xFF)==0xFF) addr-=0x100;
                                pc|=(readmem(addr+1)<<8);
                                polltime(5);
                                break;

                                case 0x6D: /*ADC abs*/
                                addr=getw();
                                temp=readmem(addr);
                                ADC(temp);
                                polltime(4);
                                break;

                                case 0x6E: /*ROR abs*/
                                addr=getw();
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&1;
                                temp>>=1;
                                if (tempi) temp|=0x80;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x70: /*BVS*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (p.v)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0x71: /*ADC (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+y);
                                ADC(temp);
                                polltime(5);
                                break;

                                case 0x75: /*ADC zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                temp=readmem((addr+x)&0xFF);
                                ADC(temp);
                                polltime(4);
                                break;

                                case 0x76: /*ROR zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                addr+=x; addr&=0xFF;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&1;
                                temp>>=1;
                                if (tempi) temp|=0x80;
                                setzn(temp);
                                writemem(addr,temp);
                                polltime(6);
                                break;

                                case 0x78: /*SEI*/
                                readmem(pc);
                                p.i=1;
                                polltime(2);
                                break;

                                case 0x79: /*ADC abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+y);
                                ADC(temp);
                                polltime(4);
                                break;

                                case 0x7D: /*ADC abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+x);
                                ADC(temp);
                                polltime(4);
                                break;

                                case 0x7E: /*ROR abs,x*/
                                addr=getw();
                                readmem(addr);
                                addr+=x;
                                temp=readmem(addr);
                                writemem(addr,temp);
                                tempi=p.c;
                                p.c=temp&1;
                                temp>>=1;
                                if (tempi) temp|=0x80;
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(7);
                                break;

                                case 0x81: /*STA (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                writemem(addr,a);
                                polltime(6);
                                break;

                                case 0x84: /*STY zp*/
                                addr=readmem(pc); pc++;
                                writemem(addr,y);
                                polltime(3);
                                break;

                                case 0x85: /*STA zp*/
                                addr=readmem(pc); pc++;
                                writemem(addr,a);
                                polltime(3);
                                break;

                                case 0x86: /*STX zp*/
                                addr=readmem(pc); pc++;
                                writemem(addr,x);
                                polltime(3);
                                break;

                                case 0x88: /*DEY*/
                                readmem(pc);
                                y--;
                                setzn(y);
                                polltime(2);
                                break;

                                case 0x8A: /*TXA*/
                                readmem(pc);
                                a=x;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x8C: /*STY abs*/
                                addr=getw();
                                polltime(3);
                                writemem(addr,y);
                                polltime(1);
                                break;

                                case 0x8D: /*STA abs*/
                                addr=getw();
                                polltime(3);
                                writemem(addr,a);
                                polltime(1);
                                break;

                                case 0x8E: /*STX abs*/
                                addr=getw();
                                polltime(3);
                                writemem(addr,x);
                                polltime(1);
                                break;

                                case 0x90: /*BCC*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (!p.c)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0x91: /*STA (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8)+y;
                                readmem(addr);
                                writemem(addr,a);
                                polltime(6);
                                break;

                                case 0x94: /*STY zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                writemem((addr+x)&0xFF,y);
                                polltime(4);
                                break;

                                case 0x95: /*STA zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                writemem((addr+x)&0xFF,a);
                                polltime(4);
                                break;

                                case 0x96: /*STX zp,y*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                writemem((addr+y)&0xFF,x);
                                polltime(4);
                                break;

                                case 0x98: /*TYA*/
                                readmem(pc);
                                a=y;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x99: /*STA abs,y*/
                                addr=getw();
                                readmem(addr);
                                polltime(4);
                                writemem(addr+y,a);
                                polltime(1);
                                break;

                                case 0x9A: /*TXS*/
                                readmem(pc);
                                s=x;
                                polltime(2);
                                break;

                                case 0x9D: /*STA abs,x*/
                                addr=getw();
                                readmem(addr);
                                polltime(4);
                                writemem(addr+x,a);
                                polltime(1);
                                break;

                                case 0xA0: /*LDY imm*/
                                y=readmem(pc); pc++;
                                setzn(y);
                                polltime(2);
                                break;

                                case 0xA1: /*LDA (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                a=readmem(addr);
                                setzn(a);
                                polltime(6);
                                break;

                                case 0xA2: /*LDX imm*/
                                x=readmem(pc); pc++;
                                setzn(x);
                                polltime(2);
                                break;

                                case 0xA4: /*LDY zp*/
                                addr=readmem(pc); pc++;
                                y=readmem(addr);
                                setzn(y);
                                polltime(3);
                                break;

                                case 0xA5: /*LDA zp*/
                                addr=readmem(pc); pc++;
                                a=readmem(addr);
                                setzn(a);
                                polltime(3);
                                break;

                                case 0xA6: /*LDX zp*/
                                addr=readmem(pc); pc++;
                                x=readmem(addr);
                                setzn(x);
                                polltime(3);
                                break;

                                case 0xA8: /*TAY*/
                                readmem(pc);
                                y=a;
                                setzn(y);
                                break;

                                case 0xA9: /*LDA imm*/
                                a=readmem(pc); pc++;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0xAA: /*TAX*/
                                readmem(pc);
                                x=a;
                                setzn(x);
                                polltime(2);
                                break;

                                case 0xAC: /*LDY abs*/
                                addr=getw();
                                polltime(3);
                                y=readmem(addr);
                                setzn(y);
                                polltime(1);
                                break;

                                case 0xAD: /*LDA abs*/
                                addr=getw();
                                polltime(3);
                                a=readmem(addr);
                                setzn(a);
                                polltime(1);
                                break;

                                case 0xAE: /*LDX abs*/
                                addr=getw();
                                polltime(3);
                                x=readmem(addr);
                                setzn(x);
                                polltime(1);
                                break;

                                case 0xB0: /*BCS*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (p.c)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0xB1: /*LDA (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a=readmem(addr+y);
                                setzn(a);
                                polltime(5);
                                break;

                                case 0xB4: /*LDY zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                y=readmem((addr+x)&0xFF);
                                setzn(y);
                                polltime(4);
                                break;

                                case 0xB5: /*LDA zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                a=readmem((addr+x)&0xFF);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0xB6: /*LDX zp,y*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                x=readmem((addr+y)&0xFF);
                                setzn(x);
                                polltime(4);
                                break;

                                case 0xB8: /*CLV*/
                                readmem(pc);
                                p.v=0;
                                polltime(2);
                                break;

                                case 0xB9: /*LDA abs,y*/
                                addr=getw();
                                polltime(3);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                a=readmem(addr+y);
                                setzn(a);
                                polltime(1);
                                break;

                                case 0xBA: /*TSX*/
                                readmem(pc);
                                x=s;
                                setzn(x);
                                polltime(2);
                                break;

                                case 0xBC: /*LDY abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                y=readmem(addr+x);
                                setzn(y);
                                polltime(4);
                                break;

                                case 0xBD: /*LDA abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                a=readmem(addr+x);
                                setzn(a);
                                polltime(4);
                                break;

                                case 0xBE: /*LDX abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                x=readmem(addr+y);
                                setzn(x);
                                polltime(4);
                                break;

                                case 0xC0: /*CPY imm*/
                                temp=readmem(pc); pc++;
                                setzn(y-temp);
                                p.c=(y>=temp);
                                polltime(2);
                                break;

                                case 0xC1: /*CMP (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                temp=readmem(addr);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(6);
                                break;

                                case 0xC4: /*CPY zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                setzn(y-temp);
                                p.c=(y>=temp);
                                polltime(3);
                                break;

                                case 0xC5: /*CMP zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(3);
                                break;

                                case 0xC6: /*DEC zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr)-1;
                                writemem(addr,temp+1);
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(5);
                                break;

                                case 0xC8: /*INY*/
                                readmem(pc);
                                y++;
                                setzn(y);
                                polltime(2);
                                break;

                                case 0xC9: /*CMP imm*/
                                temp=readmem(pc); pc++;
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(2);
                                break;

                                case 0xCA: /*DEX*/
                                readmem(pc);
                                x--;
                                setzn(x);
                                polltime(2);
                                break;

                                case 0xCC: /*CPY abs*/
                                addr=getw();
                                temp=readmem(addr);
                                setzn(y-temp);
                                p.c=(y>=temp);
                                polltime(4);
                                break;

                                case 0xCD: /*CMP abs*/
                                addr=getw();
                                temp=readmem(addr);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(4);
                                break;

                                case 0xCE: /*DEC abs*/
                                addr=getw();
                                temp=readmem(addr)-1;
                                writemem(addr,temp+1);
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(6);
                                break;

                                case 0xD0: /*BNE*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (!p.z)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0xD1: /*CMP (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+y);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(5);
                                break;

                                case 0xD5: /*CMP zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                temp=readmem((addr+x)&0xFF);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(4);
                                break;

                                case 0xD6: /*DEC zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                temp=readmem((addr+x)&0xFF)-1;
                                writemem((addr+x)&0xFF,temp+1);
                                writemem((addr+x)&0xFF,temp);
                                setzn(temp);
                                polltime(6);
                                break;

                                case 0xD8: /*CLD*/
                                readmem(pc);
                                p.d=0;
                                polltime(2);
                                break;

                                case 0xD9: /*CMP abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+y);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(4);
                                break;

                                case 0xDD: /*CMP abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+x);
                                setzn(a-temp);
                                p.c=(a>=temp);
                                polltime(4);
                                break;

                                case 0xDE: /*DEC abs,x*/
                                addr=getw();
                                readmem(addr); addr+=x;
                                temp=readmem(addr)-1;
                                writemem(addr,temp+1);
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(7);
                                break;

                                case 0xE0: /*CPX imm*/
                                temp=readmem(pc); pc++;
                                setzn(x-temp);
                                p.c=(x>=temp);
                                polltime(3);
                                break;

                                case 0xE1: /*SBC (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                temp=readmem(addr);
                                SBC(temp);
                                polltime(6);
                                break;

                                case 0xE4: /*CPX zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                setzn(x-temp);
                                p.c=(x>=temp);
                                polltime(3);
                                break;

                                case 0xE5: /*SBC zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr);
                                SBC(temp);
                                polltime(3);
                                break;

                                case 0xE6: /*INC zp*/
                                addr=readmem(pc); pc++;
                                temp=readmem(addr)+1;
                                writemem(addr,temp-1);
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(5);
                                break;

                                case 0xE8: /*INX*/
                                readmem(pc);
                                x++;
                                setzn(x);
                                polltime(2);
                                break;

                                case 0xE9: /*SBC imm*/
                                temp=readmem(pc); pc++;
                                SBC(temp);
                                polltime(2);
                                break;

                                case 0xEA: /*NOP*/
                                readmem(pc);
                                polltime(2);
                                break;

                                case 0xEC: /*CPX abs*/
                                addr=getw();
                                temp=readmem(addr);
                                setzn(x-temp);
                                p.c=(x>=temp);
                                polltime(4);
                                break;

                                case 0xED: /*SBC abs*/
                                addr=getw();
                                temp=readmem(addr);
                                SBC(temp);
                                polltime(4);
                                break;

                                case 0xEE: /*DEC abs*/
                                addr=getw();
                                temp=readmem(addr)+1;
                                writemem(addr,temp-1);
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(6);
                                break;

                                case 0xF0: /*BEQ*/
                                offset=(signed char)readmem(pc); pc++;
                                temp=2;
                                if (p.z)
                                {
                                        readmem(pc);
                                        temp++;
                                        if ((pc&0xFF00)^((pc+offset)&0xFF00)) { readmem(pc); temp++; }
                                        pc+=offset;
                                }
                                polltime(temp);
                                break;

                                case 0xF1: /*SBC (),y*/
                                temp=readmem(pc); pc++;
                                addr=readmem(temp)+(readmem(temp+1)<<8);
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+y);
                                SBC(temp);
                                polltime(5);
                                break;

                                case 0xF5: /*SBC zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                temp=readmem((addr+x)&0xFF);
                                SBC(temp);
                                polltime(4);
                                break;

                                case 0xF6: /*INC zp,x*/
                                addr=readmem(pc); pc++;
                                readmem(addr);
                                temp=readmem((addr+x)&0xFF)+1;
                                writemem((addr+x)&0xFF,temp-1);
                                writemem((addr+x)&0xFF,temp);
                                setzn(temp);
                                polltime(6);
                                break;

                                case 0xF8: /*SED*/
                                readmem(pc);
                                p.d=1;
                                polltime(2);
                                break;

                                case 0xF9: /*SBC abs,y*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+y)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+y);
                                SBC(temp);
                                polltime(4);
                                break;

                                case 0xFD: /*SBC abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) { readmem(addr); polltime(1); }
                                temp=readmem(addr+x);
                                SBC(temp);
                                polltime(4);
                                break;

                                case 0xFE: /*INC abs,x*/
                                addr=getw();
                                readmem(addr); addr+=x;
                                temp=readmem(addr)+1;
                                writemem(addr,temp-1);
                                writemem(addr,temp);
                                setzn(temp);
                                polltime(7);
                                break;
//#if 0
                                case 0x04: //Undocumented - NOP zp
                                case 0x44:
                                case 0x64:
                                addr=readmem(pc); pc++;
                                polltime(3);
                                break;

                                case 0x07: //Undocumented - SLO zp
                                addr=readmem(pc); pc++;
                                //c=ram[addr]&0x80; //No se usa c
                                ram[addr]<<=1;
                                a|=ram[addr];
                                setzn(a);
                                polltime(5);
                                break;

                                case 0x0C: /*Undocumented - NOP abs*/
                                pc+=2;
                                polltime(4);
                                break;
                                
                                case 0x14: /*Undocumented - NOP zp,x*/
                                case 0x34:
                                case 0x54:
                                case 0x74:
                                addr=readmem(pc); pc++;
                                polltime(4);
                                break;

                                case 0x1C: /*Undocumented - NOP abs,x*/
                                pc+=2;
                                polltime(4);
                                break;
                                
                                case 0x23: /*Undocumented - RLA*/
                                break;     /*This was found in Repton 3 and
                                             looks like a mistake, so I'll
                                             ignore it for now*/

                                case 0x4B: /*Undocumented - ASR*/
                                a&=readmem(pc); pc++;
                                p.c=a&1;
                                a>>=1;
                                setzn(a);
                                polltime(2);
                                break;

                                case 0x67: /*Undocumented - RRA zp*/
                                addr=readmem(pc); pc++;
                                ram[addr]>>=1;
                                if (p.c) ram[addr]|=1;
                                temp=ram[addr];
                                ADC(temp);
                                polltime(5);
                                break;

                                case 0x80: /*Undocumented - NOP imm*/
                                readmem(pc); pc++;
                                polltime(2);
                                break;

                                case 0x83: /*SAX (,x)*/
                                temp=readmem(pc)+x; pc++;
                                readmem(addr-x);
                                addr=readmem(temp)|(readmem(temp+1)<<8);
                                writemem(addr,a&x);
                                polltime(6);
                                break;

                                case 0x87: /*Undocumented - SAX zp*/
                                addr=readmem(pc); pc++;
                                ram[addr]=a&x;
                                polltime(3);
                                break;

                                case 0x9C: /*Undocumented - SHY abs,x*/
                                addr=getw();
                                writemem(addr+x,y&((addr>>8)+1));
                                polltime(5);
                                break;

                                case 0xDA: /*Undocumented - NOP*/
//                                case 0xFA:
                                polltime(2);
                                break;

                                case 0x3C:
                                case 0xDC: /*Undocumented - NOP abs,x*/
                                addr=getw();
                                if ((addr&0xFF00)^((addr+x)&0xFF00)) polltime(1);
                                readmem(addr+x);
                                polltime(4);
                                break;
//#endif
                                default:
                switch (opcode&0xF)
                {
                        case 0xA:
                        break;
                        case 0x0:
                        case 0x2:
                        case 0x3:
                        case 0x4:
                        case 0x7:
                        case 0x9:
                        case 0xB:
                        pc++;
                        break;
                        case 0xC:
                        case 0xE:
                        case 0xF:
                        pc+=2;
                        break;
                }
/*                                allegro_exit();
                                printf("Error : Bad opcode %02X\n",opcode);
                                pc--;
                                dumpregs();
                                printf("Current ROM %02X\n",currom);
                                exit(-1);*/
                        }
/*                        if (pc==0xDB3C)
                        {
                                dumpregs();
                                exit(-1);
                        }*/
//                        if (!output && pc<0x3000) { printf("PC %04X %04X %04X\n",pc,opc,oopc); output=1; }
//                        if (oldi && !p.i) printf("I cleared at line %03i  %04X %04X %04X\n",line,pc,opc,oopc);
//                        if (!oldi && p.i) printf("I set at line %03i      %04X %04X %04X %04X %04X\n",line,pc,opc,oopc,ooopc,oooopc);
                        ins++;
//                        if (output) printf("A=%02X X=%02X Y=%02X PC=%04X S=%02X %i %02X %02X\n",a,x,y,pc,s,p.i,opcode,ula.ints);
                        if (interrupt && !p.i)
                        {
                                readmem(0xFFFE); readmem(0xFFFF);
//                                printf("Servicing interrupt %02X at line %i\n",ula.ints&ula.intc,line);
                                //if (ula.ints&ula.intc&8) printf("RTC int line %i\n",line);
                                push(pc>>8);
                                push(pc&0xFF);
                                temp=0x20;
                                if (p.c) temp|=1; if (p.z) temp|=2;
                                if (p.d) temp|=8; if (p.v) temp|=0x40;
                                if (p.n) temp|=0x80;
                                push(temp);
                                pc=readmem(0xFFFE)|(readmem(0xFFFF)<<8);
                                p.i=1;
                                polltime(7);
                        }
                }
                oldnmi=nmi;
                if (nmi && !oldnmi && !nmilock)
                {
                        readmem(0xFFFA); readmem(0xFFFB);
                        push(pc>>8);
                        push(pc&0xFF);
                        temp=0x20;
                        if (p.c) temp|=1; if (p.z) temp|=2;
                        if (p.i) temp|=4; if (p.d) temp|=8;
                        if (p.v) temp|=0x40; if (p.n) temp|=0x80;
                        push(temp);
                        pc=readmem(0xFFFA)|(readmem(0xFFFB)<<8);
                        p.i=1;
                        polltime(7);
                        nmi=0;
                        nmilock=1;
                }
                lines--;
                drawline(line);
                line++;
                #ifndef use_lib_no_tape
                 if (tapeon)
                 {
                  if (!tapelcount)
                  {
                   polltape();
                   tapelcount=tapellatch;
                  }
                  tapelcount--;
                 }
                 tapewcount--;
                 if (!tapewcount)
                 {
                   tapecountover();
                 }
                #endif 
        }
}

void load6502state(FILE *f)
{
        unsigned char temp;
        a=getc(f);
        x=getc(f);
        y=getc(f);
        s=getc(f);
        temp=getc(f);
        p.c=temp&1; p.z=temp&2;
        p.d=temp&8; p.v=temp&0x40;
        p.n=temp&0x80;
        pc=getc(f);
        pc|=(getc(f)<<8);
        fread(ram,32768,1,f);
}

void load6502stateFlash(unsigned char id)
{
 unsigned int cont=4;     
 #ifdef use_lib_log_serial
  printf("load6502stateFlash %d\n",id);
 #endif 
        unsigned char temp;
        //a=gb_list_sna_data[id][cont++];
        //x=gb_list_sna_data[id][cont++];
        //y=gb_list_sna_data[id][cont++];
        //s=gb_list_sna_data[id][cont++];
        //temp=gb_list_sna_data[id][cont++];        
        a=gb_ptr_sna_data[id][cont++];
        x=gb_ptr_sna_data[id][cont++];
        y=gb_ptr_sna_data[id][cont++];
        s=gb_ptr_sna_data[id][cont++];
        temp=gb_ptr_sna_data[id][cont++];                
        p.c=temp&1; p.z=temp&2;
        p.d=temp&8; p.v=temp&0x40;
        p.n=temp&0x80;
        //pc=gb_list_sna_data[id][cont++];
        pc=gb_ptr_sna_data[id][cont++];
        //pc|=((unsigned short int)(gb_list_sna_data[id][cont])<<8);
        pc|=((unsigned short int)(gb_ptr_sna_data[id][cont])<<8);
        cont++;
        //fread(ram,32768,1,f);
        //memcpy(ram,&gb_list_sna_data[id][cont],32768);
        memcpy(ram,&gb_ptr_sna_data[id][cont],32768);
        cont+= 32768;
 //fflush(stdout);        
}


void save6502state(FILE *f)
{
        unsigned char temp;
        putc(a,f);
        putc(x,f);
        putc(y,f);
        putc(s,f);
        temp=0x20;
        if (p.c) temp|=1; if (p.z) temp|=2;
        if (p.d) temp|=8; if (p.v) temp|=0x40;
        if (p.n) temp|=0x80;
        putc(temp,f);
        putc(pc&0xFF,f);
        putc(pc>>8,f);
        fwrite(ram,32768,1,f);
}
