/**
 * @file Biosynth.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The Biosynth class interface the multipe components of the synths. 
 *        It hanlde the user interraction and the different actions he can do with the board
 * @version 1.1
 * @date 2022-04-02 
 */
#pragma once

#include "configuration.h"
#include "Project_list.h"
#include "pins.h"
#include <Chrono.h>
#if LOG
    #include "Logger.h"
#endif

class Biosynth
{    
    Chrono  confirmTimer{false}; //timer used to reset lcd state if section change not confirmed 
    Chrono  lcdUpdate; //timer to slow down lcd refresh rate
    Project *project{nullptr};
    ProjectList selected_project;
    int current_section{0};
    int last_section{-1};
    int lcd_state = 0;
    int current_encoder_value = 0;

    sample data;


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

    /** @brief check to see if the user confirmed a section change 
     */
    void maybe_confirm_section_change();

    /**
     * @brief Verify if the encoder has not been touched while trying to change piece section.
              If the time set in the configuration has passed it displays the current section on the lcd
     */
    void verify_no_touch();

    /**
     * @brief verify if the user is selecting another piece section
     */
    void section_change();

    /**
     * @brief display a message to the lcd asking the user if he want to change the currently playing piece section to the passed one
     * @param section section to change to 
     */
    void section_confirm_message(const int section);

    /**
     * @brief display the current section of the piece to the lcd screen 
     */
    void current_section_message();

    /**
     * @brief display the welcome message to the lcd screen for the specified time in the configuration
     */
    void opening_message();

#if PLOT_SENSOR
    /**
     * @brief prints out the passed sample to the arduino plotter
     * @param signals sample of data to plot
     */
    void plot_sampled_data(sample signals);
#endif

    /**
     * @brief advance the piece to the next section when called
     */
    void advance_section();

    /**
     * @brief start data logging if user presses the encoder button
     */
    void maybe_start_logging();

    /**
     * @brief stops data logging if user presses the encoder button and a recording is started
     */
    void maybe_stop_logging();

    /**
     * @brief display a message on screen to tell user logging as started
     * 
     * @param do_once pass false to display the message on screen and reset the timer. pass true just to opdate the timer
     */
    void start_logging_message(bool do_once);

    /** 
     * @brief display a message on screen to tell user logging as stopped
     * 
     * @param do_once pass false to display the message on screen and reset the timer. pass true just to opdate the timer
     */
    void stop_logging_message(bool do_once);

    /** @brief load the selected project object at boot 
     */
    void loadProject();

    /**
     * @brief display the loaded project on the screen
     * @param displayTime amount of time to display project name in ms
     */
    void selectedProjectMessage(const int &displayTime);

    /**
     * @brief lets you selects the project to load on boot
     * @param timeout time for project selection in ms
     * @return ProjectList 
     */
    ProjectList selectProject(const int &timeout);
    
    /**
     * @brief return the current value of the potentiometer
     * @return float 
     */
    float updatePotentiometer();


};

