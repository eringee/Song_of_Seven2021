#include "led.h"
#include "configuration.h"
#include "pins.h"
#include "biosensors.h"

namespace led{
    
    byte drawing_memory[4*3];         //  3 bytes per LED
    DMAMEM byte display_memory[4*12]; // 12 bytes per LED
    WS2812Serial leds(4, display_memory, drawing_memory, pins::hardware::led, WS2812_GRB);
    //Set the colors for the sensors here. !!!IT MIGHT NOT BE THE RIGHT ORDER!!!
    int led_colors [4][3] = {{250, 0, 250},  //HEART
                            {100, 255, 250},//GSR
                            {10, 48, 240},  //RESPIRATION
                            {140, 10, 240}};//UNUSED

    const int led_brightness{80};

    void initialize(){
        pinMode(pins::hardware::led, OUTPUT);
        leds.begin();
        
        for ( int i = 0 ; i <4 ; i++ )
        {      
            leds.setPixel(i,0,0,0);    
        }
        leds.show();

        Serial.println("Leds initialized");
    }

    void update(sample signals){

        // Serial.printf("%.2f,%.2f,%.2f,%.2f\n", signals.heart.sig, signals.gsr.scr, signals.resp.sig, signals.gsr2.scr);


        leds.setPixel(0,led_colors[0][0]*signals.heart.sig,
                        led_colors[0][1]*signals.heart.sig,
                        led_colors[0][2]*signals.heart.sig); 
        leds.setPixel(1,led_colors[1][0]*signals.gsr.scr,
                        led_colors[1][1]*signals.gsr.scr,
                        led_colors[1][2]*signals.gsr.scr); 
        leds.setPixel(2,led_colors[2][0]*signals.resp.sig,
                        led_colors[2][1]*signals.resp.sig,
                        led_colors[2][2]*signals.resp.sig); 
        leds.setPixel(3,led_colors[2][0]*signals.gsr2.scr,
                        led_colors[2][1]*signals.gsr2.scr,
                        led_colors[2][2]*signals.gsr2.scr); 
        
        leds.show();
    }

}//namespace led