/**
 * @file Recorder.h
 * @author Erin Gee & Etienne Montenegro
 * @brief WaW project class. USE THIS FILE AS TEMPLATE FOR OTHER PROJECTS. DO NOT FORGET TO OVERRIDE ALL THE Projects.h methods
 * @version 1.0
 * @date 2022-11-02
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "configuration.h"
#include "Projects.h"
#include <Arduino.h>
#include <Audio.h>
#include <Chrono.h>
#include "lcd.h"
#include "led.h"
#include "buttons.h"
//include library needed for the project here


//create a class that inherit Projet class and modify ist member for the project
class Recorder :public Project{
    
    private:

    const char* name{"  RECORDER"};
    static const int number_of_boards{1};
    static const int number_of_sections{20};
    int current_section{0};

    Heart *heart;
    SkinConductance *sc1;
    Respiration *resp;
    SkinConductance *sc2;

    sample processed_for_leds{};

    //vvv ADD ALL THE VARIABLES YOUR PROJECT DEPENDS ON IN THIS SECTION vvvv
    //!!!!! section title should be no more than 16 characters long. Longer strings will make the teensy crash!!!!!
    const char sections_title[number_of_sections][17] = {" Neutral        ", " Relaxation    ", " Concentration ", " Confidence     ", " Trust          ", " Gratitude      "," Joy            ", " Surprise       ", " Insecurity     ", " Anxiety        ", " Fear           ", " Disgust        ", " Anger          ", " Shame          ", " Pain           ", " Despair        ", " Sadness        ", " Tired          ", " Laughter       ", " Arousal        "};

    public:

    Recorder(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
    {};

    void setup() override {
        Serial.println("Recorder set up");
    }

    //Project update loop. Access the  biosensors from here, process the data and modify audio objects
    //The biosensor need to be accessed with arrow "->" instead of dots "." because we are dealing with pointers and not objects
    void update() override{
       //LEDs
        processed_for_leds.heart.sig =  (map(heart->getRaw(), 0, 1023, 0, 1000))/100;
        processed_for_leds.gsr.scr = (map(sc1->getRaw(), 0, 1023, 0, 1000))/100;
       // processed_for_leds.resp.sig = resp->getRaw();
    };

//Project update volume loop. Modify here if project needs special volume clamping
    void updateVolume(float vol) override{

    }

//This is where you set the sound difference for every sections

void changeSection(const int currentSection) override //this is where we change sections AND frequencies...
{
 Serial.println(currentSection);
}

const char* getName() override { //Do not modify, just copy paste to new project
    return name;
};

const int getNumberOfSection() override { //Do not modify, just copy paste to new project
    return number_of_sections;
}

const char* getSectionTitle(const int section_index) override { //Do not modify, just copy paste to new project
    return sections_title[section_index];
}

sample getLedProcessed() override{ //Do not modify, just copy paste to new project
    return processed_for_leds;
}
};
