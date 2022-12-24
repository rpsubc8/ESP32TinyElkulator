//Elkulator 0.5 by Tom Walker
//ULA/Video/Sound handling
//JJ #include <allegro.h>
#include "gbConfig.h"
#include <stdio.h>
#include "elk.h"
#include "ula.h"
#include <stdio.h>
#include <string.h>
#include "gbGlobals.h"
//#include "dataFlash/gbsna.h"

inline static void put_pixel(unsigned int x, unsigned int y, unsigned char aColor)
{
 //if ((x>=320)||(y>=200))
 // return;
 gb_buffer_vga[y][x^2]= gb_color_vga[aColor];
}

void SDLClearULA()
{
 unsigned int a0,a1,a2,a3,a32;
 //for (int y=0; y<200; y++)
 //{
 // for (int x=0; x<320; x++)
 // {
 //  //put_pixel(x,y,gb_color_vga[0]);
 //  put_pixel(x,y,0);
 // }
 //}

 a0= a1= a2= a3= gb_color_vga[0];
 a32= a2 | (a3<<8) | (a0<<16) | (a1<<24);
 for (int y=0;y<200;y++)        
 {
  for (int x=0; x<80; x++)
  {//80x4 = 320
   gb_buffer_vga32[y][x]= a32;
  }
 }
}

#ifdef use_lib_rom_flash
 void CositasROM(unsigned char cRom, unsigned int cVal)
 {
  unsigned char idRom= cRom;
  unsigned short int offsRom= ((cVal&0x3F)<<8); 
  //printf("ROM:%d Val:%02X addr:%08X\n",cRom,cVal,offsRom);
  //fflush(stdout);
  
  mem[cVal] = &gb_array_rom[idRom][offsRom];
 }
#endif 

void fast_hline(int line)
{//320 pixels de negro
 unsigned int a32= gb_color_vga[0];
 unsigned int offs=0;

 a32= a32|(a32<<8)|(a32<<16)|(a32<<24);

 for (unsigned char i=0;i<40;i++)
 {  
  gb_buffer_vga32[line][offs++]= a32;
 }
}

void hline(short int x1, short int y, short int x2, unsigned char color)
{
 //if ((y<0)||(y>300)||(x1<0)||(x1>=320)||(x2<0)||(x2>=320))
 // return;
 for (unsigned int i=x1;i<x2;i++)
 {
  //portESP32 gb_buffer_vga[y][i]= color;
  put_pixel(i,y,color);
 }
}


//int sdouble;
//int fasttape;
int tapewcount=130,tapew=0;
int drawfirst;
int checksound;
FILE *soundf;
int fps,frames;
void secondtimer()
{
        fps=frames;
        frames=0;
}

//JJ BITMAP *buffer,*buff2,*scrshotbuffer,*buf16,*buf162;


unsigned char gb_keys[2][14][4]=
{
        {
                {1,1,0,1},
                {1,1,1,1},
                {1,1,1,0},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1}
        },
        {
                {1,1,0,1},
                {1,1,1,1},
                {1,1,1,0},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1},
                {1,1,1,1}
        }
};

/*JJ Teclado Allegro
int keys[2][14][4]=
{
        {
                {KEY_RIGHT,KEY_END,0,KEY_SPACE},
                {KEY_LEFT,KEY_DOWN,KEY_ENTER,KEY_DEL},
                {KEY_MINUS,KEY_UP,KEY_QUOTE,0},
                {KEY_0,KEY_P,KEY_COLON,KEY_SLASH},
                {KEY_9,KEY_O,KEY_L,KEY_STOP},
                {KEY_8,KEY_I,KEY_K,KEY_COMMA},
                {KEY_7,KEY_U,KEY_J,KEY_M},
                {KEY_6,KEY_Y,KEY_H,KEY_N},
                {KEY_5,KEY_T,KEY_G,KEY_B},
                {KEY_4,KEY_R,KEY_F,KEY_V},
                {KEY_3,KEY_E,KEY_D,KEY_C},
                {KEY_2,KEY_W,KEY_S,KEY_X},
                {KEY_1,KEY_Q,KEY_A,KEY_Z},
                {KEY_ESC,KEY_ALT,KEY_LCONTROL,KEY_LSHIFT}
        },
        {
                {KEY_RIGHT,KEY_END,0,KEY_SPACE},
                {KEY_LEFT,KEY_DOWN,KEY_ENTER,KEY_BACKSPACE},
                {KEY_MINUS,KEY_UP,KEY_QUOTE,0},
                {KEY_0,KEY_P,KEY_COLON,KEY_SLASH},
                {KEY_9,KEY_O,KEY_L,KEY_STOP},
                {KEY_8,KEY_I,KEY_K,KEY_COMMA},
                {KEY_7,KEY_U,KEY_J,KEY_M},
                {KEY_6,KEY_Y,KEY_H,KEY_N},
                {KEY_5,KEY_T,KEY_G,KEY_B},
                {KEY_4,KEY_R,KEY_F,KEY_V},
                {KEY_3,KEY_E,KEY_D,KEY_C},
                {KEY_2,KEY_W,KEY_S,KEY_X},
                {KEY_1,KEY_Q,KEY_A,KEY_Z},
                {KEY_ESC,KEY_TILDE,KEY_RCONTROL,KEY_RSHIFT}
        }
};
*/

unsigned char pal[16];
int palwritenum=0,palwritenum2=0;
//JJ PALETTE elkpal =
//JJ {
//JJ       {0,0,0},
//JJ       {63,0,0},
//JJ       {0,63,0},
//JJ       {63,63,0},
//JJ       {0,0,63},
//JJ       {63,0,63},
//JJ       {0,63,63},
//JJ       {63,63,63},
//JJ };

//JJ PALETTE monopal =
//JJ {
//JJ       {0,0,0},
//JJ       {16,16,16},
//JJ       {24,24,24},
//JJ       {40,40,40},
//JJ       {8,8,8},
//JJ       {24,24,24},
//JJ       {32,32,32},
//JJ       {56,56,56},
//JJ };

//JJ fixed soundcount,soundlimit;
unsigned char soundstat=0;
//JJ sonido int soundon[313];

/*No se usa por ahora. Revisa rpuntero p
void mixsound(int wait)
{
        unsigned char *p;
        int c;
        p=0;
        if (tapeon && fasttape)
        {
//JJ                 p=(unsigned char *)get_audio_stream_buffer(as);
//JJ                 if (p)
//JJ                 {
//JJ                         memset(p,0,625);
//JJ                         free_audio_stream_buffer(as);
//JJ                 }
                return;
        }
//JJ         while (!p) p=(unsigned char *)get_audio_stream_buffer(as);
//JJ         if (p)
//JJ         {
//JJ                 for (c=0;c<625;c++)
//JJ                 {
//JJ                         soundcount+=0x20000;
//JJ                         if (soundcount>=soundlimit)
//JJ                         {
//JJ                                 soundcount=0;
//JJ                                 soundstat^=0xFF;
//JJ                         }
//JJ                         if (soundlimit<0x20000) p[c]=(soundon[c>>1])?0x7F:0;
//JJ                         else if (soundon[c>>1]) p[c]=soundstat;
//JJ                         else                    p[c]=0;
//JJ                 }
//JJ                 free_audio_stream_buffer(as);
//JJ         }
}
*/

