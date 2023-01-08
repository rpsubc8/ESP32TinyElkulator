#ifndef _GB_SNA_H
 #define _GB_SNA_H

 //#include <stddef.h>

 #include "sna/crazeerider.h"
 #include "sna/snaInvaders.h"
 #include "sna/snaKillergorilla.h"
 #include "sna/snaStockcar.h"
 #include "sna/snaFelix.h"
 
 
 #define max_list_sna 5
 
 static const char * gb_list_sna_title[max_list_sna]={
  "Craze Erider"
  ,"Invaders"
  ,"Killer gorilla"
  ,"Stock car"
  ,"Felix"
 };
 
 //Datos SNA
 static const unsigned char * gb_list_sna_data[max_list_sna]={
  gb_sna_crazeerider
  ,gb_sna_invaders
  ,gb_sna_killergorilla
  ,gb_sna_stockcar
  ,gb_sna_felix
 };

#endif


