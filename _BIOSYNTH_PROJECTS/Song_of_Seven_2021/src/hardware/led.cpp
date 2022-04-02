#include <hardware/led.h>
#include <configuration.h>
#include <pins.h>
#include <ArduinoLog.h>
#include <hardware/biosensors.h>

namespace led{
    
    byte drawing_memory[configuration::number_of_leds*3];         //  3 bytes per LED
    DMAMEM byte display_memory[configuration::number_of_leds*12]; // 12 bytes per LED
    WS2812Serial leds(configuration::number_of_leds, display_memory, drawing_memory, pins::hardware::led, WS2812_GRB);


    void initialize(){
        pinMode(pins::hardware::led, OUTPUT);
        leds.begin();

        

        for ( int i = 0 ; i <configuration::number_of_leds ; i++ )
        {      
            leds.setPixel(i,0,0,0);    
        }
        leds.show();

        Log.infoln("Leds initialized");
    }

    void update(const sample& signals){

        leds.setPixel(0,configuration::led_colors[0][0]*signals.heart,configuration::led_colors[0][1]*signals.heart,configuration::led_colors[0][2]*signals.heart); 
        leds.setPixel(0,configuration::led_colors[1][0]*signals.gsr,configuration::led_colors[1][1]*signals.gsr,configuration::led_colors[1][2]*signals.gsr); 
        leds.setPixel(0,configuration::led_colors[2][0]*signals.respiration,configuration::led_colors[2][1]*signals.respiration,configuration::led_colors[2][2]*signals.respiration); 
        
        leds.show();
    }

}//namespace led