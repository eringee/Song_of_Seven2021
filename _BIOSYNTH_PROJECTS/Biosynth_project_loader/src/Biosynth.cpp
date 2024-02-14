/**
 * @file Biosynth.cpp
 * @author Erin Gee & Etienne Montenegro
 * @brief  implementation file of the Biosynth class
 * @version 1.1
 * @date 2022-04-02
 */
#include "Biosynth.h"

#include <ArduinoLog.h>
#include <Chrono.h>

#include "audio_manager.h"
#include "biosensors.h"
#include "buttons.h"
#include "enc.h"
#include "lcd.h"
#include "led.h"
#include "Project_list.h"

void Biosynth::send_syn(){
            Serial1.clear();
     
            if (Serial1.availableForWrite()) {
                Serial1.println(pingCommand);

            }
        Log.infoln("SYN sent");
}

void Biosynth::send_synack(){
 Serial1.clear();
     
            if (Serial1.availableForWrite()) {
                Serial1.println(synackCommand);

            }
             Log.infoln("SYNACK sent");
}

void Biosynth::send_ack(){
    Serial1.clear();
    if (Serial1.availableForWrite()) {
        
        Serial1.println(confirmCommand);
    }
    Log.infoln("ACK sent");
}


bool Biosynth::wait_for_synack(const int timeout){
    Log.traceln("wait_for_synack");
    int current = millis();
    int elapsed = 0;
    Serial1.clear();
    while(elapsed < timeout){
        if(Serial1.available()>= sizeof(synackCommand)){
                         char buff[sizeof(synackCommand)];
                         Serial1.readBytes(buff,sizeof(synackCommand));
                         Log.infoln("%s",buff);
                         if (strcmp(buff,synackCommand) == 0){
                            return true;
                         }
                    }
        elapsed = millis() - current;
    }
    Log.warningln("waiting for syncack timed out");
    return false;
}


void Biosynth::recvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;
    
    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();
        Serial.println(rc);
        if (rc != endMarker) {
            // store char if not termination
            commBuffer[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
        }
        else {
            Serial.println("Here");
            commBuffer[ndx] = '\0'; // terminate the string
            ndx = 0;
            newData = true;
        }
    }
}

bool Biosynth::wait_for_ack(const int timeout){
    int current = millis();
    int elapsed = 0;
    Serial1.clear();
    while(elapsed < timeout){
        if(Serial1.available()){
                        Log.traceln("here");
                         char buff[sizeof(confirmCommand)];
                         Serial1.readBytes(buff,sizeof(confirmCommand));
                         Log.infoln("%s",buff);
                         if (strcmp(buff,confirmCommand) == 0){
                            return true;
                         }
                    }
        elapsed = millis() - current;
    }
    Log.warningln("waiting for ack timedout");
    return false;
}

bool::Biosynth::wait_for_syn(const int timeout){
     int current = millis();
     int elapsed = 0;
    while(elapsed < timeout){
        recvWithEndMarker();
        if(newData){
            Serial.println("Here2");
             Log.infoln("%s",commBuffer);
             if(strstr(commBuffer,pingCommand) != NULL){
                Log.infoln("Pinged by slave");
                return true;  
            }
        }

        elapsed = millis() - current;
    }
    return false;
    Log.warningln("waiting for syn timed out");
}

bool Biosynth::wait_for_command(const int timeout, const char* command){
     int current = millis();
     int elapsed = 0;
    while(elapsed < timeout){
        recvWithEndMarker();
        if(newData){
            Serial.println("Here3");
             Log.infoln("%s",commBuffer);
             if(strstr(commBuffer,command) != NULL){
                Log.infoln("command received");
                commBuffer[0] = '\0';
                newData = false;
                return true;  
            }
        }

        elapsed = millis() - current;
    }
    return false;
    Log.warningln("waiting for command timed out");
}

void Biosynth::ping_master(){
    int timeout = 3000;
    bool syn,synack,ack = false;
    while (!linked) {
        send_syn();
        if (wait_for_command(timeout,synackCommand)) {
                synack = true;
                send_ack();
                linked = true;
            }
        }
    Log.infoln("Link susccessfull");
}




void Biosynth::wait_for_slave(){
    int timeout = 3000;
     bool syn,synack,ack = false;
    //Print to lcd waiting for salve
    Log.infoln("Waiting for slave to ping...");
    
    while (!linked) {

        if (wait_for_command(timeout, pingCommand)) {
            syn=true;
            send_synack();
            if (wait_for_command(timeout,confirmCommand)) {
                linked = true;
            }
        }
    }
    Log.infoln("Link susccessfull");
}

void Biosynth::initialize() {
    Log.infoln("Erin Gee's Biosynth");
    Serial1.setTimeout(3000);
    Serial1.begin(115200 );
    set_role();

    if (master) {
        wait_for_slave();
    } else {
        ping_master();
    }

    screen::initialize();
    encoder::initialize();
    button::initialize();
    biosensors::initialize();
    audio_manager::audio_shield_initialization();
    led::initialize();
    loadProject();
    #if LOG
        session_log.initialize();
    #endif

    screen::clear();
}

