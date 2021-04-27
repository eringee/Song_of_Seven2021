
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

void setupSounds()
{


    /******************************  OLD CODE
    waveform1.begin(0.4, 1020, WAVEFORM_SINE);  //something below 32Hz displays a clear beating pattern rathern than modulation
    waveform1.amplitude(0.2);
    sine_fm1.frequency(311);
    

    sine2.frequency(frequency[BOARD_ID-1]);           //harmonics of Eb -  1-622 2-933 
                                    // 3-1244 4- 1555 5-1866 
                                    // 6- 2177 7- 2488  
    sine2.amplitude(0.0);

    ********/
    
    //GSR dependent variables
    noise1.amplitude(0.3);

    sine_fm2.frequency[BOARD_ID-1]);      //harmonics of Eb -  1-622 2-933 
                                          // 3-1244 4- 1555 5-1866 
                                          // 6- 2177 7- 2488  
    sine_fm2.amplitude(0.0);
    
    //HEART dependent variables
    waveform1.begin(0.4, 1020, WAVEFORM_SINE);  //something below 32Hz displays a clear beating pattern rathern than modulation
    sine_fm1.frequency(500);   
    sine_fm1.amplitude(0.05);  

    //ATMOSPHERIC SINES
    waveform2.begin(0.004, 1, WAVEFORM_SINE);
    sine_fm3.frequency(311);           //atmospheric sines
    sine1.frequency(424);
    sine_fm3.amplitude(0.1);
    sine1.amplitude(0.03);

    waveform3.begin(0.05, 1, WAVEFORM_SINE);
    sine_fm4.frequency(622);
    sine_fm4.amplitude(0.075);
}

void setupEnvelopes()
{
    envelope1.attack(1);            //attack envelope for heartbeat
    envelope1.decay(200);
    envelope1.release(100);
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

void checkSectionChange()
{
    if( encoderButton.read() == 0 && biosynth.getLCDState() == 1)
    {   //section has been confirmed
        currentSection = biosynth.getEncoderValue(); 
        biosynth.setLCDState(2);
    }
}
