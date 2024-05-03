#include "biosensors.h"
#include <Chrono.h>
#include "configuration.h"
#include "pins.h"



namespace biosensors{

    Heart heart(pins::sensors::heart);
    SkinConductance sc1(pins::sensors::gsr);
    SkinConductance sc2(pins::sensors::gsr2);
    Respiration resp(pins::sensors::respiration);

    void initialize(){
     pinMode(pins::sensors::heart,INPUT);
     heart.reset();

     pinMode(pins::sensors::gsr,INPUT);
     sc1.reset();

     pinMode(pins::sensors::gsr2,INPUT);
     sc2.reset();

     pinMode(pins::sensors::respiration,INPUT);
     resp.reset();

     Serial.println("Biosensors initialized");
    }


    void update(){
       
            heart.update();
            sc1.update();
            resp.update();
            sc2.update();
        
    }

}//namespace biosensors