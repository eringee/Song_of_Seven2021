#include "biosensors.h"
#include <Chrono.h>
#include "configuration.h"
#include "pins.h"
#if defined(EXT_ADC_RESP) || defined(EXT_ADC_HEART) || defined(EXT_ADC_GSR1) || defined(EXT_ADC_GSR2)
#include "ExternalADC.h"
#endif

namespace biosensors{

    Heart heart;
    SkinConductance sc1;
    SkinConductance sc2;
    Respiration resp;

    #ifdef EXT_ADC_RESP
    ADS1115 ADS_RESP(pins::sensors::respiration);
    #endif

    #ifdef EXT_ADC_HEART
    ADS1115 ADS_HEART(pins::sensors::heart);
    #endif

    #ifdef EXT_ADC_GSR1
    ADS1115 ADS_GSR1(pins::sensors::gsr);
    #endif

    #ifdef EXT_ADC_GSR2
    ADS1115 ADS_GSR2(pins::sensors::gsr2);
    #endif

    void initialize(){

     heart.initialize();
     sc1.initialize();
     sc2.initialize();
     resp.initialize();

     #ifdef EXT_ADC_RESP
     ADS_RESP.begin();             // external ADC
     ADS_RESP.setMode(0);          // continuous mode
     #endif

     #ifdef EXT_ADC_HEART
     ADS_HEART.begin();            // external ADC
     ADS_HEART.setMode(0);         // continuous mode
     #endif

     #ifdef EXT_ADC_SC1
     ADS_GSR1.begin();             // external ADC
     ADS_GSR1.setMode(0);          // continuous mode
     #endif

     #ifdef EXT_ADC_SC2
     ADS_GSR2.begin();             // external ADC
     ADS_GSR2.setMode(0);          // continuous mode
     #endif

     Serial.println("Biosensors initialized");
    }

    int respSignal(){
      static int value = 0;

      #ifdef EXT_ADC_RESP
      static int firstValue = 0;
      if (firstValue == 0){
       firstValue = ADS_RESP.readADC(pins::sensors::respiration);            // first reading   
      } else {
       value = ADS_RESP.getValue();
      }
      #else
      value = analogRead(pins::sensors::respiration);
      #endif

      return value; 
     } 

    int heartSignal(){
      static int value = 0;

      #ifdef EXT_ADC_HEART
      static int firstValue = 0;
      if (firstValue == 0){
       firstValue = ADS_HEART.readADC(pins::sensors::heart);            // first reading   
      } else {
       value = ADS_HEART.getValue();
      }
      #else
      value = analogRead(pins::sensors::heart);
      #endif

      return value;
    }

    int skinSignal(){
      static int value = 0;

      #ifdef EXT_ADC_SC1
      static int firstValue = 0;
      if (firstValue == 0){
       firstValue = ADS_SC1.readADC(pins::sensors::gsr);            // first reading   
      } else {
       value = ADS_SC1.getValue();
      }
      #else
      value = analogRead(pins::sensors::gsr);
      #endif

      return value;
    }

    int skinSignal2(){
      static int value = 0;

      #ifdef EXT_ADC_SC2
      static int firstValue = 0;
      if (firstValue == 0){
       firstValue = ADS_SC2.readADC(pins::sensors::gsr2);            // first reading   
      } else {
       value = ADS_SC2.getValue();
      }
      #else
      value = analogRead(pins::sensors::gsr2);
      #endif
 
      return value;
    }

    void update(){
            heart.update(heartSignal());
            sc1.update(skinSignal());
            resp.update(respSignal());
            sc2.update(skinSignal2());   
    }

}//namespace biosensors