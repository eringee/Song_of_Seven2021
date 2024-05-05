/**
 * @file We_As_Waves.h
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
//include library needed for the project here
#include <mtof.h>

//create a class that inherit Projet class and modify ist member for the project
class WeAsWaves :public Project{
    
    private:

    const char* name{"  AFFECT FLOW"};
    static const int number_of_boards{10};
    static const int number_of_sections{4};
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

    const char sections_title[number_of_sections][17] = {"       A       ", "       B       ", "       C       ", "       D       "};
    double sectionGlobal[number_of_sections][number_of_boards] = {
        {mtof.toFrequency(67), 
        mtof.toFrequency(71), 
        mtof.toFrequency(72), 
        mtof.toFrequency(74), 
        mtof.toFrequency(76), 
        mtof.toFrequency(77), 
        mtof.toFrequency(81), 
        mtof.toFrequency(83), 
        mtof.toFrequency(86), 
        mtof.toFrequency(88)},  // Gmaj13

        {mtof.toFrequency(51), 
        mtof.toFrequency(54), 
        mtof.toFrequency(56), 
        mtof.toFrequency(58), 
        mtof.toFrequency(61), 
        mtof.toFrequency(63), 
        mtof.toFrequency(66), 
        mtof.toFrequency(68), 
        mtof.toFrequency(70), 
        mtof.toFrequency(73)},  // pentatonic scale - add 12 for fun
    

    
        {392,416,448,587,659,1046,1174, 1300, 1400, 1700},     // section 3
        {1148,1312,1476,1640,1804,1968,2132, 2500, 2600, 2700} // section 4
    };

    float respTone;
    ///Variables for smoothing out biosignals
    float smoothHeart = 0.5; //default value for smoothing heart signal for EMA
    float smoothHeartAmp = 0.5; //default value for smoothing heart signal for EMA
    float smoothHeartBPM = 0.5; //default value for smoothing heart signal for EMA

    float smoothGSR = 0.1;   //default value for smoothing sc1 signal for EMA

    float smoothResp = 0.5;  //default value for smoothing resp signal
    float smoothResp2 = 0.5;  //default value for smoothing resp signal
    float smoothResp3 = 0.5;  //default value for smoothing resp signal
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
        patch_cords[pci++] = new AudioConnection(GSRfilter, 1, mainMixer, 0);
        patch_cords[pci++] = new AudioConnection(respAmp2, 0, mainMixer, 3);
        patch_cords[pci++] = new AudioConnection(mainMixer, 0, AudioOut, 0);
        patch_cords[pci++] = new AudioConnection(mainMixer, 0, AudioOut, 1);
    }

    void setupSounds(){ //SETUP THE INITIAL SOUNDS IN THE PROJECT HERE

        //RESP dependent variables
        respTone = (sectionGlobal[0][configuration::board_id]);
        respWave1.begin(0.1 , respTone, WAVEFORM_SINE);
        respWave2.begin(0.05, respTone, WAVEFORM_SAWTOOTH);  

        // set bandpass around the respTone frequencies
        respFilter.frequency(respTone); 
        // respFilter.resonance(1.0); 

        respnoise.amplitude(0.1);
        respnoiseLFO.begin(0.3, 15, WAVEFORM_SINE); // LFO applied to respNoise

        clickEnvelope.attack(10);
        clickEnvelope.decay(10);
        clickEnvelope.sustain(10);
        clickEnvelope.release(50);

        heartLFO.begin(0.05, 5, WAVEFORM_SINE);

        clickGlitchMixer.gain(1, 9.8);  // parameter to control the global volume of the clicks.

        bitcrusher1.bits(3);  // creates the glitch-click effect
        bitcrusher1.sampleRate(44000);  // doesn't seem to affect the sound either way
    }

    public:

    WeAsWaves(Heart *_heart = nullptr,SkinConductance *_sc1 = nullptr, Respiration *_resp = nullptr,SkinConductance *_sc2 = nullptr): heart{_heart},sc1{_sc1},resp{_resp},sc2{_sc2}
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
        
        respSig = resp->getNormalized();
        respAmp = resp->amplitudeChange();
        respBPM = resp->bpmChange();

         ////////////////////////smooth signals       
        smoothHeart += 0.005 * (heartSig - smoothHeart);
        
        smoothGSR += 0.8 * (GSRsig - smoothGSR); // run EMA filter
        smoothGSRreduced = max((smoothGSR-0.1), 0.0);  //subtract a bit from GSR value but don't pass 0  
    
        smoothResp += 0.0005  * (respSig - smoothResp);
        smoothResp2 += 0.0001 * (respSig - smoothResp2);
        smoothResp3 += 0.00005 * (respSig - smoothResp3);      
        finalResp = max((smoothResp-0.25), 0);
      
        //////AUDIO TRANSFORMATIONS////////////////////////////////////////////////

        //RESPIRATION///////////////////////////////
    
        //use breaths per minute to detune fundamental - if breathing is regular it will not be affected
        respBPMfun = (respBPM*(3*configuration::board_id+1))-0.5; 
        respWave2.frequency(respBPMfun+respTone); //detuned wave mixed with fundamental
        
        respnoiseLFO.frequency(respAmp*100); //gently modulate respNoise through the amplitude of the breath

        
        respAmp2.gain(smoothResp3*0.75); //secondary resptone comes in through lowpass filter    
        respFilter.frequency(respTone*(respAmp)); // moving bandpass around the secondary respTone    
    
        //set the gain of respiration tones in accordance with respiration signal
        for (int x=0; x <=3; x++) {
        respMixer.gain(x, max((finalResp-0.3), 0)); //linked to resp signal and a little quieter
        if(x==2) respMixer.gain(x, smoothResp2*0.05); //linked to a lowpassed resp signal and even quieter
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
        int y = map(GSRfilt, 1, 100, 750, 10000);
        int y2 = map(GSRfilt, 1, 100, 0, 8000);
        
        GSRfilter.frequency(y);
        GSRfilter1.frequency(y2);
       /*
      //section to silence GSR pops 
       GSRfilter.frequency(0);
       GSRfilter1.frequency(0);
      */
        
        processed_for_leds.heart.sig = smoothHeart*0.7;
        processed_for_leds.gsr.scr = smoothGSR;
        processed_for_leds.resp.sig = finalResp*1.2;

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
 Serial.println(currentSection);   
 if (currentSection==0){
        respWave1.begin(0.5, respTone, WAVEFORM_SINE);
        respWave2.begin(0.2, respTone, WAVEFORM_SINE);   
        }
        else if (currentSection==1){
          respWave1.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);
          respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);   
        }
        else if (currentSection==2){
          respWave1.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SQUARE);
          respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SQUARE);   
        }
        else if (currentSection==3){
          respWave1.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);
           respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);   
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
