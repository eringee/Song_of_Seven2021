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

}
