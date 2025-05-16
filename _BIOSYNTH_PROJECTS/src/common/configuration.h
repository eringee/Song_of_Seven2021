/**
 * @file configuration.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The configuration file replace the Global.h file in the older version of the project. Here you can rapidly configure
 *        variables when uploading the firmware into the boards.
 * @version 1.1
 * @date 2022-04-02
 */
#pragma once

namespace configuration{
    
    const int version_year{2025};

    #ifdef BOARD_ID
    const int board_id{BOARD_ID};
    #else
    const int board_id{0};  
    #endif
 
    const float volume{0.6}; //Initial volume of the board

    const int boot_delay{500}; //Amount of time to wait before initialization
    const int confirmation_delay{3000}; //Time allowed to confirm section change
    const int opening_message_time{3000}; //Amount of time the welcome message will be displayed

    const int biosensors_sample_rate_us{10000}; // collect a sample every 10ms

}//namespace configuration
                                                
