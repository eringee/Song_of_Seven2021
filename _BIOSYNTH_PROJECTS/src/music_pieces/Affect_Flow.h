

/**
 * @file Affect_Flow.h
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
//include library needed for the project here
#include <mtof.h>

//create a class that inherit Projet class and modify ist member for the project
class AffectFlow :public Project{
    
    private:
    const char* name{"  AFFECT FLOW"};
    static const int number_of_boards{10};
    static const int number_of_sections{3};
    int current_section{0};

    Heart *heart;
    SkinConductance *sc1;
    Respiration *resp;
    SkinConductance *sc2;

    sample processed_for_leds{};

/*// GUItool: begin automatically generated code..This code is here for reference.
AudioSynthWaveform       heartLFO;       //xy=60,109
AudioSynthNoiseWhite     respnoise;      //xy=160,489
AudioSynthWaveform       respnoiseLFO;   //xy=160,577
AudioEffectBitcrusher    bitcrusher1;    //xy=181,186
AudioEffectEnvelope      clickEnvelope;  //xy=273,120
AudioSynthWaveform       heartTone;      //xy=275,264
AudioSynthWaveform       respWave1;      //xy=355,386
AudioEffectMultiply      multiply1;      //xy=355,513
AudioSynthWaveform       respWave2;      //xy=356,430
AudioAmplifier           heartEnvAmp;    //xy=448,204
AudioEffectEnvelope      heartEnvAmp2;      //xy=506,279
AudioMixer4              respMixer;      //xy=547,419
AudioMixer4              clickGlitchMixer; //xy=583,90
AudioFilterStateVariable GSRfilter1; //xy=688,248
AudioFilterStateVariable respFilter;     //xy=708,477
AudioFilterStateVariable GSRfilter;      //xy=752,179
AudioAmplifier           respAmp2;       //xy=784,396
AudioMixer4              mainMixer;      //xy=874,317
AudioOutputI2S           AudioOut;       //xy=1031,323
AudioConnection          patchCord1(heartLFO, bitcrusher1);
AudioConnection          patchCord2(respnoise, 0, multiply1, 0);
AudioConnection          patchCord3(respnoiseLFO, 0, multiply1, 1);
AudioConnection          patchCord4(bitcrusher1, clickEnvelope);
AudioConnection          patchCord5(clickEnvelope, heartEnvAmp);
AudioConnection          patchCord6(heartTone, heartEnvAmp2);
AudioConnection          patchCord7(respWave1, 0, respMixer, 0);
AudioConnection          patchCord8(multiply1, 0, respMixer, 2);
AudioConnection          patchCord9(respWave2, 0, respMixer, 1);
AudioConnection          patchCord10(heartEnvAmp, 0, clickGlitchMixer, 1);
AudioConnection          patchCord11(heartEnvAmp2, 0, GSRfilter1, 0);
AudioConnection          patchCord12(respMixer, 0, respFilter, 0);
AudioConnection          patchCord13(respMixer, 0, mainMixer, 1);
AudioConnection          patchCord14(clickGlitchMixer, 0, GSRfilter, 0);
AudioConnection          patchCord15(GSRfilter1, 1, mainMixer, 2);
AudioConnection          patchCord16(respFilter, 1, respAmp2, 0);
AudioConnection          patchCord17(GSRfilter, 1, mainMixer, 0);
AudioConnection          patchCord18(respAmp2, 0, mainMixer, 3);
AudioConnection          patchCord19(mainMixer, 0, AudioOut, 0);
AudioConnection          patchCord20(mainMixer, 0, AudioOut, 1);
// GUItool: end automatically generated code


////////////////////////////AUDIO CODE/////////////////////////////////////
    //ADD audio objects here. No need to add the sgt5000 object
  
*/

AudioSynthWaveform       heartLFO;       //xy=60,109
AudioSynthNoiseWhite     respnoise;      //xy=160,489
AudioSynthWaveform       respnoiseLFO;   //xy=160,577
AudioEffectBitcrusher    bitcrusher1;    //xy=181,186
AudioEffectEnvelope      clickEnvelope;  //xy=273,120
AudioSynthWaveform       heartTone;      //xy=275,264
AudioSynthWaveform       respWave1;      //xy=355,386
AudioEffectMultiply      multiply1;      //xy=355,513
AudioSynthWaveform       respWave2;      //xy=356,430
AudioAmplifier           heartEnvAmp;    //xy=448,204
AudioAmplifier           heartEnvAmp2;      //xy=506,279
AudioMixer4              respMixer;      //xy=547,419
AudioMixer4              clickGlitchMixer; //xy=583,90
AudioFilterStateVariable GSRfilter1; //xy=688,248
AudioFilterStateVariable respFilter;     //xy=708,477
AudioFilterStateVariable GSRfilter;      //xy=752,179
AudioAmplifier           respAmp2;       //xy=784,396
AudioMixer4              mainMixer;      //xy=874,317
AudioOutputI2S           AudioOut;       //xy=1031,323

