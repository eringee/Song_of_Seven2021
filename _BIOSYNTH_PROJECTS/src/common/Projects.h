/**
 * @file Projects.h
 * @author Etienne Montenegro
 * @brief  Project base class. Contains all the common parts of a biosynth project. 
 *         DO NOT MODIFY UNLESS THER IN NOT ENOUGH AUDIO CONNECTION IN A NEW PROJECT
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "configuration.h"
#include "biosensors.h"
#include <Arduino.h>
#include <Audio.h>

static AudioConnection *patch_cords[50];

class Project{
    const char* name{"project"};
    static const int number_of_boards{7};
    static const int number_of_sections{4};
    int current_section{0};
    const char sections_title[number_of_sections][16] = {"      A       ", "       B       ", "       C       ", "       D       "};

    Heart *heart;
    SkinConductance *sc1;
    Respiration *resp;
    SkinConductance *sc2;

    sample processed_for_leds{};

    public:

    Project(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
    {};


    virtual void setup(){};
    virtual void update(){};

    virtual const char* getName();//{return name;};
    virtual const int getNumberOfSection();//{return number_of_sections;};
    virtual const char* getSectionTitle(const int section_index){return sections_title[section_index];};
    virtual void changeSection(const int currentSection){};
    virtual void updateVolume(float vol);//{};

    virtual sample getLedProcessed(){return processed_for_leds;};
};
