//Elkulator header
#ifndef _ELK_H
 #define _ELK_H
extern int tapewcount; 
extern int tapelcount;
extern int tapellatch;
extern int countit;
extern int output;
extern int sdouble;
extern int extline;
extern int changed[2];
extern int fasttape;
 
 
 
extern int resetspeed;
extern int quit;

//JJ //6502 registers
//JJ extern unsigned char a,x,y,s;
//JJ extern unsigned short pc;
//JJ extern struct TP
//JJ {
//JJ         int c,z,i,d,v,n;
//JJ } p;

//Interrupt stuff
extern int interrupt;
extern int nmi,oldnmi,nmilock;

//Memory structures
//JJ extern unsigned char *mem[0x100];
//JJ extern int memstat[0x100];
//JJ extern unsigned char *ram,*rom;
//JJ extern unsigned char os[0x4000];
//JJ extern int writeablerom[16];
//JJ extern unsigned char currom;

//JJ //ULA
//JJ extern struct TULA
//JJ {
//JJ         unsigned char ints,intc;
//JJ         unsigned char addrlo,addrhi;
//JJ         unsigned char mode;
//JJ         unsigned short addr;
//JJ         unsigned char receivebyte;
//JJ } ula;
//JJ extern int tapeon;
//JJ extern int soundena;

//Config
extern int soundenable;
extern int pcsound,pcsoundl;
extern int mono;
extern int blur;
//JJ extern AUDIOSTREAM *as;

//Disc
extern int plus3ena,adfsena,dfsena;
extern char discname[260],discname2[260];

//Tape
extern char tapename[260];

//Video
//JJ extern BITMAP *buffer,*scrshotbuffer,*buf162;
//JJ extern PALETTE elkpal;

extern int hires,fullscreen;
#endif