void initsound()
{
//JJ         if (install_sound(DIGI_AUTODETECT,MIDI_NONE,0)) { soundenable=0; return; }
//JJ         if (soundenable) as=play_audio_stream(625,8,0,31250,255,127);
//        soundf=fopen("sound.pcm","wb");
}

void intula(unsigned char val)
{//34 veces
//gb_cont_loop++;
     
        ula.ints|=val;
        if ((ula.ints&ula.intc)&0x7E) ula.ints|=1;
        else                          ula.ints&=~1;
        interrupt=ula.ints&1;
}

unsigned char readkeys(unsigned short addr)
{//4 veces
//gb_cont_loop++;         
        int c;
        unsigned char temp=0;
        for (c=0;c<14;c++)
        {
                if (!(addr&(1<<c)))
                {
//JJ                        if (key[keys[0][c][0]]) temp|=1;
//JJ                        if (key[keys[0][c][1]]) temp|=2;
//JJ                        if (key[keys[0][c][2]]) temp|=4;
//JJ                        if (key[keys[0][c][3]]) temp|=8;
//JJ                        if (key[keys[1][c][0]]) temp|=1;
//JJ                        if (key[keys[1][c][1]]) temp|=2;
//JJ                        if (key[keys[1][c][2]]) temp|=4;
//JJ                        if (key[keys[1][c][3]]) temp|=8;

                        if (gb_keys[0][c][0]) temp|=1;
                        if (gb_keys[0][c][1]) temp|=2;
                        if (gb_keys[0][c][2]) temp|=4;
                        if (gb_keys[0][c][3]) temp|=8;
                        if (gb_keys[1][c][0]) temp|=1;
                        if (gb_keys[1][c][1]) temp|=2;
                        if (gb_keys[1][c][2]) temp|=4;
                        if (gb_keys[1][c][3]) temp|=8;
                }
        }
        return temp;
}

#ifdef use_lib_optimize_lookup_32bits
 unsigned int lookup[256][9],lookup2[256][9][2];
#else
 unsigned long lookup[256][9],lookup2[256][9][2];
#endif 

void makelookup4()
{//10 a 12 veces
//gb_cont_loop++;     
        int c,d,temp=0;
        for (c=0;c<256;c++)
        {
                lookup[c][0]=lookup[c][1]=0;
                if (c&0x80) lookup[c][0]|=pal[8];       else lookup[c][0]|=pal[0];
                if (c&0x40) lookup[c][0]|=(pal[8]<<8);  else lookup[c][0]|=(pal[0]<<8);
                if (c&0x20) lookup[c][0]|=(pal[8]<<16); else lookup[c][0]|=(pal[0]<<16);
                if (c&0x10) lookup[c][0]|=(pal[8]<<24); else lookup[c][0]|=(pal[0]<<24);
                if (c&0x8) lookup[c][1]|=pal[8];       else lookup[c][1]|=pal[0];
                if (c&0x4) lookup[c][1]|=(pal[8]<<8);  else lookup[c][1]|=(pal[0]<<8);
                if (c&0x2) lookup[c][1]|=(pal[8]<<16); else lookup[c][1]|=(pal[0]<<16);
                if (c&0x1) lookup[c][1]|=(pal[8]<<24); else lookup[c][1]|=(pal[0]<<24);
                lookup[c][2]=0;
                if (c&0x80) lookup[c][2]|=pal[8];       else lookup[c][2]|=pal[0];
                if (c&0x20) lookup[c][2]|=(pal[8]<<8);  else lookup[c][2]|=(pal[0]<<8);
                if (c&0x08) lookup[c][2]|=(pal[8]<<16); else lookup[c][2]|=(pal[0]<<16);
                if (c&0x02) lookup[c][2]|=(pal[8]<<24); else lookup[c][2]|=(pal[0]<<24);
                
                lookup[c][3]=lookup[c][4]=0;
                switch (c&0x88)
                {
                        case 0x00: lookup[c][3]|=pal[0]|(pal[0]<<8); break;
                        case 0x08: lookup[c][3]|=pal[2]|(pal[2]<<8); break;
                        case 0x80: lookup[c][3]|=pal[8]|(pal[8]<<8); break;
                        case 0x88: lookup[c][3]|=pal[10]|(pal[10]<<8); break;
                }
                switch (c&0x44)
                {
                        case 0x00: lookup[c][3]|=(pal[0]<<16)|(pal[0]<<24); break;
                        case 0x04: lookup[c][3]|=(pal[2]<<16)|(pal[2]<<24); break;
                        case 0x40: lookup[c][3]|=(pal[8]<<16)|(pal[8]<<24); break;
                        case 0x44: lookup[c][3]|=(pal[10]<<16)|(pal[10]<<24); break;
                }
                switch (c&0x22)
                {
                        case 0x00: lookup[c][4]|=pal[0]|(pal[0]<<8); break;
                        case 0x02: lookup[c][4]|=pal[2]|(pal[2]<<8); break;
                        case 0x20: lookup[c][4]|=pal[8]|(pal[8]<<8); break;
                        case 0x22: lookup[c][4]|=pal[10]|(pal[10]<<8); break;
                }
                switch (c&0x11)
                {
                        case 0x00: lookup[c][4]|=(pal[0]<<16)|(pal[0]<<24); break;
                        case 0x01: lookup[c][4]|=(pal[2]<<16)|(pal[2]<<24); break;
                        case 0x10: lookup[c][4]|=(pal[8]<<16)|(pal[8]<<24); break;
                        case 0x11: lookup[c][4]|=(pal[10]<<16)|(pal[10]<<24); break;
                }
                lookup[c][5]=0;
                switch (c&0x88)
                {
                        case 0x00: lookup[c][5]|=pal[0]; break;
                        case 0x08: lookup[c][5]|=pal[2]; break;
                        case 0x80: lookup[c][5]|=pal[8]; break;
                        case 0x88: lookup[c][5]|=pal[10]; break;
                }
                switch (c&0x44)
                {
                        case 0x00: lookup[c][5]|=(pal[0]<<8); break;
                        case 0x04: lookup[c][5]|=(pal[2]<<8); break;
                        case 0x40: lookup[c][5]|=(pal[8]<<8); break;
                        case 0x44: lookup[c][5]|=(pal[10]<<8); break;
                }
                switch (c&0x22)
                {
                        case 0x00: lookup[c][5]|=(pal[0]<<16); break;
                        case 0x02: lookup[c][5]|=(pal[2]<<16); break;
                        case 0x20: lookup[c][5]|=(pal[8]<<16); break;
                        case 0x22: lookup[c][5]|=(pal[10]<<16); break;
                }
                switch (c&0x11)
                {
                        case 0x00: lookup[c][5]|=(pal[0]<<24); break;
                        case 0x01: lookup[c][5]|=(pal[2]<<24); break;
                        case 0x10: lookup[c][5]|=(pal[8]<<24); break;
                        case 0x11: lookup[c][5]|=(pal[10]<<24); break;
                }

                lookup[c][6]=0;
                temp=0;
                if (c&2) temp|=1;
                if (c&8) temp|=2;
                if (c&0x20) temp|=4;
                if (c&0x80) temp|=8;
                lookup[c][6]|=pal[temp]|(pal[temp]<<8);
                temp=0;
                if (c&1) temp|=1;
                if (c&4) temp|=2;
                if (c&0x10) temp|=4;
                if (c&0x40) temp|=8;
                lookup[c][6]|=(pal[temp]<<16)|(pal[temp]<<24);
                for (d=0;d<9;d++)
                {
                        lookup2[c][d][0]=lookup[c][d]&0xFF;
                        lookup2[c][d][0]|=(lookup[c][d]&0xFF00)<<8;
                        lookup2[c][d][1]=(lookup[c][d]&0xFF0000)>>16;
                        lookup2[c][d][1]|=(lookup[c][d]&0xFF000000)>>8;
                        lookup2[c][d][0]|=(lookup2[c][d][0]<<8);
                        lookup2[c][d][1]|=(lookup2[c][d][1]<<8);
                }
        }
}

