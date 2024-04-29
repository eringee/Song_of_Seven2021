/**
 * @file Song_of_seven.h
 * @author Erin Gee & Etienne Montenegro
 * @brief SoS project class. USE THIS FILE AS TEMPLATE FOR OTHER PROJECTS. DO NOT FORGET TO OVERRIDE ALL THE Projects.h methods
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
//include library needed for the project here


//create a class that inherit Projet class and modify ist member for the project
class SongOfSeven :public Project{
    
    private:

    const char* name{"  Song of Seven"};
    static const int number_of_boards{7};
    static const int number_of_sections{4};
    const char sections_title[number_of_sections][2] = {"A", "B", "C", "D"};
    int current_section{0};

    Heart *heart;
    SkinConductance *sc1;
    Respiration *resp;
    SkinConductance *sc2;

    sample processed_for_leds{};
    char section_message[17];

    //ADD audio objects here. No need to add the sgt5000 object
    AudioSynthWaveform       waveform3;      //xy=1014,743
    AudioSynthWaveform       waveform2;      //xy=1070,543
    AudioSynthWaveformSineModulated sine_fm3;       //xy=1227,547
    AudioSynthWaveformSine   sine1;          //xy=1231,587
    AudioAmplifier           amp2;           //xy=1246,728
    AudioSynthWaveformSine   sine_fm2;       //xy=1283,411
    AudioSynthWaveformSineModulated sine_fm4;       //xy=1392,650
    AudioAmplifier           amp1;           //xy=1414,361
    AudioMixer4              mixerMain;      //xy=1553,454
    AudioOutputI2S           AudioOut;       //xy=1712,455

    //vvvADD ALL THE VARIABLE YOUR PROJECT DEPENDS ON UNDER HEREvvvv

    int section_frequency[number_of_sections][number_of_boards] {
        {622,933,1244,1555,1866,2177,2488},  // going to canada
        {174,348,1244,1555,1566,1740,2488},  // cat died
        {392,416,448,587,659,1046,1174},     // fluffy bunny
        {1148,1312,1476,1640,1804,1968,2132} // board games
    };


    void createPatchCords(){
        /* This is mandatory for dynamic project loading. Redo all the patchcord connections provided by the 
            Teensy audio tool this way. You can modify the maximum number of patchcord in projects.h if you need 
            more than 20
        */
        int pci= 0;
        patch_cords[pci++] = new AudioConnection(waveform3, amp2);
        patch_cords[pci++] = new AudioConnection(waveform2, sine_fm3);
        patch_cords[pci++] = new AudioConnection(sine_fm3, 0, mixerMain, 1);
        patch_cords[pci++] = new AudioConnection(sine1, 0, mixerMain, 2);
        patch_cords[pci++] = new AudioConnection(amp2, sine_fm4);
        patch_cords[pci++] = new AudioConnection(sine_fm2, amp1);
        patch_cords[pci++] = new AudioConnection(sine_fm4, 0, mixerMain, 3);
        patch_cords[pci++] = new AudioConnection(amp1, 0, mixerMain, 0);
        patch_cords[pci++] = new AudioConnection(mixerMain, 0, AudioOut, 0);
        patch_cords[pci++] = new AudioConnection(mixerMain, 0, AudioOut, 1);
    }

    void setupSounds(){ //SETUP THE INITIAL SOUNDS IN THE PROJECT HERE
        
        //GSR dependent variables
        sine_fm2.frequency(section_frequency[0][configuration::board_id]); 
        sine_fm2.amplitude(0.5);
        
        //HEART dependent variables
        waveform3.begin(0.01, 0.005, WAVEFORM_SINE);
        sine_fm4.frequency(622);
        sine_fm4.amplitude(0.02);

        //ATMOSPHERIC SINES
        waveform2.begin(0.004, 1, WAVEFORM_SINE);
        sine_fm3.frequency(311);           //atmospheric sines
        sine1.frequency(424);
        sine_fm3.amplitude(0.1);
        sine1.amplitude(0.05);

        //mute the mixers before updating to avoid big volume change
        for( int i = 0 ; i < 4 ; i++ )
        {    
            mixerMain.gain(i, 1);  //set all four channels of main mixer to follow gain knob
        }
    }

    public:

    SongOfSeven(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
    {};

    void setup() override {
        AudioMemory(24); //MODIFY AUDIO MEMORY HERE
        createPatchCords();
        setupSounds();
    }

    //Project update loop. Access the  biosensors from here, process the data and modify audio objects
    //The biosensor need to be accessed with arrow "->" instead of dots "." because we are dealing with pointers and not objects
    void update() override{   
     
        static float smooth_heart = 0.5; //default value for smoothing out heart signal for EMA
        static float smooth_gsr = 0.5;   //default value for smoothing out sc1 signal for EMA
        //static float smooth_respiration = 0.5;  //default value for smoothing out resp signal for EMA
    
        //retrieve signals   
        // sample signals{};
        // signals.heart.sig = heart->getNormalized();
        // signals.gsr.scr = sc1->getSCR(); 
        // signals.resp.sig = resp->getNormalized();
       
        //smooth signals
        smooth_heart += 0.1 * (heart->getNormalized() - smooth_heart);
        smooth_gsr += 0.5 * (sc1->getSCR() - smooth_gsr);
        
        
        //smooth_respiration += 0.5 * (resp->getNormalized() - smooth_respiration); // I JUST COPIED THE GSR SMOOTH FUNCTION  PLEASE MODIFY
        /*in an older version of SoS the line is : 
            smooth_respiration += 0.5 * (signals.gsr - smooth_respiration);

          is it normal that it uses a gsr signal rather than the resp one?
        */

        amp1.gain(smooth_gsr); //
        amp2.gain(smooth_heart);


        //vvvTO UPDATE LED COLORS CHANGE THE right and side of these. Make it equal to 0 if you're not using the LEDSvvv
        processed_for_leds.heart.sig = smooth_heart;
        processed_for_leds.gsr.scr = smooth_gsr;
        processed_for_leds.resp.sig = 0;    
   
   
    };

//Project update volume loop. Modify here if project needs special volume clamping
void updateVolume(float vol) override{

    for( int i = 0 ; i < 4 ; i++ )
    {    
        mixerMain.gain(i, vol);  //set all four channels of main mixer to follow gain knob
    } 
}

//This is where you set the sound difference for every sections
void changeSection(const int currentSection) override
{
    
        /// THIS IS WHERE YOU NEED TO UPDATE THE FREQUENCY VALUES
        sine_fm2.frequency(section_frequency[currentSection][configuration::board_id]); //removed -1 (Etienne)
        if (currentSection==0){
          sine_fm3.frequency(311);   //atmospheric sine1
          sine1.frequency(424);      //atmospheric sine2
        }
        else if (currentSection==1){
          sine_fm3.frequency(311);   //atmospheric sine1
          sine1.frequency(369);      //atmospheric sine2
        }
        else if (currentSection==2){
          sine_fm3.frequency(261.63);   //atmospheric sine1
          sine1.frequency(329.63);      //atmospheric sine2
          sine_fm3.amplitude(0.1);  
          sine1.amplitude(0.1);
        }
        else if (currentSection==3){
          waveform2.frequency(1);
          sine_fm3.frequency(659);   //atmospheric sine1
          sine1.frequency(985);      //atmospheric sine2
          sine_fm3.amplitude(0.1);  
          sine1.amplitude(0.1);
        }
        
        
}

const char* getName() override { //Do not modify, just copy paste to new project
    return name;
};

const int getNumberOfSection() override { //Do not modify, just copy paste to new project
    return number_of_sections;
}

const char* getSectionTitle(const int section_index) override { 
    sprintf(section_message, "  Section  %s    ",sections_title[section_index]);
    return  section_message;
}

sample getLedProcessed() override{ //Do not modify, just copy paste to new project
    return processed_for_leds;
}
};
