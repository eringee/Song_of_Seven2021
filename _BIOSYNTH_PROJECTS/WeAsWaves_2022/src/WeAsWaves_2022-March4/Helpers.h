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
  sgtl5000_1.volume(0.9); //set master volume here
  sgtl5000_1.lineOutLevel(13); //set lineout to max
}
                                                                                
void setupSounds()  //initial sounds or Setting A
{
  
 //RESP dependent variables
  respTone = (sectionGlobal[0][BOARD_ID]);

  waveform2.begin(0.2, respTone, WAVEFORM_TRIANGLE);
  waveform3.begin(0.2, respTone, WAVEFORM_SAWTOOTH);  

  // set bandpass around the respTone frequencies
  respFilter.frequency(respTone); 
  respFilter.resonance(2.0); 

  noise1.amplitude(0.2);
  waveform4.begin(0.3, 15, WAVEFORM_SINE);

  envelope1.attack(10);
  envelope1.decay(10);
  envelope1.sustain(10);
  envelope1.release(50);

  envelope1delay = 60;

  waveform5.begin(0.05, 5, WAVEFORM_SINE);

  mixer3.gain(0, 0.2);
  mixer3.gain(1, 9.8);  // parameter to control the volume of the clicks.

  bitcrusher1.bits(3);  // creates the glitch-click effect
  bitcrusher1.sampleRate(44000);  // doesn't seem to affect the sound either way

  GSRfilter.resonance(3.0); // the resonator emphasises the bandpass of these clicks

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

        /// THIS IS WHERE YOU UPDATE THE SECTION SETTINGS
        
        if (currentSection==0){
        waveform2.begin(0.5, respTone, WAVEFORM_SINE);
        waveform3.begin(0.2, respTone, WAVEFORM_SINE);   
        }
        else if (currentSection==1){
          waveform2.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);
          waveform3.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);   
        }
        else if (currentSection==2){
          waveform2.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SQUARE);
          waveform3.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SQUARE);   
        }
        else if (currentSection==3){
          waveform2.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);
           waveform3.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);   
        }
        
        biosynth.setLCDState(2);
    }
}
