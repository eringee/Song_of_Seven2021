#define BOARD_ID 7
#define NUM_SECTIONS 4
#define NUM_BOARDS 7

// Use these with the Teensy Audio Shield
#define USE_SDCARD false
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#define VOL_POT_PIN A1

#define NO_TOUCH_DELAY 3000

#include <Chrono.h>
#include <LightChrono.h>

int sectionGlobal[NUM_SECTIONS][NUM_BOARDS] = {
 {622,933,1244,1555,1866,2177,2488},  // going to canada
 {174,348,1244,1555,1566,1740,2488},  // cat died
 {392,416,448,587,659,1046,1174},     // fluffy bunny
 {1148,1312,1476,1640,1804,1968,2132} // board games
};

Chrono openingMessageTimer;
int openingMessageTime = 3000;

int currentSection = 0;

/////////////////////////////AUDIO VARS//////////////////////////
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=278,354
AudioSynthNoiseWhite     noise1;         //xy=307,214
AudioSynthWaveform       waveform3;      //xy=318,469
AudioSynthWaveform       waveform1;      //xy=352,98
AudioSynthWaveformSineModulated sine_fm3;       //xy=435,358
AudioSynthWaveformSine   sine1;          //xy=439,398
AudioSynthWaveformSineModulated sine_fm4;       //xy=457,465
AudioSynthWaveformSineModulated sine_fm2;       //xy=462,209
AudioSynthWaveformSineModulated sine_fm1;       //xy=493,96
AudioEffectEnvelope      envelope1;      //xy=634,100
AudioMixer4              mixer1;         //xy=685,195
AudioMixer4              mixer2;         //xy=739,388
AudioOutputI2S           i2s1;           //xy=920,266
AudioConnection          patchCord1(waveform2, sine_fm3);
AudioConnection          patchCord2(noise1, sine_fm2);
AudioConnection          patchCord3(waveform3, sine_fm4);
AudioConnection          patchCord4(waveform1, sine_fm1);
AudioConnection          patchCord5(sine_fm3, 0, mixer2, 1);
AudioConnection          patchCord6(sine1, 0, mixer2, 2);
AudioConnection          patchCord7(sine_fm4, 0, mixer2, 3);
AudioConnection          patchCord8(sine_fm2, 0, mixer1, 1);
AudioConnection          patchCord9(sine_fm1, envelope1);
AudioConnection          patchCord10(envelope1, 0, mixer1, 0);
AudioConnection          patchCord11(mixer1, 0, mixer2, 0);
AudioConnection          patchCord12(mixer2, 0, i2s1, 0);
AudioConnection          patchCord13(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=979,355
// GUItool: end automatically generated code
