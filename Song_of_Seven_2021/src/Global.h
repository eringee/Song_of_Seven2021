#define BOARD_ID 1
#define NUM_SECTIONS 4

// Use these with the Teensy Audio Shield
#define USE_SDCARD false
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#define VOL_POT_PIN A1

#define NO_TOUCH_DELAY 3000

#include <Chrono.h>
#include <LightChrono.h>


int frequency[7] = {622,933,1244,1555,1866,2177,2488}; //Tone of the sinewave according to board id
int sectionTones[NUM_SECTIONS] = {1000,800,600,440}; //change the tone of every section here
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
AudioSynthWaveform       waveform1;      //xy=97.55556106567383,989.4445133209229
AudioSynthWaveformSine   sine2;          //xy=97.33333587646484,1091.1110401153564
AudioSynthWaveformSine   sine3; //xy=97.33333587646484,1129.1110401153564
AudioSynthWaveform       waveform2; //xy=98.44446182250977,1027.4444561004639
AudioSynthWaveformSine   sine4; //xy=99.33333587646484,1164.1110401153564
AudioSynthWaveformSineModulated sine_fm1;       //xy=227.66666793823242,988.2222232818604
AudioSynthWaveformSineModulated sine_fm2; //xy=228.5555534362793,1027.3333110809326
AudioEffectEnvelope      envelope1;      //xy=361.6666831970215,988.3333396911621
AudioEffectEnvelope      envelope2; //xy=363.5555839538574,1027.6666374206543
AudioMixer4              mixer1;         //xy=503.7777862548828,1023.6666984558105
AudioMixer4              mixer2; //xy=629,1116
AudioOutputI2S           i2s1;           //xy=760.0000114440918,1117.7777738571167
AudioConnection          patchCord1(waveform1, sine_fm1);
AudioConnection          patchCord2(sine2, 0, mixer2, 1);
AudioConnection          patchCord3(sine3, 0, mixer2, 2);
AudioConnection          patchCord4(waveform2, sine_fm2);
AudioConnection          patchCord5(sine4, 0, mixer2, 3);
AudioConnection          patchCord6(sine_fm1, envelope1);
AudioConnection          patchCord7(sine_fm2, envelope2);
AudioConnection          patchCord8(envelope1, 0, mixer1, 0);
AudioConnection          patchCord9(envelope2, 0, mixer1, 1);
AudioConnection          patchCord10(mixer1, 0, mixer2, 0);
AudioConnection          patchCord11(mixer2, 0, i2s1, 0);
AudioConnection          patchCord12(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=772.2222709655762,1076.6666450500488
// GUItool: end automatically generated code

