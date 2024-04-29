/**
 * @file Biosynth project loader.ino
 * @author Erin Gee & Etienne Montenegro
 * @brief Biosynth version that allows multiples project into the same firmware. Project selection is done during boot process
 *        In order to add projects you have to create a class that inherit from the project class.
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
  //I recommend leaving at least LOG_LEVEL_WARNING to print errors at least
  Log.begin(LOG_LEVEL_INFO,&Serial); 
  
  biosynth.initialize();
  Log.warningln("End of setup");
}

void loop() 
{ 
  biosynth.update();
  
  //vvv what are these two block suposed to do? vvv
  // Is the foot pedal reseting the synth now?

  // Answer: It was just something simple to test the function of the cable.

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