unsigned short pc;
//int extline;
void writeula(unsigned short addr, unsigned char val)
{//8 veces
//gb_cont_loop++;          
        int c;
        switch (addr&0xF)
        {
                case 0:
                ula.intc=val;
//                printf("INTC %02X\n",val);
                break;
                case 2:
                ula.addrlo=val&0xE0;
                break;
                case 3:
                ula.addrhi=val&0x3F;
                break;
                case 4:
//                printf("FE04 write %02X %04X %i\n",val,pc,extline);
                ula.ints&=~0x30;
                intula(0);
                tapewcount=130;
                return;
                case 5:
                if (val&0x10) ula.ints&=~4;
                if (val&0x20) ula.ints&=~8;
                if (val&0x40)
                {
                        ula.ints&=~0x40;
//                        printf("High tone cleared\n");
                }
                intula(0);
                val&=0xF;
                if (currom<8 || currom>11 || val>7)
                {
                        currom=val;
                        for (c=0x80;c<0x0C0;c++)
                        {
                         #ifdef use_lib_rom_flash
                          CositasROM(currom,c);
                         #else   
                          mem[c]=(unsigned char *)(rom+(currom<<14)+((c&0x3F)<<8));                                                  
                         #endif 
                        }
                        //CositasROM(currom,c);
                        
                        if (val==8 || val==9)       for (c=0x80;c<0xC0;c++) memstat[c]=2;
                        else if (val==3 && dfsena)  for (c=0x80;c<0xC0;c++) memstat[c]=0;
                        else                        for (c=0x80;c<0xC0;c++) memstat[c]=1;
                }               
                break;
                case 6:
                pcsoundl=31250/(val+1);
//JJ                soundlimit=(val<<16);
//JJ                if (soundcount>soundlimit) soundcount=0;
//JJ                if (pcsound && soundena) sound(pcsoundl);
//                else                     nosound();
//                soundcount=0;
                break;
                case 7:
                if ((((val&6)==2)?1:0)!=soundena)
                {
                        soundena=((val&6)==2)?1:0;
//JJ                        if (pcsound && soundena) sound(pcsoundl);
//JJ                        else                     nosound();
                }
                if ((val&6)==4) tapew=1;
                else            tapew=0;
//                printf("%i %i\n",val&6,tapew);
                if (tapeon && !(val&64))
                   resetspeed=1;
                tapeon=val&64;
                if (((val>>3)&7)!=ula.mode)
                {
                        ula.mode=(val>>3)&7;
                        switch (ula.mode)
                        {
                                case 0: case 3: case 4: case 6: makelookup4(); break;
                                case 1: case 5: makelookup4(); break;
                                case 2: makelookup4(); break;
                        }
                }
                break;
                case 0x8: case 0xA: case 0xC: case 0xE:
//                printf("%04X write %02X %04X %i\n",addr,val,pc,extline);
                switch ((addr>>1)&3)
                {
                        case 0:
                        pal[0]&=~4;
                        pal[2]&=~4;
                        pal[8]&=~6;
                        pal[10]&=~6;
                        if (!(val&0x10)) pal[0]|=4;
                        if (!(val&0x20)) pal[2]|=4;
                        if (!(val&0x40)) pal[8]|=4;
                        if (!(val&0x80)) pal[10]|=4;
                        if (!(val&0x4)) pal[8]|=2;
                        if (!(val&0x8)) pal[10]|=2;
                        break;
                        case 1:
                        pal[4]&=~4;
                        pal[6]&=~4;
                        pal[12]&=~6;
                        pal[14]&=~6;
                        if (!(val&0x10)) pal[4]|=4;
                        if (!(val&0x20)) pal[6]|=4;
                        if (!(val&0x40)) pal[12]|=4;
                        if (!(val&0x80)) pal[14]|=4;
                        if (!(val&0x4)) pal[12]|=2;
                        if (!(val&0x8)) pal[14]|=2;
                        break;
                        case 2:
                        pal[5]&=~4;
                        pal[7]&=~4;
                        pal[13]&=~6;
                        pal[15]&=~6;
                        if (!(val&0x10)) pal[5]|=4;
                        if (!(val&0x20)) pal[7]|=4;
                        if (!(val&0x40)) pal[13]|=4;
                        if (!(val&0x80)) pal[15]|=4;
                        if (!(val&0x4)) pal[13]|=2;
                        if (!(val&0x8)) pal[15]|=2;
                        break;
                        case 3:
                        pal[1]&=~4;
                        pal[3]&=~4;
                        pal[9]&=~6;
                        pal[11]&=~6;
                        if (!(val&0x10)) pal[1]|=4;
                        if (!(val&0x20)) pal[3]|=4;
                        if (!(val&0x40)) pal[9]|=4;
                        if (!(val&0x80)) pal[11]|=4;
                        if (!(val&0x4)) pal[9]|=2;
                        if (!(val&0x8)) pal[11]|=2;
                        break;
                }
                makelookup4();
                break;
                case 0x9: case 0xB: case 0xD: case 0xF:
//                printf("%04X write %02X %04X %i\n",addr,val,pc,extline);
                switch ((addr>>1)&3)
                {
                        case 0:
                        pal[0]&=~3;
                        pal[2]&=~3;
                        pal[8]&=~1;
                        pal[10]&=~1;
                        if (!(val&1)) pal[0]|=1;
                        if (!(val&2)) pal[2]|=1;
                        if (!(val&4)) pal[8]|=1;
                        if (!(val&8)) pal[10]|=1;
                        if (!(val&16)) pal[0]|=2;
                        if (!(val&32)) pal[2]|=2;
                        break;
                        case 1:
                        pal[4]&=~3;
                        pal[6]&=~3;
                        pal[12]&=~1;
                        pal[14]&=~1;
                        if (!(val&1)) pal[4]|=1;
                        if (!(val&2)) pal[6]|=1;
                        if (!(val&4)) pal[12]|=1;
                        if (!(val&8)) pal[14]|=1;
                        if (!(val&16)) pal[4]|=2;
                        if (!(val&32)) pal[6]|=2;
                        break;
                        case 2:
                        pal[5]&=~3;
                        pal[7]&=~3;
                        pal[13]&=~1;
                        pal[15]&=~1;
                        if (!(val&1)) pal[5]|=1;
                        if (!(val&2)) pal[7]|=1;
                        if (!(val&4)) pal[13]|=1;
                        if (!(val&8)) pal[15]|=1;
                        if (!(val&16)) pal[5]|=2;
                        if (!(val&32)) pal[7]|=2;
                        break;
                        case 3:
                        pal[1]&=~3;
                        pal[3]&=~3;
                        pal[9]&=~1;
                        pal[11]&=~1;
                        if (!(val&1)) pal[1]|=1;
                        if (!(val&2)) pal[3]|=1;
                        if (!(val&4)) pal[9]|=1;
                        if (!(val&8)) pal[11]|=1;
                        if (!(val&16)) pal[1]|=2;
                        if (!(val&32)) pal[3]|=2;
                        break;
                }
                makelookup4();
                break;
        }
}

