//Elkulator 0.5 by Tom Walker
//Snapshot loading/saving
#include <stdio.h>
#include "snapshot.h"
#include "6502.h"
#include "ula.h"
#include "gbGlobals.h"
//#include "dataFlash/gbsna.h"

int loadsnapshot(char *fn)
{
        unsigned char t[4];
        FILE *f=fopen(fn,"rb");
        if (!f) return 1;
        t[0]=getc(f); t[1]=getc(f); t[2]=getc(f); t[3]=getc(f);
        if (t[0]!='E' || t[1]!='S' || t[2]!='S' || t[3]!=0)
        {
                fclose(f);
                return 2;
        }
        load6502state(f);
        loadulastate(f);
        fclose(f);
        return 0;
}

int loadsnapshotFlash(unsigned char id)
{
 #ifdef use_lib_log_serial
  Serial.printf("loadsnapshotFlash %d\n",id);
 #endif  
        unsigned char t[4];
        unsigned int cont=0;
        //FILE *f=fopen(fn,"rb");
        //if (!f) return 1;
        //t[0]=gb_list_sna_data[id][cont]; cont++;
        //t[1]=gb_list_sna_data[id][cont]; cont++;
        //t[2]=gb_list_sna_data[id][cont]; cont++;
        //t[3]=gb_list_sna_data[id][cont]; cont++;
        t[0]=gb_ptr_sna_data[id][cont]; cont++;
        t[1]=gb_ptr_sna_data[id][cont]; cont++;
        t[2]=gb_ptr_sna_data[id][cont]; cont++;
        t[3]=gb_ptr_sna_data[id][cont]; cont++;        
        if (t[0]!='E' || t[1]!='S' || t[2]!='S' || t[3]!=0)
        {
                //fclose(f);
                return 2;
        }        
        load6502stateFlash(id);
        loadulastateFlash(id);
        //fclose(f);
 //fflush(stdout);        
        return 0;
}


void savesnapshot(char *fn)
{
        FILE *f=fopen(fn,"wb");
        putc('E',f);
        putc('S',f);
        putc('S',f);
        putc(0,f);
        save6502state(f);
        saveulastate(f);
        fclose(f);
}
