#include <hardware/biosensors.h>
#include <pins.h>



namespace biosensors{

    Heart heart(pins::sensors::heart);
    SkinConductance sc1(pins::sensors::gsr);
    Respiration resp(pins::sensors::respiration);

    void initialize(){
     pinMode(pins::sensors::heart,INPUT);
     heart.reset();

     pinMode(pins::sensors::gsr,INPUT);
     sc1.reset();

     pinMode(pins::sensors::respiration,INPUT);
     resp.reset();
    }


    void update(){
        heart.update();
        sc1.update();
        resp.update();
    }

    sample sample_sensors(){
        static float smooth_heart = 0.5; //default value for smoothing out heart signal for EMA
        static float smooth_gsr = 0.5;   //default value for smoothing out sc1 signal for EMA
        static float smooth_respiration = 0.5;  //default value for smoothing out resp signal for EMA
    
        //retrieve signals   
        sample signals{heart.getNormalized(),sc1.getSCR(), resp.getNormalized()};

        //smooth signals
        smooth_heart += 0.1 * (signals.heart - smooth_heart);
        smooth_gsr += 0.5 * (signals.gsr - smooth_gsr);
        smooth_respiration += 0.5 * (signals.gsr - smooth_respiration); // I JUST COPIED THE GSR SMOOTH FUNCTION  PLEASE MODIFY

        signals.heart = smooth_heart;
        signals.gsr = smooth_gsr;
        signals.respiration = smooth_respiration;

        return signals;
    }
}//namespace biosensors