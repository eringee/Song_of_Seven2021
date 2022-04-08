#include "audio_manager.h"
#include <ArduinoLog.h>
#include <Audio.h>
#include "pins.h"
#include "biosensors.h"

namespace audio_manager{

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform3;      //xy=1014,743
AudioSynthWaveform       waveform2;      //xy=1070,543
AudioSynthWaveformSineModulated sine_fm3;       //xy=1227,547
AudioSynthWaveformSine   sine1;          //xy=1231,587
AudioAmplifier           amp2;           //xy=1246,728
AudioSynthWaveformSine   sine_fm2;       //xy=1283,411
AudioSynthWaveformSineModulated sine_fm4;       //xy=1392,650
AudioAmplifier           amp1;           //xy=1414,361
AudioMixer4              mixerMain;      //xy=1553,454
AudioOutputI2S           AudioOut;       //xy=1712,455
AudioConnection          patchCord1(waveform3, amp2);
AudioConnection          patchCord2(waveform2, sine_fm3);
AudioConnection          patchCord3(sine_fm3, 0, mixerMain, 1);
AudioConnection          patchCord4(sine1, 0, mixerMain, 2);
AudioConnection          patchCord5(amp2, sine_fm4);
AudioConnection          patchCord6(sine_fm2, amp1);
AudioConnection          patchCord7(sine_fm4, 0, mixerMain, 3);
AudioConnection          patchCord8(amp1, 0, mixerMain, 0);
AudioConnection          patchCord9(mixerMain, 0, AudioOut, 0);
AudioConnection          patchCord10(mixerMain, 0, AudioOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1156,306
// GUItool: end automatically generated code



int piece_sections[configuration::number_of_sections]
                  [configuration::number_of_boards]
{
{622,933,1244,1555,1866,2177,2488},  // going to canada
{174,348,1244,1555,1566,1740,2488},  // cat died
{392,416,448,587,659,1046,1174},     // fluffy bunny
{1148,1312,1476,1640,1804,1968,2132} // board games
};


void audio_shield_initialization(){

  AudioMemory(24);
  pinMode(pins::audio_shield::volume,INPUT);  
  sgtl5000_1.enable();
  sgtl5000_1.volume(configuration::volume); //set master volume here
  Log.infoln("Audio shield is initialized");

}

void set_volume(){

  int potentiometer_reading{ analogRead(pins::audio_shield::volume) };
  float vol{ static_cast<float>(potentiometer_reading / 1024.00) * 0.8 }; //make sure the gain doesn't go louder than 0.8 to avoid clipping       
 
  for( int i = 0 ; i < 4 ; i++ )
      {    
          mixerMain.gain(i, vol);  //set all four channels of main mixer to follow gain knob
      } 

}

//!!!!DEFINE INITIAL SYNTH SOUND HERE!!!!
void synths_initialization(){
    
    //GSR dependent variables
    sine_fm2.frequency(piece_sections[0][configuration::board_id-1]); 
    sine_fm2.amplitude(0.5);
    
    //HEART dependent variables
    waveform3.begin(0.01, 0.005, WAVEFORM_SINE);
    sine_fm4.frequency(622);
    sine_fm4.amplitude(0.02);

    //ATMOSPHERIC SINES
    waveform2.begin(0.004, 1, WAVEFORM_SINE);
    sine_fm3.frequency(311);           //atmospheric sines
    sine1.frequency(424);
    sine_fm3.amplitude(0.1);
    sine1.amplitude(0.05);

    Log.infoln("Synths sounds are initialized");
}

//!!!!DEFINE CHANGE IN SOUND FOR SCENES HERE!!!!
void change_scene(const int& scene){
  
  sine_fm2.frequency(piece_sections[scene][configuration::board_id-1]);
  
  switch(scene){
    case 0:
      sine_fm3.frequency(311);   //atmospheric sine1
      sine1.frequency(424);      //atmospheric sine2
      break;

    case 1:
      sine_fm3.frequency(311);   //atmospheric sine1
      sine1.frequency(369);      //atmospheric sine2
      break;
    
    case 2:
      sine_fm3.frequency(261.63);   //atmospheric sine1
      sine1.frequency(329.63);      //atmospheric sine2
      sine_fm3.amplitude(0.1);  
      sine1.amplitude(0.1);
      break;

    case 3:
      waveform2.frequency(1);
      sine_fm3.frequency(659);   //atmospheric sine1
      sine1.frequency(985);      //atmospheric sine2
      sine_fm3.amplitude(0.1);  
      sine1.amplitude(0.1);
      break;
  }
  

}

//!!!!DEFINE SOUND LINK WITH BIODATA HERE!!!!
void update(const sample& signals){
  
  set_volume();

  //used smoothed signals to transform audio
  amp1.gain(signals.gsr);
  amp2.gain(signals.heart); 
  //add audio feature to modify with respiration here using signals.respiration

}


}//namespace audio_manager