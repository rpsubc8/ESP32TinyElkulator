#ifndef _6502_H
 #define _6502_H
 #include <stdio.h>
 void dumpregs(void);
 void reset6502(void);
 void updateadfs(void);
 void loadcart(char *fn);
 void loadcart2(char *fn);
 void unloadcart(void);
 void load6502state(FILE *f);
 void save6502state(FILE *f);
 void initmem(void);
 void loadroms(void);
 void exec6502(int lines, int cpl);
 
 void load6502stateFlash(unsigned char id);
#endif
