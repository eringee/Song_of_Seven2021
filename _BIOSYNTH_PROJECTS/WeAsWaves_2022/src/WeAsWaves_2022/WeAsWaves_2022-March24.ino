//#include <LCD_I2C_Teensy36.h>
#include "Global.h" //include file containing global variables
#include "Biosynth.h"
Biosynth biosynth{&leds}; // initiate the biosynth object

#include "Helpers.h"

//////////////////////////////////////HARDWARE DESIGNATIONS ON PCB////////////////////////////////////////////
// 0 - SERIAL RECEIVE
// 1 - SERIAL SEND
// 2 - Encoder Button / User Interface
// 3 - FootPedal Button / Activation Pin
// 4 - SD_ETHERNET ADAPTER
// 5 - ENCODER A 
// 6 - ENCODER B 
// 7 - MOSI 
// 8 - SD_ETHERNET ADAPTER / DISP DO 
// 9 - SD_ETHERNET ADAPTER
// 10 - CS PIN for SD card TEENSY AUDIO - ETH
// 11 - SD_ETHERNET ADAPTER
// 12 - SD_ETHERNET ADAPTER / DISP MISO 
// 13 - SD_ETHERNET ADAPTER
// 14 - (A0) DISP_SCK 
// 15 - (A1) DISP_CS (map to pot in audio designs?)
// 16 - (A2) GSR2
// 17 - (A3) Temp
// 18 - (A4) DISP SCL
// 19 - (A5) DISP SDA
// 20 - (A6) GSR1 
// 21 - (A7) PULSE 
// 22 - Audio
// 23 - Audio
// 31 - (back of Teensy) LEDS

void setup() {

  Serial.begin(9600);
  AudioMemory(80);
  setupAudioShield(); 
  biosynth.setup();
  biosynth.openingMessage();

}

void loop() 
{ 
  biosynth.update();
  openingMessage();
  
  if(pedalMarker==0){ // pedal usually reads nothing
    footPedal.update();
    if (footPedal.read()==LOW) {
      pedalMarker=1;
      //openingMessageTimer.restart();
    }
  }
  
  if(pedalMarker==1) {   //stuff to do once when you push the pedal
    setupSounds();
    openingMessage();
    checkSectionChange();
    biosynth.update();
  }
  


}
