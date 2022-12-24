#ifndef _ULA_H
 #define _ULA_H
 unsigned char readula(unsigned short addr);
 unsigned char readkeys(unsigned short addr);
 void writeula(unsigned short addr, unsigned char val);
 void drawline(int line);
 void tapecountover(void);
 //void restorepal(void); //No se usa por ahora
 //void fadepal(void); //No se usa por ahora
 void updateres(void);
 void loadulastate(FILE *f);
 void saveulastate(FILE *f);
 void receive(unsigned char val);
 void intula(unsigned char val);
 void initula(void);
 void initsound(void);
 
 void hline(short int x1, short int y, short int x2, unsigned char color);
 
 void loadulastateFlash(unsigned char id);
 //void put_pixel(unsigned int x, unsigned int y, unsigned char aColor);
 void SDLClearULA(void);
#endif
 
