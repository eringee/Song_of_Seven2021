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
#include "configuration.h"
#include "Biosynth.h"
#include "PlaquetteLib.h"

Biosynth biosynth{};

void setup() {
  Plaquette.begin();
  Serial.begin(9600);
  delay(configuration::boot_delay);
  Wire.begin();
  
  
  biosynth.initialize();
  Serial.println("End of setup");
}

void loop() 
{ 
  Plaquette.step();
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
