# ESP32TinyElkulator
Port x86 Elkulator Acorn Electron version 0.5 (author Tom Walker) to TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4) board with ESP32.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyElkulator/main/preview/elkulatorpreview.gif'></center>
<br>
I have made several modifications:
<ul>
 <li>Running on ESP32 with 520 KB of RAM (TTGO VGA32)</li>
 <li>Use of a single core</li>
 <li>Low-income OSD</li>
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>Loading ROMs in Flash progmem</li>
 <li>Loading SNA (version 0.5) in Flash progmem</li>
 <li>Loading ADF disk in Flash progmem</li>
 <li>8 and 64 color mode support (reduced version by Ricardo Massaro)</li>
 <li>Video mode 320x200</li>
 <li>Screen scale</li>
 <li>Precompiled version (flash download 3.9.2) 320x200</li>
</ul>


<br><br>
<h1>Precompiled version</h1>
In the precompile folder there is a version already compiled to be saved with the flash download tool 3.9.2. It is a version with 320x200 resolution with the demo games in FLASH.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyElkulator/tree/main/esp32/precompile/320x200'>https://github.com/rpsubc8/ESP32TinyElkulator/tree/main/esp32/precompile/320x200</a>
<br><br>
We must choose the ESP32 type:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyElkulator/main/preview/flash00.gif'></center>
Subsequently, select the files as shown in the attached screenshot, with the same offset values:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyElkulator/main/preview/flash01.gif'></center>
And we will press start. If everything has been correct, we will only have to restart the ESP32.