unsigned char readula(unsigned short addr)
{//2 veces
//         gb_cont_loop++;     
 unsigned char temp;
 switch (addr&0xF)
 {
  case 0:
   temp=ula.ints;
   ula.ints&=~2;
   return temp;
  case 4:
   ula.ints&=~0x10;
   intula(0);
   return ula.receivebyte;
  default:
   return 0; //para quitar warning
 }
}

void tapecountover()
{     
        tapewcount=13;
        if (tapew) ula.ints|=0x30;
        else       ula.ints|=0x20;
        intula(0);
//        printf("Tape count over %02X %i\n",ula.ints,extline);
}

void receive(unsigned char val)
{//0 a 4 veces
//gb_cont_loop++;          
      ula.receivebyte=val;
//      printf("Received %02X\n",ula.receivebyte);
      intula(0x10);
}

void genpal()
{
        int c,d;
        for (c=0;c<8;c++)
        {
                for (d=0;d<8;d++)
                {
//JJ                        elkpal[c+(d<<3)+128].r=elkpal[c&7].r>>1;
//JJ                        elkpal[c+(d<<3)+128].r+=elkpal[d&7].r>>1;
//JJ                        elkpal[c+(d<<3)+128].g=elkpal[c&7].g>>1;
//JJ                        elkpal[c+(d<<3)+128].g+=elkpal[d&7].g>>1;
//JJ                        elkpal[c+(d<<3)+128].b=elkpal[c&7].b>>1;
//JJ                        elkpal[c+(d<<3)+128].b+=elkpal[d&7].b>>1;
//JJ                        monopal[c+(d<<3)+128].r=monopal[c&7].r>>1;
//JJ                        monopal[c+(d<<3)+128].r+=monopal[d&7].r>>1;
//JJ                        monopal[c+(d<<3)+128].g=monopal[c&7].g>>1;
//JJ                        monopal[c+(d<<3)+128].g+=monopal[d&7].g>>1;
//JJ                        monopal[c+(d<<3)+128].b=monopal[c&7].b>>1;
//JJ                        monopal[c+(d<<3)+128].b+=monopal[d&7].b>>1;

//portESP32                 elkpal[c+(d<<3)+128].r=elkpal[c&7].r>>1;
//portESP32                 elkpal[c+(d<<3)+128].r+=elkpal[d&7].r>>1;
//portESP32                 elkpal[c+(d<<3)+128].g=elkpal[c&7].g>>1;
//portESP32                 elkpal[c+(d<<3)+128].g+=elkpal[d&7].g>>1;
//portESP32                 elkpal[c+(d<<3)+128].b=elkpal[c&7].b>>1;
//portESP32                 elkpal[c+(d<<3)+128].b+=elkpal[d&7].b>>1;



                }
        }
}

void updateres()
{
//JJ        if (hires==2) set_color_depth(16);
//JJ        else          set_color_depth(8);
//JJ        if (hires) set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);
//JJ        else       set_gfx_mode(GFX_AUTODETECT_WINDOWED,400,300,0,0);
//JJ        else       set_gfx_mode(GFX_MODEX,400,300,0,0);
//JJ        if (mono) set_palette(monopal);
//JJ        else      set_palette(elkpal);
//JJ        clear(buffer);
//JJ        clear(buff2);
//JJ        clear(buf16);
}

void initula()
{
//JJ        elkpal[255].r=elkpal[255].g=elkpal[255].b=63;
//JJ        monopal[255].r=monopal[255].g=monopal[255].b=63;
//JJ        allegro_init();
//JJ        set_window_title("SDL Port ESP32 Acorn Electron");
//JJ        set_color_depth(16);
//JJ        buf16=create_bitmap(320,256);
//JJ        buf162=create_bitmap(640,512);
//JJ no la necesito        Init_2xSaI(16);
//JJ        set_color_depth(8);
//JJ        if (hires) set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0);
//JJ        else       set_gfx_mode(GFX_AUTODETECT_WINDOWED,400,300,0,0);
//JJ        else       set_gfx_mode(GFX_MODEX,400,300,0,0);
        genpal();
//JJ        if (mono) set_palette(monopal);
//JJ        else      set_palette(elkpal);
//JJ        install_keyboard();
//JJ        buffer=create_bitmap(640,512);
//JJ        clear(buffer);
//JJ        buff2=create_bitmap(640,512);
//JJ        clear(buff2);
//JJ        scrshotbuffer=create_bitmap(320,256);
//JJ        install_timer();
//JJ        install_int_ex(secondtimer,MSEC_TO_TIMER(1000));
        ula.ints=2;
        ula.mode=6;
}

