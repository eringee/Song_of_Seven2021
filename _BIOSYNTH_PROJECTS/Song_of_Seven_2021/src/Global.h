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

int pedalMarker = 0;

int sectionGlobal[NUM_SECTIONS][NUM_BOARDS] = {
 {622,933,1244,1555,1866,2177,2488},  // going to canada
 {174,348,1244,1555,1566,1740,2488},  // cat died
 {392,416,448,587,659,1046,1174},     // fluffy bunny
 {1148,1312,1476,1640,1804,1968,2132} // board games
};

Chrono openingMessageTimer;
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
AudioSynthWaveform       waveform2;      //xy=938,673
AudioSynthWaveform       waveform3;      //xy=978,788
AudioSynthWaveformSineModulated sine_fm3;       //xy=1095,677
AudioSynthWaveformSine   sine1;          //xy=1099,717
AudioSynthWaveformSineModulated sine_fm4;       //xy=1117,784
AudioSynthWaveformSine   sine_fm2;          //xy=1151,541
AudioAmplifier           amp1;           //xy=1289,543
AudioMixer4              mixerMain;         //xy=1421,584
AudioOutputI2S           AudioOut;           //xy=1580,585
AudioConnection          patchCord1(waveform2, sine_fm3);
AudioConnection          patchCord2(waveform3, sine_fm4);
AudioConnection          patchCord3(sine_fm3, 0, mixerMain, 1);
AudioConnection          patchCord4(sine1, 0, mixerMain, 2);
AudioConnection          patchCord5(sine_fm4, 0, mixerMain, 3);
AudioConnection          patchCord6(sine_fm2, amp1);
AudioConnection          patchCord7(amp1, 0, mixerMain, 0);
AudioConnection          patchCord8(mixerMain, 0, AudioOut, 0);
AudioConnection          patchCord9(mixerMain, 0, AudioOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=1024,436
// GUItool: end automatically generated code
