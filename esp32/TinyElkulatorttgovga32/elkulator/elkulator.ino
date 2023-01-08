//Port Elkulator x86 Acorn Electron version 0.5 (author Tom Walker) to TTGO VGA32 by ackerman
//Author: Ackerman
//License: WTFPL
// Load cartdridges 16KB and 32 KB
// Load SNA
// Load DSK
// No PSRAM, only SRAM
// VGA MODE320x200
// Mode 64 colors RGB 6 bits and 3 bits RGB
// VGA Tiny library bitluni 0.3.3 mod convert C
//Visual Studio 1.66.1 PLATFORMIO 2.5.0 Espressif32 v3.5.0
//Arduino IDE 1.8.11 Espressif System 1.0.6
// gbConfig options configuration compile

#include "gbConfig.h"
#include "gbGlobals.h"
#include "PS2Kbd.h"
#include <Arduino.h>

#include "elkulator.h"
#ifdef use_lib_tinybitluni_fast
#else
 //#include "MartianVGA.h"
 //#include "def/Font.h"
#endif 
#include "hardware.h"
#include "PS2KeyCode.h"

#include "elk.h"
#include "6502.h"
#include "ula.h"
#include "uef.h"
#include "1770.h"
#include "osd.h"
#ifdef use_lib_os_flash
 #include "dataFlash/gbos.h"
 #include "dataFlash/gbrom.h"
#endif 

#include "dataFlash/gbsna.h"
//#include "dataFlash/gbcart.h"
#ifdef use_lib_disk_flash_test
 #include "dataFlash/gbdsk.h"
#endif

//#include "dataFlash/gbrom.h"
//#include "PS2Boot/PS2KeyCode.h"
#ifdef use_lib_tinybitluni_fast
 #include "vga_6bit.h"
 // VGA output pins
 //#define PIN_RED_LOW    21
 //#define PIN_RED_HIGH   22
 //#define PIN_GREEN_LOW  18
 //#define PIN_GREEN_HIGH 19
 //#define PIN_BLUE_LOW   4
 //#define PIN_BLUE_HIGH  5
 //#define PIN_HSYNC      23
 //#define PIN_VSYNC      15

 #ifdef use_lib_vga8colors
  //DAC 3 bits 8 colores
  // 3 bit pins  
  #ifdef use_lib_board_wemos  
   //Placa WEMOS D1 R32
   static const unsigned char pin_config[] = {
    PIN_RED_HIGH,   //255
    PIN_GREEN_HIGH, //255
    PIN_BLUE_LOW,  //255
    255,
    255,
    255,
    PIN_HSYNC,
    PIN_VSYNC     
   };
  #else
   //TTGO VGA32 8 colores  
   static const unsigned char pin_config[] = {
    PIN_RED_HIGH,
    PIN_GREEN_HIGH,  
    PIN_BLUE_HIGH,
    255,
    255,
    255,
    PIN_HSYNC,
    PIN_VSYNC   
   };
  #endif
// static const unsigned char pin_config[] = {  
//  /*
//  //TTGO VGA32 8 colores
//
//  PIN_RED_HIGH,
//  PIN_GREEN_HIGH,  
//  PIN_BLUE_HIGH,
//  255,
//  255,
//  255,
//  PIN_HSYNC,
//  PIN_VSYNC
//  */
//
//  //WEMOS
//  PIN_RED_HIGH,   //255
//  PIN_GREEN_HIGH, //255
//  PIN_BLUE_LOW,  //255
//  255,
//  255,
//  255,
//  PIN_HSYNC,
//  PIN_VSYNC
//
// };
 #else
  //DAC 6 bits 64 colores TTGO VGA32
  static const unsigned char pin_config[] = {
   PIN_RED_LOW,
   PIN_RED_HIGH,
   PIN_GREEN_LOW,
   PIN_GREEN_HIGH,
   PIN_BLUE_LOW,
   PIN_BLUE_HIGH,
   PIN_HSYNC,
   PIN_VSYNC
  };
 #endif 

#endif

#ifdef use_lib_tinybitluni_fast 
 //unsigned char **framebuffer = vga_get_framebuffer();
 //unsigned char **gb_buffer_vga= vga_get_framebuffer();
 unsigned char gb_sync_bits;
#endif 


unsigned char **gb_buffer_vga;
unsigned int **gb_buffer_vga32;

