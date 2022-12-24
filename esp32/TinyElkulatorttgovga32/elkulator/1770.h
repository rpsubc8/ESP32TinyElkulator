#ifndef _1770_H
 #define _1770_H
 unsigned char read1770(unsigned short addr);
 void write1770(unsigned short addr, unsigned char val);
 void reset1770(void);
 void savedisc(char *fn, int drv);
 void loaddisc(char *fn, int drv);
#endif
 
