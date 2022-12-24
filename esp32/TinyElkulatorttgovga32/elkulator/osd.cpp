#include "gbConfig.h"
#include "gbGlobals.h"
#include "hardware.h"
#include "elkulator.h"
#include "osd.h"
//#include "dataFlash/gbdsk.h"
//#include "dataFlash/gbcom.h"
#include "hardware.h"
//#ifdef use_lib_audio_tone32
// #include "Tone32.h"
//#endif
#include "PS2Kbd.h"
#include "gb_sdl_font8x8.h"


//#include "dataFlash/gbsna.h"
#include "dataFlash/gbcart.h"
#include "snapshot.h"
#include "6502.h"

#include "ula.h" //No la necesitamos solo es para el put_pixel

//#include "render.h"
//#include "dataFlash/gbsnarare.h"
//#include "cpu.h"
//#include "ports.h"


//#define BLACK   0
//#define BLUE    4
//#define RED     1
//#define MAGENTA 5
//#define GREEN   2
//#define CYAN    6
//#define YELLOW  3
//#define WHITE   15

#ifdef COLOR_3B           //       BGR 
 #define BLACK   0x08      // 0000 1000
 #define BLUE    0x0C      // 0000 1100
 #define RED     0x09      // 0000 1001
 #define MAGENTA 0x0D      // 0000 1101
 #define GREEN   0x0A      // 0000 1010
 #define CYAN    0x0E      // 0000 1110
 #define YELLOW  0x0B      // 0000 1011
 #define WHITE   0x0F      // 0000 1111
#endif


//extern int gb_screen_xIni;
//extern int gb_screen_yIni;
//extern unsigned char gb_cache_zxcolor[8];


unsigned char gb_show_osd_main_menu=0;

//extern SDL_Surface * gb_screen;
//extern SDL_Event gb_event;





//#define max_gb_osd_screen 1
//const char * gb_osd_screen[max_gb_osd_screen]={
// "Pixels Left"//,
// //"Pixels Top",
// //"Color 8",
// //"Mono Blue 8",
// //"Mono Green 8",
// //"Mono Red 8",
// //"Mono Grey 8"
//};

//#define max_gb_osd_screen_values 5
//const char * gb_osd_screen_values[max_gb_osd_screen_values]={
// "0",
// "2",
// "4", 
// "8", 
// "16"
//};



#define max_gb_main_menu 8
const char * gb_main_menu[max_gb_main_menu]={
 "Load SNA",
 "Load CART",
 "Load DISC",
 "Reset",
 "Delay CPU",
 "Video Scale",
 "Exit",
 "Return"
};

//#define max_gb_machine_menu 3
//const char * gb_machine_menu[max_gb_machine_menu]={
// "464",
// "664",
// "6128"
//};

#define max_gb_value_size_cart_menu 2
const char * gb_value_size_cart_menu[max_gb_value_size_cart_menu]={
 "16",
 "32"
};

#define max_gb_delay_cpu_menu 2
const char * gb_delay_cpu_menu[max_gb_delay_cpu_menu]={
 "Auto",
 "0"
 //,
 //"1",
 //"2",
 //"4",
 //"8",
 //"16",
 //"32",
 //"64",
 //"128"
};


//#define max_gb_speed_sound_menu 7
//const char * gb_speed_sound_menu[max_gb_speed_sound_menu]={
// "0",
// "1",
// "2",
// "4",
// "8",
// "16",
// "32"
//};

//#define max_gb_value_binary_menu 2
//const char * gb_value_binary_menu[max_gb_value_binary_menu]={
// "0",
// "1"
//};


//#define max_gb_speed_videoaudio_options_menu 2
//const char * gb_speed_videoaudio_options_menu[max_gb_speed_videoaudio_options_menu]={
// "Video poll",
// "Keyboard poll"
//};

#define max_gb_speed_video_poll_menu 4
const char * gb_speed_video_poll_menu[max_gb_speed_video_poll_menu]={
 "20",
 "30",
 "40",
 "50"
};

#define max_gb_speed_keyboard_poll_menu 5
const char * gb_speed_keyboard_poll_menu[max_gb_speed_keyboard_poll_menu]={
 "10",
 "20",
 "30",
 "40",
 "50"
};


