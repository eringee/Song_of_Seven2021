
#include "Global.h" //include file containing global variables
#include "Biosynth.h"

Biosynth biosynth{&leds}; //Set to true the sensors used with the bioSynth for the project

#include "Helpers.h"
// 0 - LEDS
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

void setup() {

  Serial.begin(9600);
  AudioMemory(10);
  setupAudioShield(); 
  biosynth.setup();
  biosynth.openingMessage();
}

void loop() 
{ 
  biosynth.update();
  openingMessage();

  
  if(pedalMarker==0){
    footPedal.update();
    if (footPedal.read()==LOW) {
      pedalMarker=1;
      openingMessageTimer.restart();
    }
  }

  
  if(pedalMarker==1) {
    setupSounds();
    openingMessage();
    checkSectionChange();
    biosynth.update();
  }
  
}
