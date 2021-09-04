void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{

  //get the midi data from the passed midi event
  int channel = pev->channel+1;
  int messageType = pev->data[0];
  int midiNote = pev->data[1];
  int velocity = pev->data[2];
  
  
  if( DEBUG_MIDI )
  { 
    Serial.println("MIDI EVENT:");
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

  //parse the received midi message. In this project we only care for noteOn/noteOff messages
  switch(messageType)
  {
    case 144 : //NOTE ON
      biosynth.playNote(channel, midiNote,velocity);
      break;
    
    case 128 : //NOTE OFF
      biosynth.stopNote(channel);
      break;
   
  }

}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  for (int c = 0; c < 16; c++) //could be set to 4 because we use only for channel on the bio synth
    biosynth.stopNote(c); 
}

void tickMetronome(void) 
// use to make flash something to the beat. Ask Erin if this is something we'd like to implement
// otherwise this function is not useful
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
/*
  Runs every loop. This function handles the update of the midi file reading
*/
static enum { S_IDLE, S_PLAYING, S_END, S_WAIT_BETWEEN } state = S_IDLE;
  static uint16_t currTune = ARRAY_SIZE(tuneList);
  static uint32_t timeStart;

  switch (state)
  {
  case S_IDLE:    // now idle, set up the next tune
    {
      int err;

      Serial.println("IDLE");

      //currTune++;
      if (currentSection >= ARRAY_SIZE(tuneList)) //prevent overflow in array
        currentSection = 0;

      // use the next file name and play it
      Serial.println("File: ");
      Serial.println(tuneList[currentSection]);

      //load the track
      err = SMF.load(tuneList[currentSection]);
      if (err != MD_MIDIFile::E_OK)
      {
        Serial.println(" - SMF load Error ");
        Serial.println(err);
        //digitalWrite(SMF_ERROR_LED, HIGH);
        timeStart = millis();
        state = S_WAIT_BETWEEN;
        Serial.println("WAIT_BETWEEN");
      }
      else
      {
        Serial.println("PLAYING");
        state = S_PLAYING;
      }
    }
    break;

  case S_PLAYING: // play the file
    
    if (!SMF.isEOF()) //if not at the end of the file
    {
      if (SMF.getNextEvent()) 
          tickMetronome();
    }
    else
      state = S_END;
    break;

  case S_END:   // done with this one
    Serial.println("END");
    SMF.close();
    midiSilence();
    timeStart = millis();
    state = S_IDLE;
    break;


  default:
    state = S_IDLE;
    break;
  }
}

float setVolume()
{   /*
      This function reads the potentiometer that control the master volume
      It returns a scaled float representing the volume value
    */
    int vol = analogRead(VOL_POT_PIN);

    return (float)vol /1280;
}

void setupAudioShield()
{/*
  This function setup the audio shield hardware

  */
   pinMode(VOL_POT_PIN,INPUT);
   AudioMemory(AUDIO_MEM);  //put in Audio Memory or weird clicks happen
   
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
  /*
    In this function you can set up the initial states of every synth in the project
    It should be rewritten when the audio pipeline is rewritten
    Make sure to keep the same name convention used in the audio pipieline 
    In either case, the midi frequencies and sensor values will write over these right away.
  
    heartSine.frequency(440); 
    heartSine.amplitude(0.2);
    
    gsr1Sine.frequency(622);
    gsr1Sine.amplitude(0.2);

    gsr2Sine.frequency(880);
    gsr2Sine.amplitude(0.2);

    respSine.frequency(220);
    heartSine.amplitude(0.2);
    
   
    */

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

void setupEnvelopes()
{ /*
    In this function you set up the initial states of every enveloppe in the project
    In order to play notes from the midi file every synth linked to the file has an envelope
    It should be rewritten when the audio pipeline is rewritten
    Make sure to keep the same name convention used in the audio pipieline 
  */
   envelopeHeart.attack(5);
   envelopeHeart.sustain(2);
   envelopeHeart.decay(2);
   envelopeHeart.release(5);
   envelopeHeart.releaseNoteOn(10);

   envelopeGSR1.attack(5);
   envelopeGSR1.sustain(2);
   envelopeGSR1.decay(2);
   envelopeGSR1.release(5);
   envelopeGSR1.releaseNoteOn(10);

   envelopeGSR2.attack(5);
   envelopeGSR2.sustain(2);
   envelopeGSR2.decay(2);
   envelopeGSR2.release(5);
   envelopeGSR2.releaseNoteOn(10);

   envelopeTemp.attack(5);
   envelopeTemp.sustain(2);
   envelopeTemp.decay(2);
   envelopeTemp.release(5);
   envelopeTemp.releaseNoteOn(10);

}



void checkSectionChange()  //this is where we change sections AND frequencies...
{
    if( encoderButton.read() == 0 && biosynth.getLCDState() == 1)
    {   //section has been confirmed
 
        lastSection = currentSection;
        currentSection = biosynth.getEncoderValue();
        updateLCDBool = true;

        // /// THIS IS WHERE YOU NEED TO UPDATE THE FREQUENCY VALUES
        // sine_fm2.frequency(sectionGlobal[currentSection][BOARD_ID]);
        // if (currentSection==0){
        //   sine_fm3.frequency(311);   //atmospheric sine1
        //   sine1.frequency(424);      //atmospheric sine2
        // }
        // else if (currentSection==1){
        //   sine_fm3.frequency(311);   //atmospheric sine1
        //   sine1.frequency(369);      //atmospheric sine2
        // }
        // else if (currentSection==2){
        //   sine_fm3.frequency(261.63);   //atmospheric sine1
        //   sine1.frequency(329.63);      //atmospheric sine2
        //   sine_fm3.amplitude(0.1);  
        //   sine1.amplitude(0.1);
        // }
        // else if (currentSection==3){
        //   waveform2.frequency(1);
        //   sine_fm3.frequency(659);   //atmospheric sine1
        //   sine1.frequency(985);      //atmospheric sine2
        //   sine_fm3.amplitude(0.1);  
        //   sine1.amplitude(0.1);
        // }
        biosynth.setLCDState(2);
    }
}
