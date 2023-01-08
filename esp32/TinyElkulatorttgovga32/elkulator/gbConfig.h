#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //Not use double numbers (IEEE not fpu) calculate vga frequency
 //#define use_lib_fix_double_precision

 //Debug i2s bitluni vga frequency
 //#define use_lib_debug_i2s

 //La placa WEMOS aun no esta. No usar
 //#define use_lib_board_wemos

 //Corrige teclados que no se inicializan(solucion dcrespo3D)
 //#define FIX_PERIBOARD_NOT_INITING

 //Usa la libreria bitluni modificada en C tiny fast
 #define use_lib_tinybitluni_fast

 //no usar cinta
 #define use_lib_no_tape
 
 //Selecionar modo video, solo uno (use_lib_vga320x200) 
 #define use_lib_vga320x200

 //Usar 8 colores de salida
 //#define use_lib_vga8colors

 //Logs
 #define use_lib_log_serial
 


 //Read keyboard poll x millis
 #define gb_ms_keyboard 10
 
 //Milisegundos polling vga
 #define gb_ms_vga 20
 
 //Delay millis iteration emulate 
 //No usarlo Pruebas
 //#define gb_delay_emulate_ms 0



 //La rom os en flash
 #define use_lib_os_flash
 
 //Las rom en flash
 #define use_lib_rom_flash

 #define use_lib_sna_flash
 
 #define use_lib_cart_flash
 
 //No usar el disco(ya no se usa)
 //#define use_lib_nodisk

 #define use_lib_disk_flash_test
 
 //buffer look y lookup2 de 32 bits
 #define use_lib_optimize_lookup_32bits
 
 //Fuerza uso de 200 lineas en vez de 256 en pantalla
 #define use_lib_320x200 


#endif
