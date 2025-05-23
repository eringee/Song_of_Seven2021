/**
 * @file Biosynth.cpp
 * @author Erin Gee & Etienne Montenegro
 * @brief  implementation file of the Biosynth class
 * @version 1.1
 * @date 2022-04-02
 */
#include "Biosynth.h"
#include <ArduinoLog.h>

#include "lcd.h"
#include "biosensors.h"
#include "buttons.h"
#include "enc.h"
#include "led.h"
#include "audio_manager.h"


void Biosynth::initialize(){
     
    encoder::initialize();
    button::initialize();
    biosensors::initialize();
    screen::initialize(); 
    led::initialize();

    #if LOG
        session_log.initialize();
    #endif
    
    audio_manager::audio_shield_initialization();
    audio_manager::synths_initialization();   
}


void Biosynth::update(){

    if(biosynthSensorTimer.hasPassed(configuration::biosensors_sample_rate)) {
        biosensors::update(); 
        biosynthSensorTimer.restart();
    }

    sample signals{ biosensors::sample_sensors()};
    
    #if PLOT_SENSOR
        plot_sampled_data(signals);
    #endif

    audio_manager::update(signals);
    
    #if LOG
        session_log.log_data(signals);
    #endif

    current_encoder_value = encoder::update();
    button::update();

    #if LOG
        maybe_start_logging();
        maybe_stop_logging();
    #endif

    #if ADVANCE_WITH_ENCODER
        maybe_confirm_section_change();
    #else
        if( button::foot_pedal.pressed() && lcd_state == 2){
        advance_section();
        Log.warningln("Foot pedal pressed. Advanced section");
        };
    #endif


    if(lcdUpdate.hasPassed(40)){
        opening_message();

        #if LOG
            start_logging_message(true);
            stop_logging_message(true);
        #endif
    
        #if ADVANCE_WITH_ENCODER
            section_change();
            verify_no_touch();
        #endif
    
        led::update(signals);
        lcdUpdate.restart();
    }
}

#if LOG
    void Biosynth::maybe_start_logging(){
        
        if( button::encoder.pressed() && lcd_state == 2 && !session_log.is_logging()){
            Log.warningln("Starting session");
            session_log.create_file();
            session_log.start_logging();
            start_logging_message(false);
        };
    }

    void Biosynth::maybe_stop_logging(){
        
        if( button::encoder.pressed() && lcd_state == 2  && session_log.is_logging()){
            Log.warningln("Ending session");
            session_log.stop_logging();
            stop_logging_message(false);
        };
    }

#endif

void Biosynth::opening_message()
{
    static bool do_once{false};
    static Chrono timer;
    
    if(!do_once){
        
        sprintf(screen::buffer_line_1, "Hello!");
        sprintf(screen::buffer_line_2, "I am board #%d",configuration::board_id);
        screen::update();
        do_once = true;

    }else if(timer.hasPassed(configuration::opening_message_time) && do_once){
        current_section_message();
        timer.restart();
        timer.stop();
    }
}

void Biosynth::maybe_confirm_section_change(){
    
    if( button::encoder.pressed() && lcd_state == 1){
        Log.infoln("Section change confirmed");
        last_section = current_section;
        current_section = current_encoder_value;
        audio_manager::change_scene(current_section);
        current_section_message();
    }
}

void Biosynth::current_section_message()
{
    sprintf(screen::buffer_line_1, "  Section  %s    ",sections[current_section]);
    sprintf(screen::buffer_line_2, "                ");
    lcd_state = 2;
    screen::update();  
}

void Biosynth::section_change()
{
    if(current_encoder_value != current_section )
    {      
        section_confirm_message(current_encoder_value);
        confirmTimer.start();    
    }
}

void Biosynth::start_logging_message(bool do_once)
{   
    static Chrono timer;
    if(!do_once){
        timer.restart();
        sprintf(screen::buffer_line_1, "Logging Started");
        sprintf(screen::buffer_line_2, "               ");
        lcd_state = 3;
        screen::update();
    }else if( timer.hasPassed(3000)&&do_once){
        current_section_message();
        timer.restart();
        timer.stop();

    }
  
}

void Biosynth::stop_logging_message(bool do_once)
{   
    static Chrono timer;
    
    if(!do_once){
        timer.restart();
        sprintf(screen::buffer_line_1, "Logging Stopped");
        sprintf(screen::buffer_line_2, "               ");
        lcd_state = 4;
        screen::update();
    
    }else if( timer.hasPassed(3000)&&do_once){
        current_section_message();
        timer.restart();
        timer.stop();

    } 
}

void Biosynth::section_confirm_message(const int encoder_value)
{
    sprintf(screen::buffer_line_1, "  Section : %s", sections[encoder_value]);
    sprintf(screen::buffer_line_2, "   Confirm ?   ");
    lcd_state = 1;
    screen::update();  
}   

void Biosynth::verify_no_touch()
{
    if(confirmTimer.hasPassed(configuration::confirmation_delay) && lcd_state == 1)
    {   
        encoder::set_value(current_section);
        current_section_message();
        confirmTimer.restart();
        confirmTimer.stop();
    }
}

void Biosynth::advance_section(){
    current_section++;
    current_section = current_section%configuration::number_of_sections;
    audio_manager::change_scene(current_section);
    current_section_message();
};

#if PLOT_SENSOR
    void Biosynth::plot_sampled_data(sample signals){

        Serial.printf("%.2f,%.2f,%.2f",signals.heart,signals.gsr,signals.respiration);
        Serial.println();
    }
#endif