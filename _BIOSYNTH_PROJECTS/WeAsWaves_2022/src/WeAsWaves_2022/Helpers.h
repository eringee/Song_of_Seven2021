void setupAudioShield()
{
    pinMode(VOL_POT_PIN,INPUT); //establish pin for volume adjustments
      
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
  sgtl5000_1.volume(0.8); //set master volume (do not exceed 0.8)
}

void setupSounds()  //initial sounds or Setting A
{
//create the dominant tone
  fundamentalWave.begin(0.3, sectionGlobal[currentSection][BOARD_ID], WAVEFORM_SINE); //basic tone frequency defined for each board
  breathingWave.begin(0.5, sectionGlobal[currentSection][BOARD_ID]*1.75, WAVEFORM_SINE); // basic tone frequence x 1.5
  timbreMod.amplitude(0.1);
  timbreWidthMod.amplitude(0.2);
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
        /*sine_fm2.frequency(sectionGlobal[currentSection][BOARD_ID]);
        
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
        }*/
        biosynth.setLCDState(2);
    }
}
