#define BOARD_ID 1
#define NUM_SECTIONS 4
#define NUM_BOARDS 7

// Use these with the Teensy Audio Shield
#define USE_SDCARD true
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14
#define VOL_POT_PIN A1
#define AUDIO_MEM 50

#define NO_TOUCH_DELAY 3000

#include <Audio.h>
#include <SerialFlash.h>
#include <Chrono.h>
#include <SPI.h>


#define DEBUG_MIDI false
#include <SdFat.h>
#include <MD_MIDIFile.h>
#include  <mtof.h>

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).

//make a function to autopopulate this array with files in MIDI_FILE FOLDER


//ADD THE NAME OF THE FILE ON THE CARD SD YOU WANT TO PLAY HERE vvv
const char *tuneList[] = 
{
  "test01.mid",
  "test02.mid",
   "ff7_test.mid", 
   "tetris.mid",
   
};

SdFat card;
MD_MIDIFile SMF;
Chrono openingMessageTimer;
int openingMessageTime = 3000;

int currentSection = 0;
int lastSection = -1;
bool updateLCDBool = true;
/////////////////////////////AUDIO VARS//////////////////////////
/* 
AUDIO DESIGN TOOL LINK
https://www.pjrc.com/teensy/gui/index.html 

Import the code below to modify the audio pipeline
 */

// GUItool: begin automatically generated code
AudioSynthWaveformSineModulated gsr2Sine; //xy=315,402
AudioSynthWaveformSineModulated gsr1Sine;       //xy=320,348
AudioSynthWaveformSineModulated respSine;       //xy=339,460
AudioSynthWaveformSineModulated heartSine;       //xy=378,289
AudioEffectEnvelope      envelopeGSR1; //xy=513,340
AudioEffectEnvelope      envelopeGSR2; //xy=515,390
AudioEffectEnvelope      envelopeTemp; //xy=517,447
AudioEffectEnvelope      envelopeHeart;      //xy=541,295
AudioMixer4              SignalMixer;         //xy=739,388
AudioOutputI2S           headphoneOut;           //xy=920,266
AudioConnection          patchCord1(gsr2Sine, envelopeGSR2);
AudioConnection          patchCord2(gsr1Sine, envelopeGSR1);
AudioConnection          patchCord3(respSine, envelopeTemp);
AudioConnection          patchCord4(heartSine, envelopeHeart);
AudioConnection          patchCord5(envelopeGSR1, 0, SignalMixer, 1);
AudioConnection          patchCord6(envelopeGSR2, 0, SignalMixer, 2);
AudioConnection          patchCord7(envelopeTemp, 0, SignalMixer, 3);
AudioConnection          patchCord8(envelopeHeart, 0, SignalMixer, 0);
AudioConnection          patchCord9(SignalMixer, 0, headphoneOut, 0);
AudioConnection          patchCord10(SignalMixer, 0, headphoneOut, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=979,355
// GUItool: end automatically generated code
