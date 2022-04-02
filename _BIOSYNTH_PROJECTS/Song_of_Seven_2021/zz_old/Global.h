#pragma once
#include"RingBuf.h"
#include <Audio.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Chrono.h>

#define BOARD_ID 7
#define NUM_SECTIONS 4
#define NUM_BOARDS 7


// Use these with the Teensy Audio Shield
#define USE_SDCARD true
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#define VOL_POT_PIN A1

FsFile session;

#define NO_TOUCH_DELAY 3000



int pedalMarker = 0;

int sectionGlobal[NUM_SECTIONS][NUM_BOARDS] = {
 {622,933,1244,1555,1866,2177,2488},  // going to canada
 {174,348,1244,1555,1566,1740,2488},  // cat died
 {392,416,448,587,659,1046,1174},     // fluffy bunny
 {1148,1312,1476,1640,1804,1968,2132} // board games
};

Chrono openingMessageTimer;
int openingMessageTime = 3000;

int current_section = 0;
int last_section = -1;
bool updateLCDBool = true;
/////////////////////////////AUDIO VARS//////////////////////////



// GUItool: begin automatically generated code
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
AudioConnection          patchCord1(waveform3, amp2);
AudioConnection          patchCord2(waveform2, sine_fm3);
AudioConnection          patchCord3(sine_fm3, 0, mixerMain, 1);
AudioConnection          patchCord4(sine1, 0, mixerMain, 2);
AudioConnection          patchCord5(amp2, sine_fm4);
AudioConnection          patchCord6(sine_fm2, amp1);
AudioConnection          patchCord7(sine_fm4, 0, mixerMain, 3);
AudioConnection          patchCord8(amp1, 0, mixerMain, 0);
AudioConnection          patchCord9(mixerMain, 0, AudioOut, 0);
AudioConnection          patchCord10(mixerMain, 0, AudioOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1156,306
// GUItool: end automatically generated code
