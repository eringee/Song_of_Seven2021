/**
 * @file Biosynth.cpp
 * @author Erin Gee & Etienne Montenegro
 * @brief  implementation file of the Biosynth class
 * @version 1.1
 * @date 2022-04-02
 */
#include <Biosynth.h>
#include <ArduinoLog.h>

#include <hardware/lcd.h>
#include <hardware/biosensors.h>
#include <hardware/buttons.h>
#include <hardware/encoder.h>
#include <hardware/led.h>
#include <audio_manager.h>


void Biosynth::initialize(){
     
    encoder::initialize();
    button::initialize();
    biosensors::initialize();
    screen::initialize(); 
    led::initialize();

    #if LOG
        session_log.initialize();
        session_log.create_file();
        session_log.start_logging();
    #endif
    
    audio_manager::audio_shield_initialization();
    audio_manager::synths_initialization();   
}


void Biosynth::update(){

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

    if (biosynthSensorTimer.hasPassed(10)) {
        biosensors::update(); 
        biosynthSensorTimer.restart();
    }

    if( button::encoder.pressed() && lcd_state == 2){
     Log.warningln("Ending session");
     session_log.stop_logging();
    };
    

    //vv wrap this in a fucntion vv
    if( button::encoder.pressed() && lcd_state == 1){
        Log.infoln("Section change confirmed");
        last_section = current_section;
        current_section = current_encoder_value;
        audio_manager::change_scene(current_section);
        currentSectionMessage();
    }

    if(lcdUpdate.hasPassed(40)){
        openingMessage();
        sectionChange();
        verifyNoTouch();
        led::update(signals);
        lcdUpdate.restart();
    }

}


void Biosynth::openingMessage()
{
    static bool do_once{false};
    static Chrono timer;
    
    if(!do_once){
        
        sprintf(screen::buffer_line_1, "Hello!");
        sprintf(screen::buffer_line_2, "I am board #%d",configuration::board_id);
        screen::update();
        do_once = true;

    }else if(timer.hasPassed(configuration::opening_message_time) && do_once){
        currentSectionMessage();
        timer.restart();
        timer.stop();
    }
}


void Biosynth::currentSectionMessage()
{
    sprintf(screen::buffer_line_1, "  Section  %s    ",sections[current_section]);
    sprintf(screen::buffer_line_2, "                ");
    lcd_state = 2;
    screen::update();  
}


void Biosynth::sectionChange()
{
    if(current_encoder_value != current_section )
    {      
        sectionConfirmMessage(current_encoder_value);
        confirmTimer.start();    
    }
}


void Biosynth::sectionConfirmMessage(const int encoder_value)
{
    sprintf(screen::buffer_line_1, "  Section : %s", sections[encoder_value]);
    sprintf(screen::buffer_line_2, "   Confirm ?   ");
    lcd_state = 1;
    screen::update();  
}   


void Biosynth::verifyNoTouch()
{
    if(confirmTimer.hasPassed(configuration::confirmation_delay) && lcd_state == 1)
    {   
        encoder::set_value(current_section);
        currentSectionMessage();
        confirmTimer.restart();
        confirmTimer.stop();
    }
}


#if PLOT_SENSOR
    void Biosynth::plot_sampled_data(sample signals){

        Serial.printf("%.2f,%.2f,%.2f",signals.heart,signals.gsr,signals.respiration);
        Serial.println();
    }
#endif