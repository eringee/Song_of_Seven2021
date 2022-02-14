#define BOARD_ID 1  //change this to suit which board you are working with

#define NUM_SECTIONS 4 //how many sections in the work?
#define NUM_BOARDS 7  //how many different boards are you working with?

// Use these with the Teensy Audio Shield
#define USE_SDCARD false
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#define VOL_POT_PIN A1

#define NO_TOUCH_DELAY 3000

#include <Chrono.h>

int pedalMarker = 0;

int sectionGlobal[NUM_SECTIONS][NUM_BOARDS] = {
 {100,100,1244,1555,1866,2177,2488},  // going to canada
 {174,348,1244,1555,1566,1740,2488},  // cat died
 {392,416,448,587,659,1046,1174},     // fluffy bunny
 {1148,1312,1476,1640,1804,1968,2132} // board games
};

float fundamentalWaveGain = 0.3; // the gain of the fundamental signal

Chrono openingMessageTimer;  //timer for how long the opening message stays on the LCD screen
int openingMessageTime = 3000;

int currentSection = 0;
int lastSection = -1;
bool updateLCDBool = true;
/////////////////////////////AUDIO VARS//////////////////////////
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       fundamentalWave;      //xy=956,552
AudioAmplifier           fundWaveAmp;           //xy=1140,550
AudioSynthWaveformDc     timbreMod;            //xy=1144,474
AudioSynthWaveform       breathingWave; //xy=1223,747
AudioSynthWaveformDc     timbreWidthMod;           //xy=1306,610
AudioEffectWaveFolder    waveMultiply1;         //xy=1333,520
AudioAmplifier           breathingWaveAmp; //xy=1457,726
AudioEffectMultiply      waveMultiply2;           //xy=1488,580
AudioMixer4              mixerMain;         //xy=1662,693
AudioOutputI2S           AudioOut;           //xy=1822,487
AudioConnection          patchCord1(fundamentalWave, fundWaveAmp);
AudioConnection          patchCord2(fundWaveAmp, 0, waveMultiply1, 1);
AudioConnection          patchCord3(timbreMod, 0, waveMultiply1, 0);
AudioConnection          patchCord4(breathingWave, breathingWaveAmp);
AudioConnection          patchCord5(timbreWidthMod, 0, waveMultiply2, 1);
AudioConnection          patchCord6(waveMultiply1, 0, waveMultiply2, 0);
AudioConnection          patchCord7(breathingWaveAmp, 0, mixerMain, 1);
AudioConnection          patchCord8(waveMultiply2, 0, mixerMain, 0);
AudioConnection          patchCord9(mixerMain, 0, AudioOut, 0);
AudioConnection          patchCord10(mixerMain, 0, AudioOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1024,436
// GUItool: end automatically generated code
