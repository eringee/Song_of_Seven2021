/**
 * @file led.h
 * @author Etienne Montenegro
 * @brief Module to interface with the synth LEDs 
 * @version 1.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <WS2812Serial.h>

struct sample;

namespace led{
    extern WS2812Serial leds;

    /** @brief Initialization routine 
     */
    void initialize();
    
    /**
     * @brief Update the brightness of the led with the biodata sample passed
     * 
     * @param signals biodata sample
     */
    void update(sample signals);
}//namespace led