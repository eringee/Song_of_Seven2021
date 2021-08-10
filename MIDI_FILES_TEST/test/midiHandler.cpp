#include "Arduino.h"
#include "MidiHandler.h"

//CONSTRUCTOR
MidiHandler::MidiHandler(SdFat *_SD ,void (*mCallback)(midi_event *pev) ) 
{
  // Initialize MIDIFile
  SMF.begin(_SD);
  //set callback functions
  SMF.setMidiHandler(*mCallback);
  //SMF.setSysexHandler(sysexCallback);

}

void MidiHandler::midiSilence()
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
   //midiCallback(&ev); //change this to target synths and not send midi messages
}

void MidiHandler::tickMetronome() //propose to implement this to erin
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

void MidiHandler::update()
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

void MidiHandler::linkSynths(int *assignment[])
{
    int numSynths = sizeof(*assignment);

    for(int track = 0 ; track < MAX_TRACKS ; track++)
    {
        if( track > numSynths )
        {
            synthTrackAssing[track] = 0;
        }
        else
        {
            synthTrackAssing[track] = *assignment[track];
        }
    }
}
