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
  "midi_test.mid",
  "naruto.mid", 
  "tetris.mid",
  "ff7_test.mid"
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
AudioSynthNoiseWhite     noise1;         //xy=69.19999694824219,544.800048828125
AudioSynthWaveform       waveform1;      //xy=76.20001220703125,447.79998779296875
AudioSynthWaveform       waveform2;      //xy=81.19999694824219,700.7999877929688
AudioSynthWaveform       waveform3;      //xy=84.19999694824219,842.7999877929688
AudioSynthWaveformSineModulated sine_fm2;       //xy=213.1999969482422,543.8000183105469
AudioSynthWaveformSineModulated sine_fm1;       //xy=215.1999969482422,447.79998779296875
AudioSynthWaveformSineModulated sine_fm3;       //xy=220.1999969482422,700.7999877929688
AudioSynthWaveformSine   sine1;          //xy=225.1999969482422,760.7999877929688
AudioSynthWaveformSineModulated sine_fm4;       //xy=227.1999969482422,842.7999877929688
AudioMixer4              mixer1;         //xy=375.20001220703125,510.79998779296875
AudioEffectEnvelope      envelopeHeart;      //xy=446.20001220703125,844.2000122070312
AudioEffectEnvelope      envelopeTemp; //xy=448.20001220703125,761.7999877929688
AudioEffectEnvelope      envelopeGSR2; //xy=452.20001220703125,701.7999877929688
AudioEffectEnvelope      envelopeGSR1;      //xy=525.2000122070312,510.20001220703125
AudioMixer4              mixer2;         //xy=762.2000122070312,687.7999877929688
AudioOutputI2S           i2s1;           //xy=921.2000122070312,686.7999877929688
AudioConnection          patchCord1(noise1, sine_fm2);
AudioConnection          patchCord2(waveform1, sine_fm1);
AudioConnection          patchCord3(waveform2, sine_fm3);
AudioConnection          patchCord4(waveform3, sine_fm4);
AudioConnection          patchCord5(sine_fm2, 0, mixer1, 1);
AudioConnection          patchCord6(sine_fm1, 0, mixer1, 0);
AudioConnection          patchCord7(sine_fm3, envelopeGSR2);
AudioConnection          patchCord8(sine1, envelopeTemp);
AudioConnection          patchCord9(sine_fm4, envelopeHeart);
AudioConnection          patchCord10(mixer1, envelopeGSR1);
AudioConnection          patchCord11(envelopeHeart, 0, mixer2, 3);
AudioConnection          patchCord12(envelopeTemp, 0, mixer2, 2);
AudioConnection          patchCord13(envelopeGSR2, 0, mixer2, 1);
AudioConnection          patchCord14(envelopeGSR1, 0, mixer2, 0);
AudioConnection          patchCord15(mixer2, 0, i2s1, 0);
AudioConnection          patchCord16(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=911.2000122070312,762.7999877929688
// GUItool: end automatically generated code

