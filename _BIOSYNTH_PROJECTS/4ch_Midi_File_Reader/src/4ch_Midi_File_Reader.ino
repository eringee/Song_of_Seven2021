
/*=========================SETUP INSTRUCTIONS==========================
 * 1) Design your audio pipeline in the teensy audio design tool (https://www.pjrc.com/teensy/gui/index.html ) and 
    replace  the output code in Global.h at line 50 with your new pipeline. 
    
    Each synth that you want to be played by the midi file needs an envelope that must be named ( envelopeHeart,
    envelopeGSR1 , envelopeGSR2 , envelopeTemp ) to be linked with the channels in the midi file.

   2) (optional) Write the code to setup the initial state of the synths and the envelopes in Helpers.h   *setupSounds() line 186  *setupEnvelopes() line 224.
   
  3) Add midi files to the root of your SD card. Add the names of up to (4) midi files to the array at line 37 in the file Global.h with the .mid extension specified in the array.  
    !*!*!*! The order in which the filenames are written in the array is the order in which you'll be able to select them.
    !*!*!*! The first name written will be the first file that will play.

   4) Assign voices to midi channels in the playNote() method in Biosynth.h line 528.  This function receives channel, the note number and its velocity as parameters.

*/
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