/* No se usa por ahora
void fadepal()
{
        int c;
//JJ        for (c=0;c<128;c++) elkpal[c].r>>=1;
//JJ        for (c=0;c<128;c++) elkpal[c].g>>=1;
//JJ        for (c=0;c<128;c++) elkpal[c].b>>=1;
//JJ        for (c=0;c<128;c++) monopal[c].r>>=1;
//JJ        for (c=0;c<128;c++) monopal[c].g>>=1;
//JJ        for (c=0;c<128;c++) monopal[c].b>>=1;
//JJ        set_palette(elkpal);
//JJ        if (mono) set_palette(monopal);
//JJ        else      set_palette(elkpal);
}
*/

/*No se usa por ahora
void restorepal()
{
        int c;
//JJ        for (c=0;c<128;c++) elkpal[c].r<<=1;
//JJ        for (c=0;c<128;c++) elkpal[c].g<<=1;
//JJ        for (c=0;c<128;c++) elkpal[c].b<<=1;
//JJ        for (c=0;c<128;c++) monopal[c].r<<=1;
//JJ        for (c=0;c<128;c++) monopal[c].g<<=1;
//JJ        for (c=0;c<128;c++) monopal[c].b<<=1;
//JJ        if (mono) set_palette(monopal);
//JJ        else      set_palette(elkpal);
}
*/

int sc;
int tapeframe=0;


