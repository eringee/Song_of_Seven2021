#ifndef MidiHandler_h
#define MidiHandler_h

#include "Arduino.h"
#include <MD_MIDIFile.h>
#include <SPI.h>
#include <SdFat.h> 

#define MAX_TRACKS 16
 

//MACROS
#define DEBUG(x)  Serial.print(x)
#define DEBUGX(x) Serial.print(x, HEX)
#define DEBUGS(s) Serial.print(F(s))
#define SERIAL_RATE 57600

//SD CARD DEFINITION/
//TEMPORARY THESE INFOS ARE PROVIDED ELSEWHERE IN THE SKETCH
const uint8_t SD_SELECT = 10;
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

class MidiHandler
{
    private : 
    enum EventName : uint8_t
    {
        VoiceNoteOff = 0x80,
        VoiceNoteOn = 0x90,
        VoiceAftertouch = 0xA0,
        VoiceControlChange = 0xB0,
        VoiceProgramChange = 0xC0,
        VoiceChannelPressure = 0xD0,
        VoicePitchBend = 0x80,
        SystemExclusive = 0xF0,     
    };

    int synthTrackAssing[16] = {}; //0 = no synth , 1 = heart , 2 = gsr1, 3 = gsr2 , 4 = temp
    const uint16_t WAIT_DELAY = 2000; // ms
    #define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
    const char *partsList[] = { "midi_test.mid"}; //this should autopopulate looking through a folder in setup

    SdFat	SD;
    MD_MIDIFile SMF;


    // Turn everything off on every channel.
    // Some midi files are badly behaved and leave notes hanging, so between songs turn
    // off all the notes and sound
    void midiSilence();

    void tickMetronome();
    void readFile();
    void linkSynths( int *assignment[]);


    public :
    MidiHandler(SdFat *_SD ,void (*mCallback)(midi_event *pev));

    void update();



};

#endif