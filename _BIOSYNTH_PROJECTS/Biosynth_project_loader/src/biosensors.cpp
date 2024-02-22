#include "biosensors.h"
#include <Chrono.h>
#include "configuration.h"
#include "pins.h"
#include <ArduinoLog.h>


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

     Log.infoln("Biosensors initialized");
    }


    void update(){
       
            heart.update();
            sc1.update();
            resp.update();
            sc2.update();
        
    }


     sample sample_sensors(){
        static float smooth_heart = 0.5; //default value for smoothing out heart signal for EMA
        static float smooth_gsr = 0.5;   //default value for smoothing out sc1 signal for EMA
        static float smooth_respiration = 0.5;  //default value for smoothing out resp signal for EMA
    
        //retrieve signals   
        sample signals{};
        signals.heart.sig = heart.getNormalized();
        signals.heart.amp = heart.amplitudeChange();
        signals.heart.bpm = heart.getBPM();

        signals.gsr.sig = sc1.getRaw();
        signals.gsr.scl = sc1.getSCL();
        signals.gsr.scr = sc1.getSCR();

        signals.resp.sig =resp.getNormalized();
        signals.resp.bpm =resp.getBPM();
        signals.resp.amp =resp.amplitudeChange();
        
        signals.gsr2.sig = sc2.getRaw();
        signals.gsr2.scl = sc2.getSCL();
        signals.gsr2.scr = sc2.getSCR();
        
        // //smooth signals
         smooth_heart += 0.1 * (signals.heart.sig - smooth_heart);
        // smooth_gsr += 0.5 * (signals.gsr - smooth_gsr);
         smooth_respiration += 0.5 * (signals.resp.sig - smooth_respiration); // I JUST COPIED THE GSR SMOOTH FUNCTION  PLEASE MODIFY

         signals.heart.sig = smooth_heart;
        // signals.gsr = smooth_gsr;
         signals.resp.sig = smooth_respiration;

        return signals;
    }
}//namespace biosensors