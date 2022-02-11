void setupAudioShield()
{
    pinMode(VOL_POT_PIN,INPUT);
    
   
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
  sgtl5000_1.volume(0.5); //set master volume here
}

void setupSounds()  //initial sounds for Section A
{

    //GSR dependent variables

    sine_fm2.frequency(sectionGlobal[0][BOARD_ID]); 
    sine_fm2.amplitude(0.5);
    
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
