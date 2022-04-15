#define BOARD_ID 5  // !!!!!REMEMBER: BOARD ID MINUS 1 !!!!!

#define NUM_SECTIONS 5 // always add 1?
#define NUM_BOARDS 11  // actually number of boards +1?

// Use these with the Teensy Audio Shield
#define USE_SDCARD false
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#define VOL_POT_PIN A1

#define NO_TOUCH_DELAY 3000

#include <Chrono.h>
#include <mtof.h>

int pedalMarker = 0;

float sectionGlobal[NUM_SECTIONS][NUM_BOARDS] = {
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
 
 {mtof.toFrequency(72),
 mtof.toFrequency(86),
 mtof.toFrequency(76), 
 mtof.toFrequency(89), 
 mtof.toFrequency(79), 
 mtof.toFrequency(93), 
 mtof.toFrequency(83), 
 mtof.toFrequency(96), 
 mtof.toFrequency(86), 
 mtof.toFrequency(100)},  // octave separated major scale
 
 {392,416,448,587,659,1046,1174, 1300, 1400, 1700},     // section 3
 {1148,1312,1476,1640,1804,1968,2132, 2500, 2600, 2700} // section 4
};

float respTone;

Chrono openingMessageTimer;  //timer for how long the opening message stays on the LCD screen
int openingMessageTime = 3000;

int currentSection = 0;
int lastSection = -1;
bool updateLCDBool = true;
int envelope1delay = 0;

/////////////////////////////AUDIO VARS//////////////////////////
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform5;      //xy=64,125
AudioSynthNoiseWhite     noise1;         //xy=140,494
AudioSynthWaveform       waveform4;      //xy=140,582
AudioEffectBitcrusher    bitcrusher1;    //xy=161,191
AudioEffectEnvelope      envelope1;      //xy=272,123
AudioSynthWaveform       waveform2;      //xy=335,391
AudioEffectMultiply      multiply1;      //xy=335,518
AudioSynthWaveform       waveform3;      //xy=336,435
AudioAmplifier           amp1;           //xy=402,167
AudioMixer4              respMixer;         //xy=527,424  aka mixer2
AudioMixer4              mixer3;         //xy=531,164     
AudioFilterStateVariable respFilter;        //xy=688,482  aka filter2
AudioFilterStateVariable GSRfilter;        //xy=692,174  aka filter1
AudioAmplifier           amp2;           //xy=735,414
AudioMixer4              mainMixer;         //xy=822,306  aka mixer1
AudioOutputI2S           AudioOut;       //xy=996,307
AudioConnection          patchCord1(waveform5, bitcrusher1);
AudioConnection          patchCord2(noise1, 0, multiply1, 0);
AudioConnection          patchCord3(waveform4, 0, multiply1, 1);
AudioConnection          patchCord4(bitcrusher1, envelope1);
AudioConnection          patchCord5(envelope1, amp1);
AudioConnection          patchCord6(waveform2, 0, respMixer, 0);
AudioConnection          patchCord7(multiply1, 0, respMixer, 2);
AudioConnection          patchCord8(waveform3, 0, respMixer, 1);
AudioConnection          patchCord9(amp1, 0, mixer3, 1);
AudioConnection          patchCord10(respMixer, 0, respFilter, 0);
AudioConnection          patchCord11(respMixer, 0, mainMixer, 1);
AudioConnection          patchCord12(mixer3, 0, GSRfilter, 0);
AudioConnection          patchCord13(respFilter, 1, amp2, 0);
AudioConnection          patchCord14(GSRfilter, 1, mainMixer, 0);
AudioConnection          patchCord15(amp2, 0, mainMixer, 3);
AudioConnection          patchCord16(mainMixer, 0, AudioOut, 0);
AudioConnection          patchCord17(mainMixer, 0, AudioOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=87,39
// GUItool: end automatically generated code
