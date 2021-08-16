void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{
  //Serial.println("POTATO");
  //MIDISERIAL.write(pev -> data);
  // DEBUG("\n");
  // DEBUG(millis());
  // DEBUG("\tM T");
  // DEBUG(pev->track);
  // DEBUG(":  Ch ");
  // DEBUG(pev->channel+1);
  // DEBUG(" Data ");
  // DEBUGX(pev->data[0]);
  // Serial.println();

  //  DEBUG(pev->data[1]);
  // Serial.println();
  //  DEBUG(pev->data[2]);
  // Serial.println();
  Serial.println("MIDI EVENT:");
  int channel = pev->channel+1;
  int messageType = pev->data[0];
  int midiNote = pev->data[1];
  int velocity = pev->data[2];
  
  if( DEBUG_MIDI)
  { 
    Serial.println("Channel");
    Serial.println(channel);
    Serial.println("Event type:");
    Serial.println(messageType,HEX);
    Serial.println(messageType);
    Serial.println("Midi note:");
    Serial.println(midiNote);
    Serial.println("Note velocity:");
    Serial.println(velocity);
  }
  switch(messageType)
  {
    case 144 : //NOTE ON
      biosynth.playNote(channel, midiNote,velocity);
      break;
    
    case 128 : //NOTE OFF
      //biosynth.stopNote(channel, midiNote,velocity);
      break;

    case 176 : //ALL NOTE OFF
      Serial.println("potato");
      //biosynth.stopNote(channel, midiNote,velocity);
      break;
    
  }

}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  for (int c = 0; c < 16; c++)
    biosynth.StopNote(c);
    
}

void tickMetronome(void)
// flash a LED to the beat
{
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

void updateMidi()
{
static enum { S_IDLE, S_PLAYING, S_END, S_WAIT_BETWEEN } state = S_IDLE;
  static uint16_t currTune = ARRAY_SIZE(tuneList);
  static uint32_t timeStart;

  switch (state)
  {
  case S_IDLE:    // now idle, set up the next tune
    {
      int err;

      DEBUGS("\nS_IDLE");

      currTune++;
      if (currTune >= ARRAY_SIZE(tuneList)) //prevent overflow in array
        currTune = 0;

      // use the next file name and play it
      DEBUG("\nFile: ");
      DEBUG(tuneList[currTune]);

      //load the track
      err = SMF.load(tuneList[currTune]);
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
          tickMetronome();
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

  case S_WAIT_BETWEEN:    // signal finished with a dignified pause
    //digitalWrite(READY_LED, HIGH);
    if (millis() - timeStart >= WAIT_DELAY)
      state = S_IDLE;
    break;

  default:
    state = S_IDLE;
    break;
  }
}

float setVolume()
{
    int vol = analogRead(VOL_POT_PIN);

    return (float)vol /1280;
}

void setupAudioShield()
{
    pinMode(VOL_POT_PIN,INPUT);
   AudioMemory(50);  //put in Audio Memory or weird clicks happen
   
    if(USE_SDCARD)
    {
        SPI.setMOSI(SDCARD_MOSI_PIN);  // Audio shield has MOSI on pin 7
        SPI.setSCK(SDCARD_SCK_PIN);  // Audio shield has SCK on pin 14
        if (!(SD.begin(SDCARD_CS_PIN)))//verify if sd card is inserted 
        {
            while (1)// stop here, but print a message repetitively 
            {
            Serial.println("Unable to access the SD card");
            delay(500);
            }
        }
    } 

  sgtl5000_1.enable();
  sgtl5000_1.volume(setVolume()); //set master volume here
}

void setupSounds()  //initial sounds for Section A
{

    //GSR dependent variables
    noise1.amplitude(0.01);

    sine_fm2.frequency(sectionGlobal[0][BOARD_ID]); 
    sine_fm2.amplitude(0.0);
    
    //HEART dependent variables
    waveform3.begin(0.01, 0.005, WAVEFORM_SINE);
    sine_fm4.frequency(622);
    sine_fm4.amplitude(0.02);

    //ATMOSPHERIC SINES
    waveform2.begin(0.004, 1, WAVEFORM_SINE);
    sine_fm3.frequency(311);           //atmospheric sines
    sine1.frequency(424);
    sine_fm3.amplitude(0.1);
    sine1.amplitude(0.05);
}

void openingMessage()
{
    if(openingMessageTimer.hasPassed(openingMessageTime))
    {    
        biosynth.setLCDState(2);
        openingMessageTimer.restart();
        openingMessageTimer.stop();
    }
}

void checkSectionChange()  //this is where we change sections AND frequencies...
{
    if( encoderButton.read() == 0 && biosynth.getLCDState() == 1)
    {   //section has been confirmed
 
        lastSection = currentSection;
        currentSection = biosynth.getEncoderValue();
        updateLCDBool = true;

        /// THIS IS WHERE YOU NEED TO UPDATE THE FREQUENCY VALUES
        sine_fm2.frequency(sectionGlobal[currentSection][BOARD_ID]);
        if (currentSection==0){
          sine_fm3.frequency(311);   //atmospheric sine1
          sine1.frequency(424);      //atmospheric sine2
        }
        else if (currentSection==1){
          sine_fm3.frequency(311);   //atmospheric sine1
          sine1.frequency(369);      //atmospheric sine2
        }
        else if (currentSection==2){
          sine_fm3.frequency(261.63);   //atmospheric sine1
          sine1.frequency(329.63);      //atmospheric sine2
          sine_fm3.amplitude(0.1);  
          sine1.amplitude(0.1);
        }
        else if (currentSection==3){
          waveform2.frequency(1);
          sine_fm3.frequency(659);   //atmospheric sine1
          sine1.frequency(985);      //atmospheric sine2
          sine_fm3.amplitude(0.1);  
          sine1.amplitude(0.1);
        }
        biosynth.setLCDState(2);
    }
}
