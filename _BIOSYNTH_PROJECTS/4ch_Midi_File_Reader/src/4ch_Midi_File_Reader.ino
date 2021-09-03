

#include "Global.h" //include file containing global variables
#include "Biosynth.h"

//Set to true the sensors used with the bioSynth for the project
Biosynth biosynth(true,true,true,true);


#include "Helpers.h"

File root;

void setup() {

  Serial.begin(9600);
  delay(2000); // power-up safety delay
  
  
  setupAudioShield();
  biosynth.setup();

  //Set the synths and envelopes to their initial state for the performance
  setupSounds(); 
  setupEnvelopes();
  
  SMF.begin(&card);
  SMF.setMidiHandler(midiCallback);  //set callback functions

  biosynth.openingMessage();
  openingMessageTimer.restart();


}

void loop() 
{
  openingMessage(); //update the UI
  checkSectionChange(); //update the section system
  updateMidi(); //update the midifile reading
  biosynth.update(); //update the hardware 
  sgtl5000_1.volume(setVolume()); //update master volume level
}