//vvv ADD ALL THE VARIABLES YOUR PROJECT DEPENDS ON IN THIS SECTION vvvv

  /*const char sections_title[number_of_sections][17] = {"       A       ", "       B       ", "       C       ", "       D       "};  //menu display 17 chars
      
  double sectionGlobal[number_of_sections][number_of_boards] = {
    
    {mtof.toFrequency(50), 
    mtof.toFrequency(57), 
    mtof.toFrequency(62), 
    mtof.toFrequency(71), 
    mtof.toFrequency(72)}, // Intro section A  

    {mtof.toFrequency(69), 
    mtof.toFrequency(76), 
    mtof.toFrequency(83), 
    mtof.toFrequency(84), 
    mtof.toFrequency(91)},  // Intro section B
      
    {mtof.toFrequency(69), 
    mtof.toFrequency(72), 
    mtof.toFrequency(74), 
    mtof.toFrequency(79), 
    mtof.toFrequency(86)},  // intro section C plus song
          
    {mtof.toFrequency(46),   
    mtof.toFrequency(48), 
    mtof.toFrequency(51), 
    mtof.toFrequency(54), 
    mtof.toFrequency(56)},  // Anger section D

    {mtof.toFrequency(),   
    mtof.toFrequency(), 
    mtof.toFrequency(), 
    mtof.toFrequency(), 
    mtof.toFrequency()},  // Ending section E
  };
*/

    //vvv ADD ALL THE VARIABLES YOUR PROJECT DEPENDS ON IN THIS SECTION vvvv

    //!!!!! section title should be no more than 16 characters long. Longer strings will make the teensy crash!!!!!
    const char sections_title[number_of_sections][17] = {" Section 1 CALM ", "Section 2 ANGER", " Section 3 END  "};
    double sectionGlobal[number_of_sections][number_of_boards] = {
    
    //SECTION A - INTRO/CALM
    {
    mtof.toFrequency(100), //fake value to get the math right
    mtof.toFrequency(50),  //D3
    mtof.toFrequency(57),   //A3
    mtof.toFrequency(62),   //D4
    mtof.toFrequency(71),   //B4
    mtof.toFrequency(72)},  //C5

    //SECTION B - ANGER
    {mtof.toFrequency(100), //fake value to get the math right
    mtof.toFrequency(44),  // Bb2
    mtof.toFrequency(48),   // C3
    mtof.toFrequency(51),   // Eb3
    mtof.toFrequency(54),   //Gb3
    mtof.toFrequency(56)},  //Ab3

    //SECTION C - ENDING/SONG
    {mtof.toFrequency(100), //fake value to get the math right
    mtof.toFrequency(55),   
    mtof.toFrequency(69), 
    mtof.toFrequency(74), 
    mtof.toFrequency(79), 
    mtof.toFrequency(81)},  

    };

  
    //Variables for smoothing out biosignals
    float respTone;

    float smoothHeart = 0.5; //default value for smoothing heart signal for EMA
    float smoothHeartAmp = 0.5; //default value for smoothing heart signal for EMA
    float smoothHeartBPM = 0.5; //default value for smoothing heart signal for EMA

    float smoothGSR = 0.07;   //default value for smoothing sc1 signal for EMA

    float smoothRespLED = 0.5;  //default value for smoothing resp signal
    float smoothResp = 0.5;  //default value for smoothing resp signal

    float smoothRespAmp = 0.5;  //default value for smoothing resp signal
    float smoothRespBPM = 0.5;  //default value for smoothing resp signal

    float finalResp = 0.1;

    //variables for storing last sensor reading
    float heartSig;
    float heartAmp;
    float heartBPM;  
    float GSRsig;
    float GSRlowpass;
    float respSig;
    float respAmp;
    float respBPM;
    float smoothGSRreduced;

    int GSRfilt; // value for storing filter data relative to GSR
    float respBPMfun;

    void createPatchCords(){
        /* This is mandatory for dynamic project loading. Redo all the patchcord connections provided by the 
            Teensy audio tool this way. You can modify the maximum number of patchcord in projects.h if you need 
            more 
        */
        int pci= 0;


        patch_cords[pci++] = new AudioConnection(heartLFO, bitcrusher1);
        patch_cords[pci++] = new AudioConnection(respnoise, 0, multiply1, 0);
        patch_cords[pci++] = new AudioConnection(respnoiseLFO, 0, multiply1, 1);
        patch_cords[pci++] = new AudioConnection(bitcrusher1, clickEnvelope);
        patch_cords[pci++] = new AudioConnection(clickEnvelope, heartEnvAmp);
        patch_cords[pci++] = new AudioConnection(heartTone, heartEnvAmp2);
        patch_cords[pci++] = new AudioConnection(respWave1, 0, respMixer, 0);
        patch_cords[pci++] = new AudioConnection(multiply1, 0, respMixer, 2);
        patch_cords[pci++] = new AudioConnection(respWave2, 0, respMixer, 1);
        patch_cords[pci++] = new AudioConnection(heartEnvAmp, 0, clickGlitchMixer, 1);
        patch_cords[pci++] = new AudioConnection(heartEnvAmp2, 0, GSRfilter1, 0);
        patch_cords[pci++] = new AudioConnection(respMixer, 0, respFilter, 0);
        patch_cords[pci++] = new AudioConnection(respMixer, 0, mainMixer, 1);
        patch_cords[pci++] = new AudioConnection(clickGlitchMixer, 0, GSRfilter, 0);
        patch_cords[pci++] = new AudioConnection(GSRfilter1, 0, mainMixer, 2);
        patch_cords[pci++] = new AudioConnection(respFilter, 1, respAmp2, 0);
        patch_cords[pci++] = new AudioConnection(GSRfilter, 2, mainMixer, 0);
        patch_cords[pci++] = new AudioConnection(respAmp2, 0, mainMixer, 3);
        patch_cords[pci++] = new AudioConnection(mainMixer, 0, AudioOut, 0);
        patch_cords[pci++] = new AudioConnection(mainMixer, 0, AudioOut, 1);
    }

    void setupSounds(){ //SETUP THE INITIAL SOUNDS IN THE PROJECT HERE
        Serial.println("Setup sounds");
        //RESP dependent variables
        respTone = (sectionGlobal[0][configuration::board_id]); 
        respWave1.begin(0.3, respTone, WAVEFORM_SINE);
        respWave2.begin(0.15, respTone, WAVEFORM_TRIANGLE);

        // set bandpass around the respTone frequencies
        respFilter.frequency(respTone); 
        respFilter.resonance(1.0); 

        respnoise.amplitude(0.1);
        respnoiseLFO.begin(0.3, 15, WAVEFORM_SINE); // LFO applied to respNoise

        clickEnvelope.attack(10);
        clickEnvelope.decay(10);
        clickEnvelope.sustain(10);
        clickEnvelope.release(50);

        heartLFO.begin(0.01, 5, WAVEFORM_SINE);

        clickGlitchMixer.gain(1, 5.0);  // the global volume of the clicks.
       // clickGlitchMixer.gain(1, 0.0);  // turn off the clicksx


        bitcrusher1.bits(3);  // creates the glitch-click effect
        bitcrusher1.sampleRate(44000);  // doesn't seem to affect the sound either way
    }

    public:

    AffectFlow(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
    {};

    void setup() override {
        AudioMemory(80);  //MODIFY AUDIO MEMORY HERE
        createPatchCords();
        setupSounds();
    }

    //Project update loop. Access the  biosensors from here, process the data and modify audio objects
    //The biosensor need to be accessed with arrow "->" instead of dots "." because we are dealing with pointers and not objects
    void update() override{
        //Retrieve sensor values
        heartSig = heart->getNormalized();
        heartAmp = heart->amplitudeChange();
        heartBPM = heart->bpmChange();
        
        GSRsig = sc1->getSCR();
        
        respSig = resp->getScaled();
        respAmp = resp->getScaledAmplitude();
        respBPM = resp->getScaledRpm();

         ////////////////////////smooth signals       
        smoothHeart += 0.005 * (heartSig - smoothHeart);
        
        smoothGSR += 0.8 * (GSRsig - smoothGSR); // run EMA filter
        smoothGSRreduced = max((smoothGSR-0.1), 0.0);  //subtract a bit from GSR value but don't pass 0  
    
        smoothResp += 0.0005  * (respSig - smoothResp);

        finalResp = max((respSig), 0); 
        
        // Smoothing for the LEDs
        smoothRespLED += 0.03 * (respSig - smoothRespLED);   
      
        //////AUDIO TRANSFORMATIONS////////////////////////////////////////////////

        //RESPIRATION///////////////////////////////
    
        //use breaths per minute to detune fundamental - if breathing is regular it will not be affected
        respBPMfun = (respBPM*(2*configuration::board_id))-0.5; 
        respWave2.frequency(respBPMfun+respTone); //detuned wave mixed with fundamental
        
        respnoiseLFO.frequency(respAmp*100); //gently modulate respNoise through the amplitude of the breath
        
        respAmp2.gain(smoothResp*0.75); //secondary resptone comes in through lowpass filter    
        respFilter.frequency(respTone*(respAmp)); // moving bandpass around the secondary respTone    
    
        //set the gain of respiration tones in accordance with respiration signal
        for (int x=0; x <=3; x++) {
        respMixer.gain(x, respSig); //linked to resp signal and a little quieter
        if(x==2) respMixer.gain(x, respSig*0.1); //linked to a lowpassed resp signal and even quieter
        }

        //////////HEART////////////////////////
        if (heart->beatDetected()) clickEnvelope.noteOn(); //activate the env but never turn it off: it creates useful clicks     
        
        heartLFO.frequency(heartBPM*8);  //the frequency of the heartsignal LFO
        heartEnvAmp.gain(0.1+(smoothHeart/2)); // clicks follow the envelope of heart signal 

        heartTone.begin(0.005, respTone*2, WAVEFORM_SQUARE);
        heartEnvAmp2.gain(0.1+(smoothHeart/2.5)); // clicks follow the envelope of heart signal 

        ////GSR//////////////////////////////////           
        // map GSR signal to a bandpass filter frequency for filtering heartclicks
        GSRfilt = GSRsig*100; //convert Gsrsig into a value from 1-100
        int y = map(GSRfilt, 1, 100, 2000, 50);
        int y2 = map(GSRfilt, 1, 100, 5000, 50);
        
        GSRfilter.frequency(y);
        GSRfilter1.frequency(y2);

        int y3 = map(GSRfilt, 1, 100, 0, 700);
        float y4 = y3 / 100.0;

        clickGlitchMixer.gain(1, y4);
       /*
      //section to silence GSR pops 
       GSRfilter.frequency(0);
       GSRfilter1.frequency(0);
      */

        processed_for_leds.heart.sig =  biosensors::heart.getNormalized();
        processed_for_leds.gsr.scr = biosensors::sc1.getSCR();
        processed_for_leds.resp.sig = smoothRespLED;

    };

//Project update volume loop. Modify here if project needs special volume clamping
    void updateVolume(float vol) override{
        for( int i = 0 ; i < 4 ; i++ )
        {    
            
            if (i == 0){
                mainMixer.gain(i, vol*smoothGSRreduced*1.2); //clamp down heartbeat volume a bit when GSR is low 
            } else if(i == 3) {
             mainMixer.gain(i, vol/8);  //clamp down on these more distorted sounds from respiration
            } else {
             mainMixer.gain(i, vol);  //set all four channels of main mixer to follow gain knob
            }
        } 
    }

//This is where you set the sound difference for every sections

void changeSection(const int currentSection) override //this is where we change sections AND frequencies...
{
 //Serial.println(currentSection);   
 if (currentSection==0){
    respTone = (sectionGlobal[0][configuration::board_id]);      
    respWave1.begin(0.3, respTone, WAVEFORM_SINE);
    respWave2.begin(0.15, respTone, WAVEFORM_TRIANGLE);
    respFilter.frequency(respTone);  
        }
else if (currentSection==1){
    respTone = (sectionGlobal[1][configuration::board_id]);
    respWave1.begin(0.3, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);
    respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);
    respFilter.frequency(respTone);    
}
else if (currentSection==2){
    respTone = (sectionGlobal[2][configuration::board_id]);
    respWave1.begin(0.3, respTone, WAVEFORM_SINE);
    respWave2.begin(0.2, respTone, WAVEFORM_SINE);
    respFilter.frequency(respTone);    
    }
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
