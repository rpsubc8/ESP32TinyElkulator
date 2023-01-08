#ifndef _GB_DSK_H
 #define _GB_DSK_H

 //#include <stddef.h>

 #include "dsk/dskWelcome.h"
 
 
 #define max_list_dsk 1
 
 static const char * gb_list_dsk_title[max_list_dsk]={  
  "Welcome"
 };
 
 //Posicion ultima de datos es 0x5A
 static const unsigned int gb_list_dsk_end[max_list_dsk]={  
  0x2F600
 };
 
 //Datos DSK
 static const unsigned char * gb_list_dsk_data[max_list_dsk]={  
  gb_dsk_welcome
 };

#endif