//Global apuntando a Flash SNA
const char ** gb_ptr_sna_title= gb_list_sna_title; //Puntero a la lista global
const unsigned char **gb_ptr_sna_data= gb_list_sna_data; //Puntero a la lista global
unsigned short int gb_max_sna = max_list_sna;
//Global apuntando a Flash DSK
const char ** gb_ptr_dsk_title= gb_list_dsk_title;
const unsigned int *gb_ptr_dsk_end= gb_list_dsk_end;
const unsigned char ** gb_ptr_dsk_data= gb_list_dsk_data;
unsigned short int gb_max_dsk= max_list_dsk;
unsigned char gb_dsk_id_cur= 0; //Disco actual, el 0


unsigned char gb_color_vga[256];

unsigned int gb_cpu_timer_before,gb_cpu_timer_cur;
unsigned int gb_max_cpu_ticks=0,gb_min_cpu_ticks=1000000,gb_cur_cpu_ticks;
unsigned int jj_ini_cpu,jj_end_cpu;
unsigned int jj_ms_ini_cpu=0;
unsigned int jj_ms_cur_cpu=0;
unsigned int gb_ms_cur_keyboard=0;
unsigned int gb_ms_ini_keyboard=0;

unsigned char gb_cpu_auto=1;
unsigned char gb_cur_fps=0;
unsigned char gb_use_video_scale=0; //usar escalado


int gb_volumen01;
int gb_frecuencia01;

unsigned int gb_cont_loop=0;


unsigned char gb_ula_mode_cur = 255;
unsigned char gb_ula_mode_last = 255;
 
//portESP32 unsigned char elkpal[256];
//portESP32 unsigned char monopal[256];
unsigned char gb_colors[256];


int done=0;
unsigned char gb_make_sna=0;


int fasttape;

TULA ula;
struct TP p;

int resetspeed;
int quit;
unsigned char a,x,y,s;
int interrupt;
unsigned char *mem[0x100];
int memstat[0x100];
unsigned char *ram;
#ifdef use_lib_rom_flash
#else
 unsigned char *rom;
#endif 

unsigned char *gb_array_rom[16]; //16 roms de 16384 bytes

#ifdef use_lib_os_flash
 //unsigned char * os;
 //const unsigned char * gb_rom_os;
#else 
 unsigned char os[0x4000];
#endif 
int writeablerom[16];
unsigned char currom;
int tapeon;
int soundenable;
int soundena;
int pcsound,pcsoundl;
int mono;
int blur;
int plus3ena=1; //activo el disco PLUS3
int adfsena= 1;
int dfsena= 1;

char discname[260],discname2[260];
char tapename[260];
int hires,fullscreen;


//Funciones
void Setup(void);
void Poll_Keyboard(void);
void ResetKeyboardAcorn(void);
void PreparaColorVGA(void);


//**************************************************
void PreparaColorVGA()
{
 #ifdef use_lib_tinybitluni_fast
  #ifdef use_lib_vga8colors
   for (unsigned int i=0;i<256;i++)
   {  
    //gb_color_vga[i] = (gb_color_vga[i] & 0x07) | gb_sync_bits;
    gb_color_vga[i] = ((i*4) & 0x07) | gb_sync_bits;  
   }

   gb_color_vga[0]= (0x00 & 0x07)| gb_sync_bits;  
   gb_color_vga[1]= (0x01 & 0x07)| gb_sync_bits;  //000011
   gb_color_vga[2]= (0x02 & 0x07)| gb_sync_bits;  //001100
   gb_color_vga[3]= (0x03 & 0x07)| gb_sync_bits;  //001111
   gb_color_vga[4]= (0x04 & 0x07)| gb_sync_bits;  //110000
   gb_color_vga[5]= (0x05 & 0x07)| gb_sync_bits;  //110011
   gb_color_vga[6]= (0x06 & 0x07)| gb_sync_bits;  //111100
   gb_color_vga[7]= (0x07 & 0x07)| gb_sync_bits;  //111111      
  #else
   //for (unsigned char i=0;i<8;i++)
   for (unsigned int i=0;i<256;i++)
   {  
    //gb_color_vga[i] = (gb_color_vga[i] & 0x3F) | gb_sync_bits;  
    gb_color_vga[i] = ((i*4) & 0x3F) | gb_sync_bits;  
   }
  
   gb_color_vga[0]= (0x00 & 0x3F)| gb_sync_bits;  
   gb_color_vga[1]= (0x03 & 0x3F)| gb_sync_bits;   //000011
   gb_color_vga[2]= (0x0C & 0x3F)| gb_sync_bits;  //001100
   gb_color_vga[3]= (0x0F & 0x3F)| gb_sync_bits;  //001111
   gb_color_vga[4]= (0x30 & 0x3F)| gb_sync_bits;   //110000
   gb_color_vga[5]= (0x33 & 0x3F)| gb_sync_bits;  //110011
   gb_color_vga[6]= (0x3C & 0x3F)| gb_sync_bits;  //111100
   gb_color_vga[7]= (0x3F & 0x3F)| gb_sync_bits;  //111111   

 //elkpal[0].r=  0; elkpal[0].g=  0; elkpal[0].b=  0;
 //elkpal[1].r= 63; elkpal[1].g=  0; elkpal[1].b=  0;
 //elkpal[2].r=  0; elkpal[2].g= 63; elkpal[2].b=  0;
 //elkpal[3].r= 63; elkpal[3].g= 63; elkpal[3].b=  0;
 //elkpal[4].r=  0; elkpal[4].g=  0; elkpal[4].b= 63;
 //elkpal[5].r= 63; elkpal[5].g=  0; elkpal[5].b= 63;
 //elkpal[6].r=  0; elkpal[6].g= 63; elkpal[6].b= 63;
 //elkpal[7].r= 63; elkpal[7].g= 63; elkpal[7].b= 63;   
  #endif 
 #else	
  for (unsigned int i=0;i<8;i++)
  {  
   gb_color_vga[i] = (gb_color_vga[i] & vga.RGBAXMask) | vga.SBits;  
  }
 #endif 
}

