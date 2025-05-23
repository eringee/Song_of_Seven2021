/**
 * @file Biosynth.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The Biosynth class interface the multipe components of the synths. 
 *        It hanlde the user interraction and the different actions he can do with the board
 * @version 1.1
 * @date 2022-04-02 
 */
#ifndef BIOSYNTH_H
#define BIOSYNTH_H
#pragma once

#include <Arduino.h>

#include "configuration.h"
#include "Project_list.h"
#include "pins.h"
#include <Chrono.h>
#if LOG
#include "Logger.h"
#endif

class Biosynth
{   
    // IntervalTimer updateTimer;
    Chrono  confirmTimer{false}; //timer used to reset lcd state if section change not confirmed 
    Chrono openingtimer;
    Chrono idleTimer;
    Chrono endLogging{false};
    Chrono nowLogging{false};
    Project *project{nullptr};
    ProjectList selected_project{NOMUSIC};

    int current_section{0};
    int last_section{-1};
    int current_encoder_value = 0;

    float vol = 0.0;

    #ifdef LOG
    logger session_log; //logger object only create if specified
    #endif

public:

    Biosynth(){}

    /**
     * @brief runs the initialization routine of all the components of the project
     * 
     */
    void initialize();


    /** @brief load the selected project object at boot 
     */
    void loadProject();

    /**
     * @brief runs the update loop of all the components of the project. Most of the action happens here
     * 
     */
    void update();

private:

    /**
     * @brief updates the lcd screen
     * 
     */
    void updateLCD();

    /**
     * @brief return the current value of the potentiometer
     * @return float 
     */
    float updatePotentiometer();

    /**
     * @brief start data logging if user presses the encoder button
     */
    void handle_logging();

    /**
     * @brief advance the current section if the encoder button is turned
     */
    void advanceWithEncoder();

    /**
     * @brief advance the current section if the foot pedal is pressed
     */
    void advanceWithPedal();

    /**
     * @brief sends the biosensor data over serial
     * 
     * @param output 
     */
    void send_over_serial( Print *output);

    /**
     * @brief sends the biosensor data over serial to ESP32
     * 
     */
    void send_to_ESP32();

};

#endif
