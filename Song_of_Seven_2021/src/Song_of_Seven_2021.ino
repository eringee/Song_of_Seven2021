#include "Global.h" //include file containing global variables
#include "Biosynth.h"
//Set to true the sensors used with the bioSynth for the project
Biosynth biosynth(true,true,false,false);
#include "Helpers.h"

void setup() {

  Serial.begin(9600);

  //delay(2000); // power-up safety delay
  
  setupAudioShield(); //argument is master volume
  biosynth.setup();
  setupSounds();
  setupEnvelopes();
  
  openingMessageTimer.restart();
  
}

void loop() 
{
  openingMessage();
  checkSectionChange();
  biosynth.update(); 
  sgtl5000_1.volume(setVolume());
  
}