//#define max_gb_speed_menu 5
//const char * gb_speed_menu[max_gb_speed_menu]={
// "Normal",
// "2x",
// "4x",
// "8x",
// "16x"
//};


//#define max_gb_osd_mouse_menu 2
//const char * gb_osd_mouse_menu[max_gb_osd_mouse_menu]={
// "right handed",
// "left handed"
//};

#define max_gb_reset_menu 2
const char * gb_reset_menu[max_gb_reset_menu]={
 "Soft",
 "Hard"
};

#define max_gb_bool_menu 2
const char * gb_bool_menu[max_gb_bool_menu]={
 "Yes",
 "No"
};


#define gb_pos_x_menu 40
#define gb_pos_y_menu 20
#define gb_osd_max_rows 10


void jj_fast_putpixel(int x,int y,unsigned char aColor)
{
 gb_buffer_vga[y][x^2]= gb_color_vga[aColor];
}

void SDLClear()
{
 unsigned int a0,a1,a2,a3,a32;
 //for (int y=0; y<(auxSurface->w); y++)
 // for (int x=0; x<(auxSurface->h); x++)
 //  SDLputpixel(auxSurface,x,y,1);
 //for (int y=0; y<200; y++)
 // for (int x=0; x<320; x++)   

// for (int y=0; y<200; y++)
//  for (int x=0; x<320; x++)    
//   jj_fast_putpixel(x,y,0);

   //SDLputpixel(auxSurface,x,y,3); 


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

//*************************************************************************************
void SDLprintCharOSD(char car,int x,int y,unsigned char color,unsigned char backcolor)
{ 
// unsigned char aux = gb_sdl_font_6x8[(car-64)];
 int auxId = car << 3; //*8
 unsigned char aux;
 //unsigned char auxBit,auxColor;
 unsigned char auxColor; //no se usa auxBit
 for (unsigned char j=0;j<8;j++)
 {
  aux = gb_sdl_font_8x8[auxId + j];
  for (int i=0;i<8;i++)
  {
   auxColor= ((aux>>i) & 0x01);
   //SDLputpixel(surface,x+(6-i),y+j,(auxColor==1)?color:backcolor);
   jj_fast_putpixel(x+(6-i),y+j,(auxColor==1)?color:backcolor);
  }
 }
}

void SDLprintText(const char *cad,int x, int y, unsigned char color,unsigned char backcolor)
{
//SDL_Surface *surface,
// gb_sdl_font_6x8
 int auxLen= strlen(cad);
 if (auxLen>50)
  auxLen=50;
 for (int i=0;i<auxLen;i++)
 {
  SDLprintCharOSD(cad[i],x,y,color,backcolor);
  x+=7;
 }
}

void OSDMenuRowsDisplayScroll(const char **ptrValue,unsigned char currentId,unsigned char aMax)
{//Dibuja varias lineas
 for (int i=0;i<gb_osd_max_rows;i++)
  SDLprintText("                    ",gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),0,0);
 
 for (int i=0;i<gb_osd_max_rows;i++)
 {
  if (currentId >= aMax)
   break;
  //SDLprintText(gb_osd_sdl_surface,ptrValue[currentId],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?CYAN:WHITE),((i==0)?BLUE:BLACK),1);
  SDLprintText(ptrValue[currentId],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_VIOLETA:ID_COLOR_BLACK));
  currentId++;
 }     
}