void drawline(int line)
{//313
//gb_cont_loop++;     
 unsigned char drawBitluni=1;
 unsigned int offsX=0;
 unsigned int a0,a1,a2,a3;
 unsigned int a32; 
#ifdef use_lib_320x200
 int paramLine= line; //guardo la linea
#endif 

        int x,y;
        unsigned char val;
        unsigned short addr;
        //JJ sonido soundon[line]=soundena;
        if (!line) 
        {
                if (tapeon && fasttape)
                {
                        tapeframe++;
                        if (tapeframe==50)
                        {
                                tapeframe=0;
                                drawfirst=1;
                        }
                }
                else
                   drawfirst=1;
                sc=0;
                ula.addr=(ula.addrlo<<1)|(ula.addrhi<<9);
        }
        if (drawfirst)
        {
                              
           #ifdef use_lib_320x200
            // Funciona reduccion escala 320x200 perfecto
            
            if (gb_use_video_scale == 0)
            {
             drawBitluni = ((line>=0)&&(line<200)) ? 1: 0;
            }
            else
            {
             if ((line>=0) && (line<256))
             {
              if ((line % 10 == 8)||(line % 10 == 9))
              {//No dibujo linea             
               drawBitluni= 0;
              }
              else
              {
               unsigned int aRow= (line/10);
               unsigned int aOffset = (line%10);
 
               if ((aRow>=20) && (aRow<26))
               {
                //printf("aRow %d\n",aRow); 
                if (line % 10 == 7)
                {//No dibujo linea
                 drawBitluni= 0;
                }
                else
                {
                 line= (160) + ((aRow-20)*7) + aOffset; //(20*8) + ((aRow-20)*7) + aOffset;
                 if (line>199)
                 {
                  //printf("line %d\n",line);
                  drawBitluni= 0;
                 }
                }
               }
               else
               {
                line= (aRow<<3)+aOffset; //(aRow*8)+aOffset;
               }
              }
             }
            }//fin gb_use_video_Scale
            
            
            //drawBitluni = ((line>=0)&&(line<200)) ? 1: 0;
           #endif
                                 
        if (line<256)
        {
                     
         gb_ula_mode_cur = (ula.mode|(hires&8));
         if (gb_ula_mode_cur != gb_ula_mode_last)
         {
          #ifdef use_lib_log_serial
           printf("Video:%d\n",gb_ula_mode_cur);
          #endif 
          //fflush(stdout);

          gb_ula_mode_last = gb_ula_mode_cur;
         }

         if (drawBitluni == 1)
         {                                        
                switch (ula.mode|(hires&8))
                {
                        case 0:
                         if (sc&8)
                         {
//JJ                           hline(buffer,0,line,320,0);
                          //hline(0,line,320,0);
                          fast_hline(line);
                         }
                         else
                         {
                                addr=ula.addr+sc;
                                offsX= 0;
                                for (x=0;x<80;x++)
                                {
                                        if (addr&0x8000) addr-=0x5000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][2];                                          
                                        //portESP32 gb_buffer_vga[line][(x*4)]= (lookup[val][2] & 0xFF);
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]= (lookup[val][2]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][2]>>16)& 0xFF;                                        
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][2]>>24)& 0xFF;

                                        //put_pixel(offsX++ ,line, (lookup[val][2] & 0xFF));
                                        //put_pixel(offsX++ ,line, (lookup[val][2]>>8)& 0xFF);
                                        //put_pixel(offsX++ ,line, (lookup[val][2]>>16)& 0xFF);                                        
                                        //put_pixel(offsX++ ,line, (lookup[val][2]>>24)& 0xFF);
                                        
                                        a0= (lookup[val][2] & 0xFF);
                                        a1= ((lookup[val][2]>>8)& 0xFF);
                                        a2= ((lookup[val][2]>>16)& 0xFF);                                        
                                        a3= ((lookup[val][2]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;                                        
                                }
                         }
                        break;
                        case 1:         
                         if (sc&8)
                         {
//JJ                           hline(buffer,0,line,320,0);
                          //hline(0,line,320,0);
                          fast_hline(line);
                         }
                         else
                         {
                                addr=ula.addr+sc;
                                offsX=0;
                                for (x=0;x<80;x++)
                                {
                                        if (addr&0x8000) addr-=0x5000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][5];
                                        //portESP32 gb_buffer_vga[line][(x*4)]=lookup[val][5] & 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]=(lookup[val][5]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][5]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][5]>>24)& 0xFF;

                                        //put_pixel(offsX++ ,line ,lookup[val][5] & 0xFF);
                                        //put_pixel(offsX++ ,line ,(lookup[val][5]>>8)& 0xFF);
                                        //put_pixel(offsX++ ,line ,(lookup[val][5]>>16)& 0xFF);
                                        //put_pixel(offsX++ ,line ,(lookup[val][5]>>24)& 0xFF);                                        

                                        a0= (lookup[val][5] & 0xFF);
                                        a1= ((lookup[val][5]>>8)& 0xFF);
                                        a2= ((lookup[val][5]>>16)& 0xFF);
                                        a3= ((lookup[val][5]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;
                                }
                         }
                        break;
                        case 2:         
                         if (sc&8)
                         {
//JJ                           hline(buffer,0,line,320,0);
                          //hline(0,line,320,0);
                          fast_hline(line);
                         }
                         else
                         {
                                addr=ula.addr+sc;
                                offsX=0;
                                for (x=0;x<80;x++)
                                {
                                        if (addr&0x8000) addr-=0x5000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][6];
                                        //portESP32 gb_buffer_vga[line][(x*4)]=lookup[val][6] & 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]=(lookup[val][6]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][6]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][6]>>24)& 0xFF;                                        

                                        /*
                                        put_pixel(offsX++ ,line,lookup[val][6] & 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][6]>>8)& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][6]>>16)& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][6]>>24)& 0xFF);
                                        */

                                        a0= (lookup[val][6] & 0xFF);
                                        a1= ((lookup[val][6]>>8)& 0xFF);
                                        a2= ((lookup[val][6]>>16)& 0xFF);
                                        a3= ((lookup[val][6]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;

                                }
                         }
                        break;
                        case 3:
                         #ifdef use_lib_320x200
                          if (sc&8 || line>194)
                         #else
                          if (sc&8 || line>249)                          
                         #endif 
                          {
//JJ                           hline(buffer,0,line,320,0);
                           //hline(0,line,320,0);
                           fast_hline(line);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                offsX=0;
                                for (x=0;x<80;x++)
                                {
                                        if (addr&0x8000) addr-=0x4000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][2];
                                        //portESP32 gb_buffer_vga[line][(x*4)]=lookup[val][2] & 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]=(lookup[val][2]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][2]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][2]>>24)& 0xFF;

                                        //put_pixel(offsX++ ,line,lookup[val][2] & 0xFF);
                                        //put_pixel(offsX++ ,line,(lookup[val][2]>>8)& 0xFF);
                                        //put_pixel(offsX++ ,line,(lookup[val][2]>>16)& 0xFF);
                                        //put_pixel(offsX++ ,line,(lookup[val][2]>>24)& 0xFF);

                                        a0= (lookup[val][2] & 0xFF);
                                        a1= ((lookup[val][2]>>8)& 0xFF);
                                        a2= ((lookup[val][2]>>16)& 0xFF);
                                        a3= ((lookup[val][2]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;                                        
                                }
                          }
                         break;
                        case 4:         
                          if (sc&8)
                          {
//JJ                           hline(buffer,0,line,320,0);
                           //hline(0,line,320,0);
                           fast_hline(line);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                offsX=0;
                                for (x=0;x<80;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x2800;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][0];
//JJ                                        ((unsigned long *)buffer->line[line])[x+1]=lookup[val][1];
//                                        gb_buffer_vga[line][x]=lookup[val][0];
//                                        gb_buffer_vga[line][x+1]=lookup[val][1];
                                        
                                        //portESP32 gb_buffer_vga[line][(x*4)]=(lookup[val][0])& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]=(lookup[val][0]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][0]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][0]>>24)& 0xFF;
                                        
                                        //portESP32 gb_buffer_vga[line][(x*4)+4]=(lookup[val][1])& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+5]=(lookup[val][1]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+6]=(lookup[val][1]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+7]=(lookup[val][1]>>24)& 0xFF;

                                        //put_pixel((x*4),line,(lookup[val][0])& 0xFF);
                                        //put_pixel((x*4)+1,line,(lookup[val][0]>>8)& 0xFF);
                                        //put_pixel((x*4)+2,line,(lookup[val][0]>>16)& 0xFF);
                                        //put_pixel((x*4)+3,line,(lookup[val][0]>>24)& 0xFF);
                                        
                                        //put_pixel((x*4)+4,line,(lookup[val][1])& 0xFF);
                                        //put_pixel((x*4)+5,line,(lookup[val][1]>>8)& 0xFF);
                                        //put_pixel((x*4)+6,line,(lookup[val][1]>>16)& 0xFF);
                                        //put_pixel((x*4)+7,line,(lookup[val][1]>>24)& 0xFF);


                                        a0= ((lookup[val][0])& 0xFF);
                                        a1= ((lookup[val][0]>>8)& 0xFF);
                                        a2= ((lookup[val][0]>>16)& 0xFF);
                                        a3= ((lookup[val][0]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;
                                        
                                        a0= ((lookup[val][1])& 0xFF);
                                        a1= ((lookup[val][1]>>8)& 0xFF);
                                        a2= ((lookup[val][1]>>16)& 0xFF);
                                        a3= ((lookup[val][1]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;
                                }
                          }
                         break;
                        case 5:         
                         if (sc&8)
                         {
//JJ                           hline(buffer,0,line,320,0);
                           //hline(0,line,320,0);
                           fast_hline(line);
                         }
                         else
                         {
                                addr=ula.addr+sc;
                                offsX=0;
                                for (x=0;x<80;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x2800;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][3];
//JJ                                        ((unsigned long *)buffer->line[line])[x+1]=lookup[val][4];
//                                        gb_buffer_vga[line][x]=lookup[val][3];
//                                        gb_buffer_vga[line][x+1]=lookup[val][4];
                                        
                                        //portESP32 gb_buffer_vga[line][(x*4)]=(lookup[val][3])& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]=(lookup[val][3]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][3]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][3]>>24)& 0xFF;
                                        
                                        //portESP32 gb_buffer_vga[line][(x*4)+4]=(lookup[val][4])& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+5]=(lookup[val][4]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+6]=(lookup[val][4]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+7]=(lookup[val][4]>>24)& 0xFF;


                                        /*
                                        put_pixel(offsX++ ,line,(lookup[val][3])& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][3]>>8)& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][3]>>16)& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][3]>>24)& 0xFF);                                        
                                        
                                        put_pixel(offsX++ ,line,(lookup[val][4])& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][4]>>8)& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][4]>>16)& 0xFF);
                                        put_pixel(offsX++ ,line,(lookup[val][4]>>24)& 0xFF);
                                        */

                                        a0=  ((lookup[val][3])& 0xFF);
                                        a1=  ((lookup[val][3]>>8)& 0xFF);
                                        a2=  ((lookup[val][3]>>16)& 0xFF);
                                        a3=  ((lookup[val][3]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;
                                        
                                        a0= ((lookup[val][4])& 0xFF);
                                        a1= ((lookup[val][4]>>8)& 0xFF);
                                        a2= ((lookup[val][4]>>16)& 0xFF);
                                        a3= ((lookup[val][4]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        
                                        gb_buffer_vga32[line][offsX++]= a32;

                                }
                          }
                         break;
                        case 6:
                        //Modo de video BASIC
                         #ifdef use_lib_320x200
                          if (sc&8 || line>194)
                         #else
                          if (sc&8 || line>249)
                         #endif 
                          {
//JJ                           hline(buffer,0,line,320,0);
                           //hline(0,line,320,0);
                           fast_hline(line);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<80;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x2000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line])[x]=lookup[val][0];
//JJ                                        ((unsigned long *)buffer->line[line])[x+1]=lookup[val][1];
                                        //gb_buffer_vga[line][x]=lookup[val][0];
                                        //gb_buffer_vga[line][x+1]=lookup[val][1];
                                        
                                        //portESP32 gb_buffer_vga[line][(x*4)]=(lookup[val][0])& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+1]=(lookup[val][0]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+2]=(lookup[val][0]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+3]=(lookup[val][0]>>24)& 0xFF;

                                        //portESP32 gb_buffer_vga[line][(x*4)+4]=(lookup[val][1])& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+5]=(lookup[val][1]>>8)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+6]=(lookup[val][1]>>16)& 0xFF;
                                        //portESP32 gb_buffer_vga[line][(x*4)+7]=(lookup[val][1]>>24)& 0xFF;


                                        /*
                                        put_pixel((x*4),line,(lookup[val][0])& 0xFF);
                                        put_pixel((x*4)+1,line,(lookup[val][0]>>8)& 0xFF);
                                        put_pixel((x*4)+2,line,(lookup[val][0]>>16)& 0xFF);
                                        put_pixel((x*4)+3,line,(lookup[val][0]>>24)& 0xFF);

                                        put_pixel((x*4)+4,line,(lookup[val][1])& 0xFF);
                                        put_pixel((x*4)+5,line,(lookup[val][1]>>8)& 0xFF);
                                        put_pixel((x*4)+6,line,(lookup[val][1]>>16)& 0xFF);
                                        put_pixel((x*4)+7,line,(lookup[val][1]>>24)& 0xFF);
                                        */

                                        a0= ((lookup[val][0])& 0xFF);
                                        a1= ((lookup[val][0]>>8)& 0xFF);
                                        a2= ((lookup[val][0]>>16)& 0xFF);
                                        a3= ((lookup[val][0]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;

                                        a0= ((lookup[val][1])& 0xFF);
                                        a1= ((lookup[val][1]>>8)& 0xFF);
                                        a2= ((lookup[val][1]>>16)& 0xFF);
                                        a3= ((lookup[val][1]>>24)& 0xFF);
                                        a32= (gb_color_vga[a2]) | (gb_color_vga[a3]<<8) | (gb_color_vga[a0]<<16) | (gb_color_vga[a1]<<24);
                                        gb_buffer_vga32[line][offsX++]= a32;

                                }
                          }
                         break;
                        case 7:
//JJ                        hline(buffer,0,line,320,0);
                          //hline(0,line,320,0);
                          fast_hline(line);
                         break;
                        case 8:         
                          if (sc&8)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                                hline(0,line<<1,640,0);
                                //Pendiente if (sdouble) hline(0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x5000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup[val][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup[val][1];
                                        //portESP32 gb_buffer_vga[line][x]=lookup[val][0];
                                        //portESP32 gb_buffer_vga[line][x+1]=lookup[val][1];

                                        put_pixel(x,line,lookup[val][0]);
                                        put_pixel(x+1,line,lookup[val][1]);
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x]=lookup[val][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+1]=lookup[val][1];
                                        }
                                }
                          }
                         break;
                        case 9:         
                          if (sc&8)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
                           hline(0,line<<1,640,0);                           
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x5000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup2[val][5][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup2[val][5][1];
                                        //portESP32 gb_buffer_vga[line<<1][x]=lookup2[val][5][0];
                                        //portESP32 gb_buffer_vga[line<<1][x+1]=lookup2[val][5][1];

                                        put_pixel(x,line,lookup2[val][5][0]);
                                        put_pixel(x+1,(line<<1),lookup2[val][5][1]);
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x]=lookup2[val][5][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+1]=lookup2[val][5][1];
                                        }
                                }
                          }
                         break;
                        case 10:         
                          if (sc&8)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
                           hline(0,line<<1,640,0);
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x5000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup2[val][6][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup2[val][6][1];
                                        //portESP32 gb_buffer_vga[line<<1][x]=lookup2[val][6][0];
                                        //portESP32 gb_buffer_vga[line<<1][x+1]=lookup2[val][6][1];

                                        put_pixel(x,(line<<1),lookup2[val][6][0]);
                                        put_pixel(x+1,(line<<1),lookup2[val][6][1]);
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+1])[x]=lookup2[val][6][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+1])[x+1]=lookup2[val][6][1];
                                        }
                                }
                          }
                         break;
                        case 11:
                          if (sc&8  || line>249)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
                           hline(0,line<<1,640,0);
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=2)
                                {
                                        if (addr&0x8000) addr-=0x4000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup[val][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup[val][1];
                                        //portESP32 gb_buffer_vga[line<<1][x]=lookup[val][0];
                                        //portESP32 gb_buffer_vga[line<<1][x+1]=lookup[val][1];

                                        put_pixel(x,(line<<1),lookup[val][0]);
                                        put_pixel(x+1,line<<1,lookup[val][1]);
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+1])[x]=lookup[val][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+1])[x+1]=lookup[val][1];
                                        }
                                }
                          }
                         break;
                        case 12:
                          if (sc&8)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
                           hline(0,line<<1,640,0);
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=4)
                                {
                                        if (addr&0x8000) addr-=0x2800;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup2[val][0][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup2[val][0][1];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+2]=lookup2[val][1][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+3]=lookup2[val][1][1];
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x]=lookup2[val][0][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+1]=lookup2[val][0][1];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+2]=lookup2[val][1][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+3]=lookup2[val][1][1];
                                        }
                                }
                          }
                         break;
                        case 13:
                          if (sc&8)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
                           hline(0,line<<1,640,0);
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=4)
                                {
                                        if (addr&0x8000) addr-=0x2800;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup2[val][3][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup2[val][3][1];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+2]=lookup2[val][4][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+3]=lookup2[val][4][1];
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x]=lookup2[val][3][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+1]=lookup2[val][3][1];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+2]=lookup2[val][4][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+3]=lookup2[val][4][1];
                                        }
                                }
                          }
                         break;
                        case 14:         
                          if (sc&8 || line>249)
                          {
//JJ                                hline(buffer,0,line<<1,640,0);
                            hline(0,line<<1,640,0);
//JJ                                if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                          }
                          else
                          {
                                addr=ula.addr+sc;
                                for (x=0;x<160;x+=4)
                                {
                                        if (addr&0x8000) addr-=0x2000;
                                        val=ram[addr];
                                        addr+=8;
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x]=lookup2[val][0][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+1]=lookup2[val][0][1];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+2]=lookup2[val][1][0];
//JJ                                        ((unsigned long *)buffer->line[line<<1])[x+3]=lookup2[val][1][1];
                                        if (sdouble)
                                        {
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x]=lookup2[val][0][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+1]=lookup2[val][0][1];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+2]=lookup2[val][1][0];
//JJ                                                ((unsigned long *)buffer->line[(line<<1)+sdouble])[x+3]=lookup2[val][1][1];
                                        }
                                }
                          }
                         break;
                        case 15:
