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
    
    //Pins 0 and 1 are used for Serial communication over Serial1

    // namespace sd{
    //     const int cs{10};     //any digital pin
    //     const int mosi{7};    //alternative can be 11
    //     const int sck{14};    //alternative can be 13    
    // }//namespace sd

    namespace audio_shield{
        // Pin used for audio data 9, 11, 13, 22, 23
        const int volume{15};
    }//namespace audio_shield

    namespace sensors{
        const int heart{21};
        const int gsr{20};
        const int respiration{2};
        const int gsr2{16};

    }//namespace sensors

    namespace hardware{
        const int led {31}; //using Serial2. On version 2.1 of the PCB this pad is shorted to pin 0 in order to connect it to the board. This makes Serial 1 unusable.
        const int encoder_A {5};
        const int encoder_B {6};
        const int encoder_button {2};
        const int foot_pedal {3};
    }//namespace hardware

}//namespace pins