#ifdef use_lib_wifi
 void ShowStatusWIFI(unsigned char aState)
 {
  if (aState == 1)
  {
   SDLprintText("WIFI LOAD",8,8,ID_COLOR_BLACK,ID_COLOR_WHITE);   
  }
  else
  {  
   SDLprintText("         ",8,8,ID_COLOR_BLACK,ID_COLOR_BLACK);
  }
 }


 //**********************************************************************************
 void OSDMenuRowsDisplayScrollFromWIFI(unsigned char *ptrBuffer,unsigned char currentId,unsigned char aMax)
 {//Dibuja varias lineas 
  char cadName8[10];
  cadName8[8]='\0';
  for (int i=0;i<gb_osd_max_rows;i++)
  {
   SDLprintText("                    ",gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),ID_COLOR_BLACK,ID_COLOR_BLACK);
  }
 
  for (int i=0;i<gb_osd_max_rows;i++)
  {
   if (currentId >= aMax)
    break;
   memcpy(cadName8, &ptrBuffer[(currentId*8)], 8);
   cadName8[8]='\0';
//   SDLprintText(ptrValue[currentId],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_MAGENTA:ID_COLOR_BLACK));
   //SDLprintText((const char*)cadName8,gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_MAGENTA:ID_COLOR_BLACK));
   SDLprintText((const char*)cadName8,gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_VIOLETA:ID_COLOR_BLACK));
   currentId++;
  }     
 }

 //**********************************************************************************
 unsigned char ShowTinyMenuFromWIFI(const char *cadTitle,unsigned char *ptrBuffer,unsigned char aMax)
 {
  unsigned char aReturn=0;
  unsigned char salir=0;

  SDLClear();
  SDLprintText("FreeChaF by Ackerman",gb_pos_x_menu-(4<<3),gb_pos_y_menu-16,ID_COLOR_WHITE,ID_COLOR_BLACK);
  for (int i=0;i<14;i++)
  {
   SDLprintCharOSD(' ',gb_pos_x_menu+(i<<3),gb_pos_y_menu,ID_COLOR_BLACK,ID_COLOR_WHITE);   
  }
  SDLprintText(cadTitle,gb_pos_x_menu,gb_pos_y_menu,ID_COLOR_BLACK,ID_COLOR_WHITE);
  OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,0,aMax); 
  while (salir == 0)
  {  
   if (checkAndCleanKey(KEY_CURSOR_LEFT))
   {
    if (aReturn>10) aReturn-=10;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   }
   if (checkAndCleanKey(KEY_CURSOR_RIGHT))
   {
    if (aReturn<(aMax-10)) aReturn+=10;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   }
   if (checkAndCleanKey(KEY_CURSOR_UP))
   {
    if (aReturn>0) aReturn--;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   }
   if (checkAndCleanKey(KEY_CURSOR_DOWN))
   {
    if (aReturn < (aMax-1)) aReturn++;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   } 
   if (checkAndCleanKey(KEY_ENTER))
   {
    salir= 1;
   }
   if (checkAndCleanKey(KEY_ESC))
   {
    salir=1; aReturn= 255;
   }
  }
  gb_show_osd_main_menu= 0;
  return aReturn;
 }
#endif


//Maximo 256 elementos
unsigned char ShowTinyMenu(const char *cadTitle,const char **ptrValue,unsigned char aMax)
{
 unsigned char aReturn=0;
 unsigned char salir=0;
 SDLClear();
 SDLprintText("Elkulator ESP32 mod by Ackerman",gb_pos_x_menu-(4<<3),gb_pos_y_menu-16,ID_COLOR_WHITE,ID_COLOR_BLACK);
 //for (int i=0;i<20;i++) 
 for (int i=0;i<14;i++)
 {
  SDLprintCharOSD(' ',gb_pos_x_menu+(i<<3),gb_pos_y_menu,ID_COLOR_BLACK,ID_COLOR_WHITE);
 }
 SDLprintText(cadTitle,gb_pos_x_menu,gb_pos_y_menu,ID_COLOR_BLACK,ID_COLOR_WHITE);

 OSDMenuRowsDisplayScroll(ptrValue,0,aMax);
 
 while (salir == 0)
 {
  //SDL_PollEvent(gb_osd_sdl_event);
  //if(SDL_WaitEvent(gb_osd_sdl_event))
  {
   //if(gb_osd_sdl_event->type == SDL_KEYDOWN)
   {
    //switch(gb_osd_sdl_event->key.keysym.sym)
    {
     //case SDLK_UP:
     if (checkAndCleanKey(KEY_CURSOR_LEFT))
     {
      if (aReturn>10) aReturn-=10;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
     }
     if (checkAndCleanKey(KEY_CURSOR_RIGHT))
     {
      if (aReturn<(aMax-10)) aReturn+=10;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
     }

     if (checkAndCleanKey(KEY_CURSOR_UP))
     {
      //vga.setTextColor(WHITE,BLACK);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      ////SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),WHITE,BLACK,1);
      if (aReturn>0) aReturn--;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
      //vga.setTextColor(CYAN,BLUE);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      ////SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),CYAN,BLUE,1);
      ////break;
     }
     if (checkAndCleanKey(KEY_CURSOR_DOWN))
     {
     //case SDLK_DOWN: 
      //SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),WHITE,BLACK,1);
      //vga.setTextColor(WHITE,BLACK);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      if (aReturn < (aMax-1)) aReturn++;
      OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
      //vga.setTextColor(CYAN,BLUE);
      //vga.setCursor(gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3));
      //vga.print(ptrValue[aReturn]);
      ////SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((aReturn+1)<<3),CYAN,BLUE,1);
      ////break;
     }
     if (checkAndCleanKey(KEY_ENTER))
     {
      salir= 1;
     }
     //case SDLK_KP_ENTER: case SDLK_RETURN: salir= 1;break;
     if (checkAndCleanKey(KEY_ESC))
     {
      salir=1; aReturn= 255;    
     }
     //case SDLK_ESCAPE: salir=1; aReturn= 255; break;
     //default: break;
    }
    //SDL_Flip(gb_osd_sdl_surface);
    //SDL_PollEvent(gb_osd_sdl_event);
   }  
  }  
 } 
 gb_show_osd_main_menu= 0;
 return aReturn;
}