//**********************
void ResetKeyboardAcorn()
{
  for (int i=0;i<2;i++)
  {
   for (int j=0;j<14;j++)
   {
    for (int k=0;k<4;k++)
    {
     gb_keys[i][j][k]=0;
    }
   }  
  }     
}



//Setup principal
void setup()
{
 //Sonido Setup
 //pinMode(SPEAKER_PIN, OUTPUT);
 ////REG_WRITE(GPIO_OUT_W1TC_REG , BIT25); //LOW clear
 //digitalWrite(SPEAKER_PIN, LOW);
 
 #ifdef use_lib_log_serial
  Serial.begin(115200);         
  Serial.printf("HEAP BEGIN %d\n", ESP.getFreeHeap()); 
 #endif
 
 ram=(unsigned char *)malloc(0x10000);
 #ifdef use_lib_log_serial
  Serial.printf("HEAP RAM malloc %d\n", ESP.getFreeHeap()); 
 #endif 

 #ifdef use_lib_tinybitluni_fast  
  //void vga_init(const int *pin_map, const VgaMode &mode, bool double_buffered)
  //vga_init(pin_config,vga_mode_320x240,false);
  //vga_init(pin_config,vga_mode_320x200,false); //funciona
  //vga_init(pin_config,vga_mode_240x240,false); //funciona
  
  //vga_init(pin_config,vga_mode_200x150,false);  
  #ifdef use_lib_log_serial
   Serial.printf("Antes vga\n");
  #endif
  
  #ifdef use_lib_vga320x200
   vga_init(pin_config,VgaMode_vga_mode_320x200,false); //Llamada en C   
  #endif
   
  #ifdef use_lib_log_serial
   Serial.printf("Despues vga\n"); 
  #endif 
  

  //vga_init(pin_config,VgaMode_vga_mode_320x200,false); //Llamada en C

  //Serial.printf("pin_config size:%d\n",sizeof(pin_config));
  gb_sync_bits= vga_get_sync_bits();
  gb_buffer_vga = vga_get_framebuffer();
  gb_buffer_vga32=(unsigned int **)gb_buffer_vga;
  #ifdef use_lib_log_serial
   Serial.printf("Antes prepara\n"); 
  #endif 
  PreparaColorVGA();
  #ifdef use_lib_log_serial
   Serial.printf("Despues prepara\n"); 
  #endif 
  SDLClearULA();
  #ifdef use_lib_log_serial
   Serial.printf("Despues Clear\n"); 
  #endif 

  #ifdef use_lib_log_serial
   Serial.printf("Fast Tiny Bitluni\n");
   Serial.printf("vsync_inv_bit:0x%02X hsync_inv_bit:0x%02X\n",vga_get_vsync_inv_bit(),vga_get_hsync_inv_bit());
   Serial.printf("Sync bits:0x%02X\n",gb_sync_bits);
  #endif 
//  PreparaColorVGA();
//  SDLClear();
  //En 320x200 sale bits:0x80 y deberia ser 0x40
  //En 320x240 sale bits:0xC0
 #endif 
 
 
 #ifdef use_lib_log_serial
  Serial.printf("VGA %d\n", ESP.getFreeHeap()); 
 #endif

 kb_begin();

 ResetKeyboardAcorn();
 initmem();
 #ifdef use_lib_rom_flash
 #else
  loadroms();
 #endif
 reset6502();
 hires=0;
 initula();
 reset1770();
 updateadfs();
 initsound();
 updateres(); 

 gb_cpu_timer_cur= millis();
 gb_ms_cur_keyboard= gb_ms_ini_keyboard= jj_ms_ini_cpu= jj_ms_cur_cpu= gb_cpu_timer_cur;
 

 #ifdef use_lib_log_serial  
  Serial.printf("END SETUP %d\n", ESP.getFreeHeap()); 
 #endif 
}