//JJ                        hline(buffer,0,line<<1,640,0);
                           hline(0,line<<1,640,0);
    //JJ                    if (sdouble) hline(buffer,0,(line<<1)+1,640,0);
                         break;
                }//fin del switch
                
         }//fin del if drawBitluni
    
                sc++;
                sc&=15;
                if (ula.mode==3 || ula.mode==6)
                {
                        if (sc>=10)
                        {
                                sc=0;
                                if (ula.mode&4) ula.addr+=320;
                                else            ula.addr+=640;
                        }
                }
                else
                {                        
                        if (sc>=8)
                        {
                                sc=0;
                                if (ula.mode&4) ula.addr+=320;
                                else            ula.addr+=640;
                        }
                }                        
        }
        }
        
        
        #ifdef use_lib_320x200
         line= paramLine; //Recupero valor linea
        #endif
                   
        
        if (line==99)
           intula(8);
        if (line==256)
        {
                intula(4);
//                textprintf(buffer,font,0,0,7,"%i",fps);
                if (drawfirst)//!tapeon || !tapeframe)
                {
                        if (hires==2)
                        {
//JJ                                blit(buffer,buf16,0,0,0,0,320,256);
                                //blit(buf16,screen,0,0,80,44,640,512); //Directo sin zoom
//JJ no la necesito             Super2xSaI(buf16,buf162,0,0,0,0,320,256);
//JJ                                blit(buf162,screen,0,0,80,44,640,512);
                        }
                        else
                        {
                        if (blur)
                        {
                                if (hires)
                                {
                                        for (y=0;y<512;y+=2)
                                        {
//JJ                                                buff2->line[y][0]=buff2->line[y][1]=buffer->line[y][0];
                                                for (x=2;x<640;x+=2)
                                                {
//JJ                                                        buff2->line[y][x]=buff2->line[y][x+1]=128+buffer->line[y][x]+(buffer->line[y][x-2]<<3);
                                                }
                                        }
//JJ                                        blit(buff2,screen,0,0,80,44,640,512);
                                }
                                else
                                {
                                        for (y=0;y<256;y++)
                                        {
//JJ                                                buff2->line[y][0]=buffer->line[y][0];
                                                for (x=1;x<320;x++)
                                                {
//JJ                                                        buff2->line[y][x]=128+buffer->line[y][x]+(buffer->line[y][x-1]<<3);
                                                }
                                        }
//JJ                                        blit(buff2,screen,0,0,40,22,320,256);
                                }
                        }
                        else
                        {
//JJ                                if (hires) blit(buffer,screen,0,0,80,44,640,512);
//JJ                                else       blit(buffer,screen,0,0,40,22,320,256);
                        }
                        }
                }
                drawfirst=0;
                frames++;
        }
        
        
           #ifdef use_lib_320x200                     
            //printf("%d %d\n",line,(line % 8));
            /*
            if ((line>=0) && (line<256))
            {
             if ((line % 10 == 8)||(line % 10 == 9))
             {
              //float auxLine= (float)line / (float)1.28f; //256 DIV 200 = 1,28 
              ////float auxLine= (float)line / (float)1.06f; //256 DIV 200 = 1,28             
              //line= (int)auxLine;              
              hline(0,line,320,0);
             }
            }
            */
           #endif
                   
}

