#include "audio_manager.h"
#include <ArduinoLog.h>
#include <Audio.h>
#include "pins.h"
#include "biosensors.h"

namespace audio_manager{

AudioControlSGTL5000     sgtl5000_1;    

void audio_shield_initialization(){
  
  pinMode(pins::audio_shield::volume,INPUT);  
  sgtl5000_1.enable();
  sgtl5000_1.dacVolume(map(analogRead(pins::audio_shield::volume),0,1023,0.0,0.8));
  sgtl5000_1.volume(0.8);//configuration::volume); //set master volume here
  sgtl5000_1.lineOutLevel(13);
  
  Log.infoln("Audio shield is initialized");
}


//Volume pot was not working because the volume was changed through .volume() and this only affect headphone out.
void setVolume(float v){
  sgtl5000_1.dacVolume(v);
}

void mute(bool on){
  if(on){
    sgtl5000_1.muteHeadphone();
    sgtl5000_1.muteLineout();
  }else{
    sgtl5000_1.unmuteHeadphone();
    sgtl5000_1.unmuteLineout();
  }
  
}
}//namespace audio_manager