/**
 * @file configuration.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The configuration file replace the Global.h file in the older version of the project. Here you can configure
 *        rapidly variable when uploading the firmware into the boards.
 * @version 1.1
 * @date 2022-04-02
 */
#pragma once

#define LOG false                //Set to true if you want the sessions to be logged in the sd card
#define PLOT_SENSOR  false      //Set to true to print sensor value in the serial plotter
#define FOOT_PEDAL true         //Set to true if using the foot pedal in the project
#define REVERSE_ENCODER false   //Set to true if the encoder rotation needs to be reversed
#define ADVANCE_WITH_ENCODER true //Set to true if you want to advance sections using the encoder. If false, press the foot pedal to advance section
#define SEND_OVER_SERIAL true

namespace configuration{
    
    #if defined(MASTER)
        const int board_id{0};
    #elif defined(SLAVE)
        const int board_id{1};
    #endif
    const float volume{0.6}; //Initial volume of the board

    const int boot_delay{2000}; //Amount of time to wait before initialization
    const int confirmation_delay{3000}; //Time allowed to confirm section change
    const int opening_message_time{3000}; //Amount of time the welcome message will be displayed

    const int biosensors_sample_rate_ms{10}; //10 samples per second?  just checking (Erin) . It is in ms, so it samples every 10ms(Etienne)

}//namespace configuration
                                                
