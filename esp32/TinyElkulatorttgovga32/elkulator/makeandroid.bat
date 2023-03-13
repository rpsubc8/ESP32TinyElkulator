@echo off
echo Convert ANDROID PROJECT DATA (WIN32)
echo Delete dataFlash (Borrar dataFlash)
pause

echo Process gbcart.h (Procesar gbcart.h)
fart dataFlash\gbcart.h "cart/cart" "cart"
ping -n 2 127.0.0.1>nul

echo Process gbdsk.h (Procesar gbdsk.h)
fart dataFlash\gbdsk.h "dsk/dsk" "dsk"
ping -n 2 127.0.0.1>nul

echo Process gbos.h (Procesar gbos.h)
fart dataFlash\gbos.h "os/os" "os"
ping -n 2 127.0.0.1>nul

echo Process gbrom.h basic (Procesar gbrom.h)
fart dataFlash\gbrom.h "roms/basic.h" "basic.h"
ping -n 2 127.0.0.1>nul
echo Process gbrom.h dfs (Procesar gbrom.h)
fart dataFlash\gbrom.h "roms/dfs.h" "dfs.h"
ping -n 2 127.0.0.1>nul
echo Process gbrom.h adfs (Procesar gbrom.h)
fart dataFlash\gbrom.h "roms/adfs.h" "adfs.h"
ping -n 2 127.0.0.1>nul

echo Process gbsna.h (Procesar gbsna.h)
fart dataFlash\gbsna.h "sna/sna" "sna"
ping -n 2 127.0.0.1>nul



echo Process osd.cpp gbcart (Procesar osd.cpp)
fart osd.cpp "dataFlash/gbcart.h" "gbcart.h"
ping -n 2 127.0.0.1>nul

echo Process elkulator.ino gbdsk (Procesar elkulator.ino)
fart elkulator.ino "dataFlash/gbdsk.h" "gbdsk.h"
ping -n 2 127.0.0.1>nul

echo Process 6502.cpp gbos (Procesar 6502.cpp)
fart 6502.cpp "dataFlash/gbos.h" "gbos.h"
ping -n 2 127.0.0.1>nul

echo Process elkulator.ino gbos (Procesar elkulator.ino)
fart elkulator.ino "dataFlash/gbos.h" "gbos.h"
ping -n 2 127.0.0.1>nul

echo Process 6502.cpp gbrom (Procesar 6502.cpp)
fart 6502.cpp "dataFlash/gbrom.h" "gbrom.h"
ping -n 2 127.0.0.1>nul

echo Process elkulator.ino gbrom (Procesar elkulator.ino)
fart elkulator.ino "dataFlash/gbrom.h" "gbrom.h"
ping -n 2 127.0.0.1>nul

echo Process elkulator.ino gbsna (Procesar elkulator.ino)
fart elkulator.ino "dataFlash/gbsna.h" "gbsna.h"
ping -n 2 127.0.0.1>nul



echo Copy gbcart.h (Copiar gbcart.h)
copy/Y dataFlash\gbcart.h
ping -n 2 127.0.0.1>nul

echo Copy gbdsk.h (Copiar gbdsk.h)
copy/Y dataFlash\gbdsk.h
ping -n 2 127.0.0.1>nul

echo Copy gbos.h (Copiar gbos.h)
copy/Y dataFlash\gbos.h
ping -n 2 127.0.0.1>nul

echo Copy gbrom.h (Copiar gbrom.h)
copy/Y dataFlash\gbrom.h
ping -n 2 127.0.0.1>nul

echo Copy gbsna.h (Copiar gbsna.h)
copy/Y dataFlash\gbsna.h
ping -n 2 127.0.0.1>nul


echo Copy cart (Copiar cart)
copy/Y dataFlash\cart\*.h
ping -n 2 127.0.0.1>nul

echo Copy dsk (Copiar dsk)
copy/Y dataFlash\dsk\*.h
ping -n 2 127.0.0.1>nul

echo Copy os (Copiar os)
copy/Y dataFlash\os\*.h
ping -n 2 127.0.0.1>nul

echo Copy roms (Copiar roms)
copy/Y dataFlash\roms\*.h
ping -n 2 127.0.0.1>nul

echo Copy sna (Copiar sna)
copy/Y dataFlash\sna\*.h
ping -n 2 127.0.0.1>nul


echo Delete dataFlash (Borrar dataFlash)
rmdir /S /Q dataFlash

pause