//Load SNA
void ShowTinySNAMenu()
{
 unsigned char aSelNum;     
 //aSelNum = ShowTinyMenu("SNA",gb_list_sna_title,max_list_sna);
 aSelNum = ShowTinyMenu("SNA",gb_ptr_sna_title,gb_max_sna);
 if (aSelNum==255)
 {
  return;
 }
  
 //gb_cartfilename= (char *)gb_list_rom_title[aSelNum];
 //gb_force_load_cart= 1;
 //gb_id_cur_cart= aSelNum;
 //running= 0;
 loadsnapshotFlash(aSelNum);
}

void ShowTinyDiscMenu()
{
 unsigned char aSelNum; 

 //aSelNum = ShowTinyMenu("Disc",gb_list_dsk_title,max_list_dsk);
 aSelNum= ShowTinyMenu("Disc",gb_ptr_dsk_title,gb_max_dsk);
 if (gb_dsk_id_cur == 255)
 {
  return;
 }
 gb_dsk_id_cur= aSelNum;
}


//Menu Cart
void ShowTinyCartMenu()
{
 unsigned char aSelNum,aSelKB; 

 aSelKB = ShowTinyMenu("CART SIZE",gb_value_size_cart_menu,max_gb_value_size_cart_menu); 
 if (aSelKB == 255)
 {
  return;
 }

 switch (aSelKB)
 {
  case 0: //16 KB cartuchos
   aSelNum = ShowTinyMenu("CART 16 KB",gb_list_cart_title_16KB,max_list_cart_16KB);
   if (aSelNum == 255)
   {
    return;
   }
   gb_array_rom[12-1]= (unsigned char *)gb_list_cart_data_16KB[aSelNum];
   break;
  case 1: //32 KB cartuchos
   aSelNum = ShowTinyMenu("CART 32 KB",gb_list_cart_title_32KB,max_list_cart_32KB);
   if (aSelNum == 255)
   {
    return;
   }   
   gb_array_rom[12-1]= (unsigned char *)gb_list_cart_data_32KB[aSelNum*2];
   gb_array_rom[13-1]= (unsigned char *)gb_list_cart_data_32KB[(aSelNum*2)+1];
   break;
  default: 
   return;
 }
 //loadcartFlash(aSelNum);
 //fread(&rom[12*0x4000],16384,1,f);

 reset6502();
 #ifdef use_lib_log_serial
  Serial.printf("Cart Load %d\n",aSelNum); 
 #endif 
}

//Menu ROM
//void ShowTinyMachineMenu()
//{
 //unsigned char aSelNum;
 //model = ShowTinyMenu("Machine",gb_machine_menu,max_gb_machine_menu);
 //loadroms2FlashModel();
 //switch (aSelNum)
 //{
 // case 0: model= 0; loadroms2FlashModel(0); break;
 // case 1: model= 1; loadroms2FlashModel(1); break;
 // case 2: model= 2; loadroms2FlashModel(2); break;
 // default: model= 0; loadroms2FlashModel(0); break;
 //}
 //vTaskDelay(2);
 //resetz80();
 //resetcrtc();
 //SDL_Delay(2);