void Biosynth::loadProject() {
    selected_project = selectProject(5000);
    
    switch (selected_project)  //add new projects to this switch case (just copy paste the case and change the title and class name)
    {
    case SONG_OF_SEVEN:
        project = new SongOfSeven(&biosensors::heart,&biosensors::sc1,&biosensors::resp,&biosensors::sc2);
        break;
    
    case WE_AS_WAVE:
        project = new WeAsWaves(&biosensors::heart,&biosensors::sc1,&biosensors::resp,&biosensors::sc2);
        break;
    }

    Log.infoln("Project loaded: %s",project->getName());
    project ->setup();
    selectedProjectMessage(3000);  //get stuck when trying to update lcd
}


void Biosynth::update(){

    biosensors::update(); 
    data = biosensors::sample_sensors();

    project->updateVolume(updatePotentiometer());
    project->update();
    

    #if PLOT_SENSOR
        plot_sampled_data(data);
    #endif

    #if SEND_OVER_SERIAL
    //maybe wrap this into chrono
        send_over_serial(data,&Serial,16);
    #endif
    #if LOG
        session_log.log_data(data);
    #endif

    current_encoder_value = encoder::update(project->getNumberOfSection());
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

    if(lcdUpdate.hasPassed(40,true)){
        opening_message();
        #if LOG
            start_logging_message(true);
            stop_logging_message(true);
        #endif
    
        #if ADVANCE_WITH_ENCODER
            section_change();
            verify_no_touch();
        #endif
        //sample test = project->getLedProcessed();
        //Serial.println(test.gsr);
        led::update(project->getLedProcessed());
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
        sprintf(screen::buffer_line_2, "I am board #%d",configuration::board_id+1);
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
        project->changeSection(current_encoder_value);
        current_section_message();
        confirmTimer.restart();
        confirmTimer.stop();
    }
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

void Biosynth::section_change()
{  
    
    if(current_encoder_value != current_section )
    {     
        section_confirm_message(current_encoder_value);
        if(!confirmTimer.isRunning()){
            confirmTimer.start();  
        }  
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
    sprintf(screen::buffer_line_1, "%s", project->getSectionTitle(current_encoder_value));
    sprintf(screen::buffer_line_2, "   Confirm ?   ");
    lcd_state = 1;
    screen::update();  
}   

void Biosynth::current_section_message()
{
    sprintf(screen::buffer_line_1, "%s",project->getSectionTitle(current_section));
    sprintf(screen::buffer_line_2, "                ");
    lcd_state = 2;
    screen::update();  
}


void Biosynth::advance_section(){
    current_section++;
    current_section = current_section%project->getNumberOfSection();
    project->changeSection(current_section);
    current_section_message();
};

float Biosynth::updatePotentiometer(){
        float vol = analogRead(pins::audio_shield::volume);
        vol = (vol/1024)*0.8; //make sure the gain doesn't go louder than 0.8 to avoid clipping
        //Log.traceln(vol);
        return vol;
}

ProjectList Biosynth::selectProject(const int &timeout){ //need to be modified if more than two project
    Chrono waitTime;
    waitTime.restart();
    int project = 0;
    while (!waitTime.hasPassed(timeout))
    {
        project = button::getEncoder();
    }

    if (project == 1){ //project selected when button not pressed on boot
     
        return SONG_OF_SEVEN;
    }
    else{ //project selected when button not pressed on boot
        return WE_AS_WAVE;
    }
}

void Biosynth::selectedProjectMessage(const int &displayTime){
    Chrono waitTime;

    sprintf(screen::buffer_line_1, "    Biosynth   ");
    sprintf(screen::buffer_line_2, project->getName());
    screen::update();
    waitTime.restart();
    while (!waitTime.hasPassed(displayTime))
    {
        // wait in function
    }
}


void Biosynth::send_over_serial(sample signals,Print *output, int rate_ms){
        static Chrono wait(true);

        if(wait.hasPassed(rate_ms,true)){
            output->printf("ID %d,H %.2f,G %.2f,R %.2f\n",configuration::board_id,signals.heart.sig,signals.gsr,signals.resp.sig);
        }
        
    }

#if PLOT_SENSOR
    void Biosynth::plot_sampled_data(sample signals){

        Serial.printf("%.2f,%.2f,%.2f",signals.heart.sig,signals.gsr,signals.resp.sig);
        Serial.println();
    }
#endif


void Biosynth::set_role(){

    if (configuration::board_id % 2 == 1 ) master = false;

    if(master){
        Log.infoln("Biosynth acting as a Master");
    }else{
        Log.infoln("Biosynth acting as a Slave");
    }

}


