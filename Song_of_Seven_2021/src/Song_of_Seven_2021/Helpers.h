
float setVolume()
{
    int vol = analogRead(VOL_POT_PIN);
    return map(float(vol) , 0, 1023, 0.0, 0.75);
}

void setupAudioShield()
{
    pinMode(VOL_POT_PIN,INPUT);
    AudioMemory(10);
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
        currentSection = biosynth.getEncoderValue();
        Serial.println(sectionGlobal[currentSection][BOARD_ID]);  /// THIS IS WHERE YOU NEED TO UPDATE THE VALUES
        biosynth.setLCDState(2);
    }
}