void Poll_Keyboard()
{
 gb_show_osd_main_menu = (!keymap[PS2_KC_F1])&0x01;

 gb_keys[0][0][0] = gb_keys[1][0][0] = !(keymap[KEY_CURSOR_RIGHT]);
 //gb_keys[0][1][1] = gb_keys[0][1][1] = !(keymap[KEY_CURSOR_DOWN]);
 //gb_keys[0][1][2] = gb_keys[0][1][2] = !(keymap[PS2_KC_ENTER]);
 gb_keys[0][0][3] = gb_keys[1][0][3] = !(keymap[PS2_KC_SPACE]);

 gb_keys[0][1][0] = gb_keys[1][1][0] = !(keymap[KEY_CURSOR_LEFT]);
 gb_keys[0][1][1] = gb_keys[1][1][1] = !(keymap[KEY_CURSOR_DOWN]);
 gb_keys[0][1][2] = gb_keys[1][1][2] = !(keymap[PS2_KC_ENTER]);
 gb_keys[0][1][3] = gb_keys[1][1][3] = !(keymap[KEY_BACKSPACE]);

 gb_keys[0][2][0] = gb_keys[1][2][0] = !(keymap[PS2_KC_MINUS]);
 gb_keys[0][2][1] = gb_keys[1][2][1] = !(keymap[KEY_CURSOR_UP]);
 gb_keys[0][2][2] = gb_keys[1][2][2] = !(keymap[PS2_KC_APOS]);

 gb_keys[0][3][0] = gb_keys[1][3][0] = !(keymap[PS2_KC_0]);
 gb_keys[0][3][1] = gb_keys[1][3][1] = !(keymap[PS2_KC_P]);
 //gb_keys[0][3][2] = gb_keys[1][3][2] = !(keymap[PS2_KC_DOT]); //colon :
 gb_keys[0][3][3]= gb_keys[1][3][3] = !(keymap[PS2_KC_DIV]); //slash /

 gb_keys[0][4][0]= gb_keys[1][4][0] = !(keymap[PS2_KC_9]);
 gb_keys[0][4][1]= gb_keys[1][4][1] = !(keymap[PS2_KC_O]);
 gb_keys[0][4][2]= gb_keys[1][4][2] = !(keymap[PS2_KC_L]);
 gb_keys[0][4][3]= gb_keys[1][4][3] = !(keymap[PS2_KC_DOT]); //STOP punto

 gb_keys[0][5][0]= gb_keys[1][5][0] = !(keymap[PS2_KC_8]);
 gb_keys[0][5][1]= gb_keys[1][5][1] = !(keymap[PS2_KC_I]);
 gb_keys[0][5][2]= gb_keys[1][5][2] = !(keymap[PS2_KC_K]);
 gb_keys[0][7][3]= gb_keys[1][7][3] = !(keymap[PS2_KC_COMMA]); //comma

 gb_keys[0][6][0]= gb_keys[1][6][0] = !(keymap[PS2_KC_7]);
 gb_keys[0][6][1]= gb_keys[1][6][1] = !(keymap[PS2_KC_U]);
 gb_keys[0][6][2]= gb_keys[1][6][2] = !(keymap[PS2_KC_J]);
 gb_keys[0][6][3]= gb_keys[1][6][3] = !(keymap[PS2_KC_M]);

 gb_keys[0][7][0]= gb_keys[1][7][0] = !(keymap[PS2_KC_6]);
 gb_keys[0][7][1]= gb_keys[1][7][1] = !(keymap[PS2_KC_Y]);
 gb_keys[0][7][2]= gb_keys[1][7][2] = !(keymap[PS2_KC_H]);
 gb_keys[0][7][3]= gb_keys[1][7][3] = !(keymap[PS2_KC_N]);
                                 
 gb_keys[0][8][0]= gb_keys[1][8][0] = !(keymap[PS2_KC_5]);
 gb_keys[0][8][1]= gb_keys[1][8][1] = !(keymap[PS2_KC_T]);
 gb_keys[0][8][2]= gb_keys[1][8][2] = !(keymap[PS2_KC_G]);
 gb_keys[0][8][3]= gb_keys[1][8][3] = !(keymap[PS2_KC_B]);
                                    
 gb_keys[0][9][0]= gb_keys[1][9][0] = !(keymap[PS2_KC_4]);
 gb_keys[0][9][1]= gb_keys[1][9][1] = !(keymap[PS2_KC_R]);
 gb_keys[0][9][2]= gb_keys[1][9][2] = !(keymap[PS2_KC_F]);
 gb_keys[0][9][3]= gb_keys[1][9][3] = !(keymap[PS2_KC_V]);
                                 
 gb_keys[0][10][0]= gb_keys[1][10][0]= !(keymap[PS2_KC_3]);
 gb_keys[0][10][1]= gb_keys[1][10][1]= !(keymap[PS2_KC_E]);
 gb_keys[0][10][2]= gb_keys[1][10][2]= !(keymap[PS2_KC_D]);
 gb_keys[0][10][3]= gb_keys[1][10][3]= !(keymap[PS2_KC_C]);
                                 
 gb_keys[0][11][0]= gb_keys[1][11][0]= !(keymap[PS2_KC_2]);
 gb_keys[0][11][1]= gb_keys[1][11][1]= !(keymap[PS2_KC_W]);
 gb_keys[0][11][2]= gb_keys[1][11][2]= !(keymap[PS2_KC_S]);
 gb_keys[0][11][3]= gb_keys[1][11][3]= !(keymap[PS2_KC_X]);
                                 
 gb_keys[0][12][0]= gb_keys[1][12][0]= !(keymap[PS2_KC_1]);
 gb_keys[0][12][1]= gb_keys[1][12][1]= !(keymap[PS2_KC_Q]);
 gb_keys[0][12][2]= gb_keys[1][12][2]= !(keymap[PS2_KC_A]);
 gb_keys[0][12][3]= gb_keys[1][12][3]= !(keymap[PS2_KC_Z]);

 gb_keys[0][13][0]= gb_keys[1][13][0]= !(keymap[PS2_KC_ESC]);
 gb_keys[0][13][1]= gb_keys[1][13][1]= !(keymap[PS2_KC_ALT]);
 gb_keys[0][13][2]= gb_keys[1][13][2]= !(keymap[PS2_KC_CTRL]);
 gb_keys[0][13][3]= gb_keys[1][13][3]= !(keymap[PS2_KC_L_SHIFT]);
}


