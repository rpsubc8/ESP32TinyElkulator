#ifndef GB_GLOBALS_H
 #define GB_GLOBALS_H

 extern unsigned char gb_cpu_auto;
 extern unsigned char gb_use_video_scale; //usar escalado
 
 extern volatile unsigned char keymap[256];
 extern volatile unsigned char oldKeymap[256];

 extern unsigned char **gb_buffer_vga; //Puntero video 8 bits
 extern unsigned int **gb_buffer_vga32; //Puntero video 32 bits

 extern unsigned char gb_color_vga[256];

 //fast bitluni
 extern unsigned char gb_sync_bits;


 extern unsigned char gb_show_osd_main_menu;



 extern int gb_volumen01;
 extern int gb_frecuencia01;
 
 extern unsigned int gb_cont_loop;
 
 extern unsigned char gb_ula_mode_cur;
 extern unsigned char gb_ula_mode_last;
 

 //portESP32 extern unsigned char elkpal[256];
 //portESP32 extern unsigned char monopal[256];
 extern unsigned char gb_colors[256];

 extern unsigned char gb_keys[2][14][4]; 

//6502 registers
extern unsigned char a,x,y,s;
extern unsigned short pc;
extern struct TP
{
        int c,z,i,d,v,n;
} p; 

//ULA
extern struct TULA
{
        unsigned char ints,intc;
        unsigned char addrlo,addrhi;
        unsigned char mode;
        unsigned short addr;
        unsigned char receivebyte;
} ula;
extern int tapeon;
extern int soundena;

//Memory structures
extern unsigned char *mem[0x100];
extern int memstat[0x100];
extern unsigned char *ram;

#ifdef use_lib_rom_flash
#else
 extern unsigned char *rom; 
#endif

extern unsigned char *gb_array_rom[16]; //16 roms de 16384 bytes

#ifdef use_lib_os_flash
 //extern unsigned char * os;
 //extern const unsigned char gb_rom_os[];
#else
 extern unsigned char os[0x4000];
#endif 
extern int writeablerom[16];
extern unsigned char currom;


extern const char ** gb_ptr_sna_title;
extern const unsigned char **gb_ptr_sna_data;
extern unsigned short int gb_max_sna;

extern const char ** gb_ptr_dsk_title;
extern const unsigned int *gb_ptr_dsk_end;
extern const unsigned char ** gb_ptr_dsk_data;
extern unsigned short int gb_max_dsk;
extern unsigned char gb_dsk_id_cur; //El disco actual

#endif

 
