/**
 * @file Song_of_Seven_2021.ino
 * @author Erin Gee & Etienne Montenegro
 * @brief Refactored version of the Song of Seven project. everything has been wrapped up into the biosynth class to have
 *        cleaner main file. The projects is now split in multiple files for easier maintenance.
 * 
 *        The files in the hardware folder are simple interface to communicate with the projects components. You should not
 *        have to modify much in these. 
 * 
 *        The audio_manager file  is where all the functions and variables releated to sounds are for the project
 *        
 *        The Biosynth class interacts with the multiple components. This is where you should work if you want to add actions
 *        or beavhiors to the synth
 * 
 *        The configuration file is where you can tweak multiples variables of the biosynth 
 * 
 *        The pins file is where the pin attribution is made. If you ever change pins it should be done there
 * 
 *        The logger class manages the logging of the sessions
 * 
 *        Project structure : 
 *        src/ 
 *           |-audio_manager.cpp
 *           |-audio_manager.h
 *           |-biosensor.cpp 
 *           |-biosensor.h
 *           |-buttons.cpp
 *           |-buttons.h
 *           |-Biosynth.cpp
 *           |-Biosynth.h
 *           |-configuration.h
 *           |-enc.cpp
 *           |-enc.h
 *           |-lcd.cpp
 *           |-lcd.h
 *           |-led.cpp
 *           |-led.h
 *           |-Logger.cpp
 *           |-Logger.h
 *           |-pins.h
 *           |-Song_of_Seven_2021.ino
 * 
 *        Project Dependencies:
 *          -ArduinoLog         (https://github.com/thijse/Arduino-Log)
 *          -Chrono             (https://github.com/SofaPirate/Chrono)
 *          -Biodata            (https://github.com/eringee/BioData)
 *          -LiquidCrystal_I2C  (https://github.com/johnrickman/LiquidCrystal_I2C)
 *          -WS2812Serial       (https://github.com/PaulStoffregen/WS2812Serial)
 *          -CircularBuffer     (https://github.com/rlogiacco/CircularBuffer)
 *          -Bounce2            (provided with teensyduino)
 *          -Encoder            (provided with teensyduino)
 *          -Audio              (provided with teensyduino)
 *          -SD                 (provided with teensyduino)
 *        
 * @version 1.1
 * @date 2022-04-02
 */
#include <Arduino.h>
#include <ArduinoLog.h>
#include "configuration.h"
#include "Biosynth.h"

Biosynth biosynth{};

void setup() {

  Serial.begin(9600);
  delay(configuration::boot_delay);
  
  //To silent the logging replace LOG_LEVEL_INFO with LOG_LEVEL_SILENT
  //I recommend leaving at least LOG_LEVEL_WARNING to print errors al least
  Log.begin(LOG_LEVEL_INFO,&Serial); 
  
  biosynth.initialize();
}

void loop() 
{ 
   biosynth.update();
  


  //vvv what are these two block suposed to do? vvv
  // Is the foot pedal reseting the synth now?

  //if(pedalMarker==0){ 
  //  footPedal.update();
  //  if (footPedal.read()==LOW) {
  //    pedalMarker=1;
  //    openingMessageTimer.restart();
  //  }
 // }

  // if(pedalMarker==1) {
  //   setupSounds();
  //   opening_message();
  //   checkSectionChange();
  //   biosynth.update();
  // }
  

}
