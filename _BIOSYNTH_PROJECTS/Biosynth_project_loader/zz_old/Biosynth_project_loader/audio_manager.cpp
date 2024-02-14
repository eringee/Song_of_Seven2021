#include "audio_manager.h"
#include <ArduinoLog.h>
#include <Audio.h>
#include "pins.h"
#include "biosensors.h"

namespace audio_manager{

AudioControlSGTL5000     sgtl5000_1;    

void audio_shield_initialization(){
  
  pinMode(pins::audio_shield::volume,INPUT);  
  
  sgtl5000_1.volume(configuration::volume); //set master volume here
  sgtl5000_1.lineOutLevel(13);
  sgtl5000_1.enable();
  Log.infoln("Audio shield is initialized");
}

}//namespace audio_manager