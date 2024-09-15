#include "biosensors.h"
#include <Chrono.h>
#include "configuration.h"
#include "pins.h"
#include "ExternalADC.h"

namespace biosensors{

    Heart heart(pins::sensors::heart);
    SkinConductance sc1(pins::sensors::gsr);
    SkinConductance sc2(pins::sensors::gsr2);
    Respiration resp(getADCValue);

    ADS1115 ADS(pins::sensors::respiration);

    void initialize(){
     pinMode(pins::sensors::heart,INPUT);
     heart.reset();

     pinMode(pins::sensors::gsr,INPUT);
     sc1.reset();

     pinMode(pins::sensors::gsr2,INPUT);
     sc2.reset();

     resp.reset();
     ADS.begin();                  // external ADC
     ADS.setMode(0);               // continuous mode

     Serial.println("Biosensors initialized");
    }

    int getADCValue(){
      static int value = 0;
      static int firstValue = 0;
      if (firstValue == 0){
       firstValue = ADS.readADC(pins::sensors::respiration);            // first reading   
      } else {
       value = ADS.getValue();
      }
      return value; 
     } 


    void update(){
       
            heart.update();
            sc1.update();
            resp.update();
            sc2.update();
        
    }

}//namespace biosensors