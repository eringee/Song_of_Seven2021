/**
 * @file pins.h
 * @author Erin Gee & Etienne Montenegro
 * @brief All the pins definition are made in this file. Change the number of the pin here if you ever need to
 *        I have wrapped the variables in namespaces for cleaner code.
 * @version 1.1
 * @date 2022-04-02

 */
#pragma once

namespace pins{
    
    namespace sd{
        const int cs{10};
        const int mosi{7};
        const int sck{14};        
    }//namespace sd

    namespace audio_shield{
        const int volume{15};
    }//namespace audio_shield

    namespace sensors{
        const int heart{21};
        const int gsr{20};
        const int respiration{17};
        const int gsr2{16};

    }//namespace sensors

    namespace hardware{
        const int led {31};
        const int encoder_A {5};
        const int encoder_B {6};
        const int encoder_button {2};
        const int foot_pedal {3};
    }//namespace hardware

}//namespace pins
