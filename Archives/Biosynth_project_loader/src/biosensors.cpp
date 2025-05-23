#include "biosensors.h"
#include <Chrono.h>
#include "configuration.h"
#include "pins.h"
#include "ExternalADC.h"

namespace biosensors{

    Heart heart(pins::sensors::heart);
    SkinConductance sc1(pins::sensors::gsr);
    SkinConductance sc2(pins::sensors::gsr2);
    Respiration resp;

    ADS1115 ADS(pins::sensors::respiration);

    void initialize(){

     pinMode(pins::sensors::heart,INPUT);
     heart.initialize();

     pinMode(pins::sensors::gsr,INPUT);
     sc1.initialize();

     pinMode(pins::sensors::gsr2,INPUT);
     sc2.initialize();

     resp.initialize();

     ADS.begin();                  // external ADC
     ADS.setMode(0);               // continuous mode

     Serial.println("Biosensors initialized");
    }

    int respSignal(){
      static int value = 0;
      static int firstValue = 0;
      if (firstValue == 0){
       firstValue = ADS.readADC(pins::sensors::respiration);            // first reading   
      } else {
       value = ADS.getValue();
      }
      return value; 
     } 

    int heartSignal(){
      static int value = 0;
      value = analogRead(pins::sensors::heart);
      return value;
    }

    int skinSignal(){
      static int value = 0;
      value = analogRead(pins::sensors::gsr);
      return value;
    }

    int skinSignal2(){
      static int value = 0;
      value = analogRead(pins::sensors::gsr2);
      return value;
    }

    void update(){
            heart.update(heartSignal());
            sc1.update(skinSignal());
            resp.update(respSignal());
            sc2.update(skinSignal2());   
    }

}//namespace biosensors