#ifndef _GB_CART_H
 #define _GB_CART_H

 //#include <stddef.h>

 #include "cart/cartHopper.h"
 #include "cart/cartSnapper.h"
 #include "cart/cartCountDown32KB.h"
 #include "cart/cartStarship32KB.h"
 #include "cart/cartLisp32KB.h"
 
 
 #define max_list_cart_16KB 2
 #define max_list_cart_32KB 3
 
 static const char * gb_list_cart_title_16KB[max_list_cart_16KB]={
  "Hopper"
  ,"Snapper"
 };
 
 
 //Datos CART 16 KB
 static const unsigned char * gb_list_cart_data_16KB[max_list_cart_16KB]={
  gb_cart_hopper
  ,gb_cart_snapper
 };


 static const char * gb_list_cart_title_32KB[max_list_cart_32KB]={
  "Count Down"
  ,"Star Ship Command"
  ,"LISP"
 };
 
 //Datos CART 32 KB
 static const unsigned char * gb_list_cart_data_32KB[max_list_cart_32KB*2]={
  gb_cart_countDownToDoom_0, gb_cart_countDownToDoom_1
  ,gb_cart_starshipCommand_0, gb_cart_starshipCommand_1
  ,gb_cart_lisp_0, gb_cart_lisp_1
 }; 

#endif