//Loop main
void loop() 
{
 gb_ms_cur_keyboard = millis();
 if ((gb_ms_cur_keyboard - gb_ms_ini_keyboard) >= gb_ms_keyboard)
 {
  gb_ms_ini_keyboard= gb_ms_cur_keyboard;
  Poll_Keyboard();
 
  if (gb_show_osd_main_menu == 1)
  {
   //#ifdef use_lib_log_serial
   // Serial.printf("F1\n");
   //#endif 
   do_tinyOSD();  
  }
 }

 jj_ms_cur_cpu = millis();
 if (((jj_ms_cur_cpu - jj_ms_ini_cpu) >= 20) || (gb_cpu_auto==0))
 {
  jj_ms_ini_cpu= jj_ms_cur_cpu;

  jj_ini_cpu = micros(); 
  exec6502(312,128);  
  jj_end_cpu = micros();
  gb_cur_fps++;

  gb_cur_cpu_ticks= (jj_end_cpu-jj_ini_cpu);
  if (gb_cur_cpu_ticks>gb_max_cpu_ticks)
   gb_max_cpu_ticks= gb_cur_cpu_ticks;
  if (gb_cur_cpu_ticks<gb_min_cpu_ticks)   
   gb_min_cpu_ticks= gb_cur_cpu_ticks; 
 }


 gb_cpu_timer_cur= millis();
 if ((gb_cpu_timer_cur-gb_cpu_timer_before)>= 999)
 {//1 segundo
  gb_cpu_timer_before= gb_cpu_timer_cur;
  //Imprimo CPU
  #ifdef use_lib_log_serial
   Serial.printf("fps:%d c:%u m:%u mx:%u\n",gb_cur_fps,gb_cur_cpu_ticks,gb_min_cpu_ticks,gb_max_cpu_ticks);
  #endif 

  //Reseteo CPU a 1 segundo
  gb_cur_fps= 0;
  gb_min_cpu_ticks= 1000000;
  gb_max_cpu_ticks= 0;
  gb_cur_cpu_ticks= 0;   
 }

}
