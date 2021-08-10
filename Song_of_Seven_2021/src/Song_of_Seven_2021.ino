//#include <LCD_I2C_Teensy36.h>

#include "Global.h" //include file containing global variables

#include "Biosynth.h"
//Set to true the sensors used with the bioSynth for the project
Biosynth biosynth(true,true,false,false);
#include "Helpers.h"

void setup() {

  Serial.begin(9600);
  MIDISERIAL.begin(9600);   //find midi baudrate
  //delay(2000); // power-up safety delay
  
  
  setupAudioShield(); //argument is master volume
  biosynth.setup();
  setupSounds();
  SMF.begin(&test);
  //set callback functions
  SMF.setMidiHandler(midiCallback);
  MIDI.begin(MIDI_CHANNEL_OMNI);

 
  biosynth.openingMessage();
  openingMessageTimer.restart();


  
}

void loop() 
{
  
  updateMidi();
  if(MIDI.read())
  {
    Serial.print("it works");
  }
  // openingMessage();
  // checkSectionChange();
  // biosynth.update();
  // sgtl5000_1.volume(setVolume());
}
