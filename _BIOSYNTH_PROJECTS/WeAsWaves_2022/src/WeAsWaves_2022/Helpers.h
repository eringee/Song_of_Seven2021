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

  TeensyAudio.enable();
  TeensyAudio.volume(0.9); //set master volume here
  TeensyAudio.lineOutLevel(13); //set lineout to max
}
                                                                                
void setupSounds()  //initial sounds or Setting A
{
  
 //RESP dependent variables
  respTone = (sectionGlobal[0][BOARD_ID]);

  respWave1.begin(0.1 , respTone, WAVEFORM_TRIANGLE);
  respWave2.begin(0.05, respTone, WAVEFORM_SAWTOOTH);  

  // set bandpass around the respTone frequencies
  respFilter.frequency(respTone); 
//  respFilter.resonance(1.0); 

  respnoise.amplitude(0.1);
  respnoiseLFO.begin(0.3, 15, WAVEFORM_SINE); // LFO applied to respNoise

  clickEnvelope.attack(10);
  clickEnvelope.decay(10);
  clickEnvelope.sustain(10);
  clickEnvelope.release(50);

  //envelope1delay = 60;

  heartLFO.begin(0.05, 5, WAVEFORM_SINE);

  clickGlitchMixer.gain(1, 9.8);  // parameter to control the global volume of the clicks.

  bitcrusher1.bits(3);  // creates the glitch-click effect
  bitcrusher1.sampleRate(44000);  // doesn't seem to affect the sound either way

  //GSRfilter.resonance(1.0); // the resonator emphasises the bandpass of these clicks

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
        respWave1.begin(0.5, respTone, WAVEFORM_SINE);
        respWave2.begin(0.2, respTone, WAVEFORM_SINE);   
        }
        else if (currentSection==1){
          respWave1.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);
          respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH);   
        }
        else if (currentSection==2){
          respWave1.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SQUARE);
          respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SQUARE);   
        }
        else if (currentSection==3){
          respWave1.begin(0.5, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);
           respWave2.begin(0.2, respTone, WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE);   
        }
        
        biosynth.setLCDState(2);
    }
}