//}


//Menu resetear
void ShowTinyResetMenu()
{
 unsigned char aSelNum;
 aSelNum= ShowTinyMenu("Reset",gb_reset_menu,max_gb_reset_menu);   
 if (aSelNum==255)
 {
  return;
 }
 
 initmem();
 reset6502();
 initula();   

 if (aSelNum == 1)
 {
  ESP.restart();
 }
}

//Menu TAPE
/*void ShowTinyTAPEMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K TAPE",gb_list_tapes_48k_title,max_list_tape_48);
 load_tape2Flash(aSelNum);
}

//Play sound tape
void ShowTinySelectTAPEMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K TAPE",gb_list_tapes_48k_title,max_list_tape_48);
 Z80EmuSelectTape(aSelNum);
// gb_play_tape_current = aSelNum;
// gb_play_tape_on_now = 1;
}

//Menu SCREEN
void ShowTinySCRMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K SCREEN",gb_list_scr_48k_title,max_list_scr_48);
 load_scr2Flash(aSelNum);     
}
*/

//Menu velocidad emulador
void ShowTinySpeedMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("CPU SPEED",gb_delay_cpu_menu,max_gb_delay_cpu_menu);
 switch(aSelNum)
 {
    case 0: gb_cpu_auto=1; break;
    case 1: gb_cpu_auto=0; break;
 }
}

//void ShowTinyKeyboardPollMenu()
//{
//
//}

void ShowTinyVGAScaleMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("Video Scale",gb_bool_menu,max_gb_bool_menu);
 switch(aSelNum)
 {
    case 0: gb_use_video_scale= 1; break;
    case 1: gb_use_video_scale= 0; break;
 }
}





//*******************************************
void SDLActivarOSDMainMenu()
{     
 gb_show_osd_main_menu= 1;   
}



//Very small tiny osd
void do_tinyOSD() 
{
 int auxVol;
 int auxFrec;  
 unsigned char aSelNum;
 if (checkAndCleanKey(KEY_F1))
 {
  gb_show_osd_main_menu= 1;
  #ifdef use_lib_sound_ay8912  
   gb_silence_all_channels = 1;
  #endif    
  //return;
 }

 if (gb_show_osd_main_menu == 1)
 {
  //#ifdef use_lib_log_serial
  // Serial.printf("Dentro menu OSD\n");
  //#endif 
  auxVol= gb_volumen01;
  auxFrec= gb_frecuencia01;
  gb_volumen01= gb_frecuencia01=0;
  //Audio PWM ledcWriteTone(1,0);
  //#ifdef use_lib_audio_tone32
  // Tone32_noTone(SPEAKER_PIN, SPEAKER_CHANNEL);
  //#endif

  aSelNum = ShowTinyMenu("MAIN MENU",gb_main_menu,max_gb_main_menu);
  switch (aSelNum)
  {
   case 0:
    //ShowTinyCartMenu();
    ShowTinySNAMenu();
    gb_show_osd_main_menu=0;
    break;
   case 1:
    ShowTinyCartMenu();
    gb_show_osd_main_menu=0;
    break;
   case 2:
    ShowTinyDiscMenu();
    gb_show_osd_main_menu=0;
    break;
   case 3:
    ShowTinyResetMenu();
    gb_show_osd_main_menu=0;
    break;
   case 4: ShowTinySpeedMenu();
    gb_show_osd_main_menu=0;
    break;
   case 5: ShowTinyVGAScaleMenu();
    gb_show_osd_main_menu=0;
    break;
   default: break;
  }

  gb_volumen01= auxVol;
  gb_frecuencia01= auxFrec;  
  //Audio PWM ledcWriteTone(1,auxFrec);
  //#ifdef use_lib_audio_tone32
  // Tone32_tone(SPEAKER_PIN, gb_frecuencia01, 0, SPEAKER_CHANNEL);
  //#endif  
  
 }

 SDLClear();
 
 #ifdef use_lib_sound_ay8912
  gb_silence_all_channels = 0;
 #endif 
}

