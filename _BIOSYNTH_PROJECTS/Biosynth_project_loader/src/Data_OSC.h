

/**
 * @file Data_OSC.h
 * @author Erin Gee & Etienne Montenegro & Luana Belinsky
 * @brief WaW project class. USE THIS FILE AS TEMPLATE FOR OTHER PROJECTS. DO NOT FORGET TO OVERRIDE ALL THE Projects.h methods
 * @version 1.0
 * @date 2025-01-30
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

//  #define DEBUG_PRINT

#include "configuration.h"
#include "Projects.h"
#include <Arduino.h>
#include <Audio.h>
#include <Chrono.h>
//include library needed for the project here
#include "Packets.h"

static Chrono sendMetro;

//create a class that inherit Projet class and modify ist member for the project
class DataOSC :public Project{
    
    private:
    const char* name{"  DATA OSC"};
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

    DataOSC(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
    {};

    void setup() override {
        // Initialize Serial port
        Serial3.begin(115200, SERIAL_8N1);
    }

    //Project update loop. Access the  biosensors from here, process the data and modify audio objects
    //The biosensor need to be accessed with arrow "->" instead of dots "." because we are dealing with pointers and not objects
    void update() override{
        if(sendMetro.hasPassed(10)){
            fillPacket(heart);
            fillPacket(sc1);
            fillPacket(resp);
            sendPackets();
            sendMetro.restart();
        }

        processed_for_leds.heart.sig =  biosensors::heart.getNormalized();
        processed_for_leds.gsr.scr = biosensors::sc1.getSCR();
        processed_for_leds.resp.sig = biosensors::resp.getScaled();

    };

    void changeSection(const int currentSection) override //this is where we change sections AND frequencies...
    {
    debugPrint("Current section: %d", currentSection);
    }

//This is where you set the sound difference for every sections

    void updateVolume(float vol) override{
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
