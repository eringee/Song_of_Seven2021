/*
MIDI FILE READER FOR ERIN GEE'S BIOSYNTH

READ A MIDI FILE AND PASSES THE INFORMATION OF EACH TRACK
TO THE ATTACHED VOICE ON THE SYNTHESIZER
*/

#include <Arduino.h>
#include <SPI.h>
#include <Audio.h>
#include <SdFat.h> //NOT THE SAME AS THE SD LIBRARY USED ON THE BIOSYNTH
#include <MD_MIDIFile.h>



#define USE_MIDI  0   // set to 1 to enable MIDI output, otherwise debug output

#if USE_MIDI // set up for direct MIDI serial output

#define DEBUG(x)
#define DEBUGX(x)
#define DEBUGS(s)
#define SERIAL_RATE 31250

#else // don't use MIDI to allow printing debug statements

#define DEBUG(x)  Serial.print(x)
#define DEBUGX(x) Serial.print(x, HEX)
#define DEBUGS(s) Serial.print(F(s))
#define SERIAL_RATE 57600

#endif // USE_MIDI


//SD CARD DEFINITION
const uint8_t SD_SELECT = 10;
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14


const uint16_t WAIT_DELAY = 2000; // ms

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
const char *partsList[] = { "midi_test.mid"}; //this should autopopulate looking through a folder in setup

SdFat	card;
MD_MIDIFile SMF;


//function that receive the midi events. Parse here
void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
#if USE_MIDI
  if ((pev->data[0] >= 0x80) && (pev->data[0] <= 0xe0))
  {
    Serial.write(pev->data[0] | pev->channel);
    Serial.write(&pev->data[1], pev->size-1);
  }
  else
    Serial.write(pev->data, pev->size);
#endif
  DEBUG("\n");
  DEBUG(millis());
  DEBUG("\tM T");
  DEBUG(pev->track);
  DEBUG(":  Ch ");
  DEBUG(pev->channel+1);
  DEBUG(" Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
  DEBUGX(pev->data[i]);
    DEBUG(' ');
  }
}

//could be removed
void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{
  DEBUG("\nS T");
  DEBUG(pev->track);
  DEBUG(": Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
    DEBUGX(pev->data[i]);
    DEBUG(' ');
  }
}


//helpers
void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  midi_event ev;

  // All sound off
  // When All Sound Off is received all oscillators will turn off, and their volume
  // envelopes are set to zero as soon as possible.
  ev.size = 0;
  ev.data[ev.size++] = 0xb0;
  ev.data[ev.size++] = 120;
  ev.data[ev.size++] = 0;

  for (ev.channel = 0; ev.channel < 16; ev.channel++)
    midiCallback(&ev);
}


void setup(void)
{
  
  Serial.begin(SERIAL_RATE);

 // Initialize SD
  SPI.setMOSI(SDCARD_MOSI_PIN);  // Audio shield has MOSI on pin 7
  SPI.setSCK(SDCARD_SCK_PIN);  // Audio shield has SCK on pin 14
 
  //verify sd card. Could be passed to midihandler object if transformed in class
  if (!card.begin(SD_SELECT, SPI_FULL_SPEED)) 
  {
    DEBUG("\nSD init fail!");
    while (true) ;
  }

  // Initialize MIDIFile
  SMF.begin(&card);
  //set callback functions
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback); //could be removed
}

void tickMetronome(void) //propose to implement this to erin
{
  // flash a LED to the beat

  static uint32_t lastBeatTime = 0;
  static boolean  inBeat = false;
  uint16_t  beatTime;

  beatTime = 60000/SMF.getTempo();    // msec/beat = ((60sec/min)*(1000 ms/sec))/(beats/min)
  if (!inBeat)
  {
    if ((millis() - lastBeatTime) >= beatTime)
    {
      lastBeatTime = millis();
      //digitalWrite(BEAT_LED, HIGH);
      inBeat = true;
    }
  }
  else
  {
    if ((millis() - lastBeatTime) >= 100)	// keep the flash on for 100ms only
    {
      //digitalWrite(BEAT_LED, LOW);
      inBeat = false;
    }
  }
}

void loop(void) //transform in update method
{
  //dont understant why this is reinitialised every loop
  static enum { S_IDLE, S_PLAYING, S_END, S_WAIT_BETWEEN } state = S_IDLE;
  static uint16_t currPart = ARRAY_SIZE(partsList);
  static uint32_t timeStart;

  switch (state)
  {
  case S_IDLE:    // now idle, set up the next tune
    {
      int err;

      DEBUGS("\nS_IDLE");

      currPart++;
      if (currPart >= ARRAY_SIZE(partsList)) //prevent overflow in array
        currPart = 0;

      // use the next file name and play it
      DEBUG("\nFile: ");
      DEBUG(partsList[currPart]);

      //load the track
      err = SMF.load(partsList[currPart]);
      if (err != MD_MIDIFile::E_OK)
      {
        DEBUG(" - SMF load Error ");
        DEBUG(err);
        //digitalWrite(SMF_ERROR_LED, HIGH);
        timeStart = millis();
        state = S_WAIT_BETWEEN;
        DEBUGS("\nWAIT_BETWEEN");
      }
      else
      {
        //DEBUGS("\nS_PLAYING");
        state = S_PLAYING;
      }
    }
    break;

  case S_PLAYING: // play the file
    //DEBUGS("\nS_PLAYING");
    if (!SMF.isEOF()) //if not at the end of the file
    {
      if (SMF.getNextEvent())
      {
        //tickMetronome();
      }
    }
    else
      state = S_END;
    break;
  
  case S_END:   // done with this one
    DEBUGS("\nS_END");
    SMF.close();
    midiSilence();
    timeStart = millis();
    state = S_WAIT_BETWEEN;
    DEBUGS("\nWAIT_BETWEEN");
    break;

  //veriify if needed
  case S_WAIT_BETWEEN:    // signal finished with a dignified pause
    if (millis() - timeStart >= WAIT_DELAY)
      state = S_IDLE;
    break;

  default:
    state = S_IDLE;
    break;
  }
}