void loadulastate(FILE *f)
{
        int c;
        fread(pal,16,1,f);
        ula.ints=getc(f);
        ula.intc=getc(f);
        ula.addrlo=getc(f);
        ula.addrhi=getc(f);
        ula.mode=getc(f);
        ula.receivebyte=getc(f);
        tapeon=getc(f);
        soundena=getc(f);
//JJ        soundlimit=getc(f);
//JJ        soundlimit|=getc(f)<<8;
//JJ        soundlimit|=getc(f)<<16;
//JJ        soundlimit|=getc(f)<<24;
        intula(0);
        for (c=0x80;c<0x0C0;c++)
        {
         #ifdef use_lib_rom_flash
          CositasROM(currom,c);
         #else 
          mem[c]=(unsigned char *)(rom+(currom<<14)+((c&0x3F)<<8));         
         #endif 
        }
        //CositasROM(currom,c);
        
        if (currom==8 || currom==9) for (c=0x80;c<0xC0;c++) memstat[c]=2;
        else                        for (c=0x80;c<0xC0;c++) memstat[c]=1;
        makelookup4();
}

void loadulastateFlash(unsigned char id)
{
 unsigned int cont=4+7+32768;

 #ifdef use_lib_log_serial
  printf("loadulastateFlash %d\n",id);
 #endif 
        int c;
        //fread(pal,16,1,f);
        //memcpy(pal,&gb_list_sna_data[id][cont],16);
        memcpy(pal,&gb_ptr_sna_data[id][cont],16);
        cont+=16;
        //ula.ints=gb_list_sna_data[id][cont++];
        //ula.intc=gb_list_sna_data[id][cont++];
        //ula.addrlo=gb_list_sna_data[id][cont++];
        //ula.addrhi=gb_list_sna_data[id][cont++];
        //ula.mode=gb_list_sna_data[id][cont++];
        //ula.receivebyte=gb_list_sna_data[id][cont++];
        //tapeon=gb_list_sna_data[id][cont++];
        //soundena=gb_list_sna_data[id][cont++];

        ula.ints=gb_ptr_sna_data[id][cont++];
        ula.intc=gb_ptr_sna_data[id][cont++];
        ula.addrlo=gb_ptr_sna_data[id][cont++];
        ula.addrhi=gb_ptr_sna_data[id][cont++];
        ula.mode=gb_ptr_sna_data[id][cont++];
        ula.receivebyte=gb_ptr_sna_data[id][cont++];
        tapeon=gb_ptr_sna_data[id][cont++];
        soundena=gb_ptr_sna_data[id][cont++];

//JJ        soundlimit=getc(f);
//JJ        soundlimit|=getc(f)<<8;
//JJ        soundlimit|=getc(f)<<16;
//JJ        soundlimit|=getc(f)<<24;
        intula(0);
        for (c=0x80;c<0x0C0;c++)
        {
         #ifdef use_lib_rom_flash
          CositasROM(currom,c);
         #else 
          mem[c]=(unsigned char *)(rom+(currom<<14)+((c&0x3F)<<8));         
         #endif 
        }
        //CositasROM(currom,c);
        
        if (currom==8 || currom==9) for (c=0x80;c<0xC0;c++) memstat[c]=2;
        else                        for (c=0x80;c<0xC0;c++) memstat[c]=1;
        makelookup4();
        
 //fflush(stdout);        
}


void saveulastate(FILE *f)
{
        fwrite(pal,16,1,f);
        putc(ula.ints,f);
        putc(ula.intc,f);
        putc(ula.addrlo,f);
        putc(ula.addrhi,f);
        putc(ula.mode,f);
        putc(ula.receivebyte,f);
        putc(tapeon,f);
        putc(soundena,f);
//JJ        putc(soundlimit&0xFF,f);
//JJ        putc((soundlimit>>8)&0xFF,f);
//JJ        putc((soundlimit>>16)&0xFF,f);
//JJ        putc((soundlimit>>24)&0xFF,f);
}

