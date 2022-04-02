/**
 * @file Biosynth.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The Biosynth class interface the multipe components of the synths. 
 *        It hanlde the user interraction and the different actions he can do with the board
 * @version 1.1
 * @date 2022-04-02 
 */
#pragma once

#include <configuration.h>
#include <pins.h>
#include <Chrono.h>
#include <Logger.h>

class Biosynth
{    
    Chrono  confirmTimer{false}; //timer used to reset lcd state if section change not confirmed 
    Chrono  lcdUpdate; //timer to slow down lcd refresh rate
    Chrono biosynthSensorTimer; //timer to slow down biosensor update rate
 
    const char sections[configuration::number_of_sections][2] = {"A","B","C","D"}; // section title display on screen
    
    int current_section{0};
    int last_section{-1};
    int lcd_state = 0;
    int current_encoder_value = 0;

    #if LOG
        logger session_log; //logger object only create if specified
    #endif

public:

    Biosynth(){}

    /**
     * @brief runs the initialization routine of all the components of the project
     * 
     */
    void initialize();

    /**
     * @brief runs the update loop of all the components of the project. Most of the action happens here
     * 
     */
    void update();

private:

    /**
     * @brief Verify if the encoder has not been touched while trying to change piece section.
              If the time set in the configuration has passed it displays the current section on the lcd
     */
    void verifyNoTouch();

    /**
     * @brief verify if the user is selecting another piece section
     */
    void sectionChange();

    /**
     * @brief display a message to the lcd asking the user if he want to change the currently playing piece section to the passed one
     * @param section section to change to 
     */
    void sectionConfirmMessage(const int section);

    /**
     * @brief display the current section of the piece to the lcd screen 
     */
    void currentSectionMessage();

    /**
     * @brief display the welcome message to the lcd screen for the specified time in the configuration
     */
    void openingMessage();

#if PLOT_SENSOR
    /**
     * @brief prints out the passed sample to the arduino plotter
     * @param signals sample of data to plot
     */
    void plot_sampled_data(sample signals);
#endif

};
