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
#define ADVANCE_WITH_ENCODER false //Set to true if you want to advance sections using the encoder. If false, press the foot pedal to advance section

namespace configuration{
    
    const int board_id{7};
    const int number_of_boards{7}; 

    const int number_of_sections{4}; //could be move to audio_manager maybe
  
    const float volume{0.5}; //Initial volume of the board

    const int number_of_leds{4};  //Only 3 are used now 
    const int led_brightness{16}; //maximum led brightness
    
    //Set the colors for the sensors here. !!!IT MIGHT NOT BE THE RIGHT ORDER!!!
    const int led_colors [number_of_leds][3] = {{250, 0, 250},  //HEART
                                                {100, 255, 250},//GSR
                                                {10, 48, 240},  //RESPIRATION
                                                {140, 10, 240}};//UNUSED

    const int boot_delay{2000}; //Amount of time to wait before initialization
    const int confirmation_delay{3000}; //Time allowed to confirm section change
    const int opening_message_time{3000}; //Amount of time the welcome message will be displayed

    const int biosensors_sample_rate{10};

}//namespace configuration
                                                