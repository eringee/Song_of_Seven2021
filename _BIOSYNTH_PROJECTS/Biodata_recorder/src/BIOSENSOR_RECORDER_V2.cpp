//#include <Arduino>
//Include all subfiles of the project
#include <Arduino.h>
#include "Global.h" //include file containing global variables

#define REVERSE_ENCODER false
#define PLOT_SENSOR false

//Encoder dependencies
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#define ENCODER_PHASE_A 5
#define ENCODER_PHASE_B 6
#define ENCODER_SWITCH 2

#if REVERSE_ENCODER == true
    Encoder myEnc(ENCODER_PHASE_B, ENCODER_PHASE_A);
#else
    Encoder myEnc(ENCODER_PHASE_A, ENCODER_PHASE_B);
#endif

long oldPosition = -999;

//temp global var. will move
int heartamp{};
int respamp{};
int heartbpm{};
int respbpm{};
float scr{};
float scl{};

#include "Helpers.h" // include file containing helpers function
#include "hardware_helpers.h"
#include "setup_Helpers.h"
//#include "OSC_Helpers.h" //include file containing helpers function regarding OSC -- DONT NEED FOR BOX
#include "update_helpers.h"
#include "recording_helpers.h"

// 0 - LEDS
// 1 - SERIAL SEND
// 2 - Encoder Button / StartStop recording 
// 3 - FootPedal Button / Place marker 
// 4 - SD_ETHERNET ADAPTER
// 5 - ENCODER A 
// 6 - ENCODER B 
// 7 - MOSI 
// 8 - SD_ETHERNET ADAPTER / DISP DO 
// 9 - SD_ETHERNET ADAPTER
// 10 - CS PIN for SD card TEENSY AUDIO - ETH
// 11 - SD_ETHERNET ADAPTER
// 12 - SD_ETHERNET ADAPTER / DISP MISO 
// 13 - SD_ETHERNET ADAPTER
// 14 - (A0) DISP_SCK 
// 15 - (A1) DISP_CS (map to pot in audio designs?)
// 16 - (A2) GSR2
// 17 - (A3) Temp
// 18 - (A4) DISP SCL
// 19 - (A5) DISP SDA
// 20 - (A6) GSR1 
// 21 - (A7) PULSE 
// 22 - Audio
// 23 - Audio

// VIN - SD_ETHERNET ADAPTER
// GND - SD_ETHERNET ADAPTER

// TO - DO :::
// SEND DATA VIA UDP @ 1000HZ IF POSSIBLE
// BEING ABLE TO RECORD MULTIPLE SESSIONS WITHOUT DISCONNECTING THE ARDUINO
//FOOT PEDAL gets stuck

void setup() {

  Serial.begin(9600);

  delay(3000); // power-up safety delay
  Serial.println("BIORECORDER");
  SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
  SPI.setSCK(14);  // Audio shield has SCK on pin 14
  
  setupAllSensors(); // restart all the sensors to initial state

  checkForCard(); //check if a SD card is inserted NO NEED TO RUN FOR PROTOTYPE
  cardInfo(); //display the informations of the inserted sd card NO NEED TO RUN FOR PROTOTYPE
 
  setupButtons(1); //setup all the buttons and set the refresh rate at 1ms
  lcdSetup(); //setup the lcd screen
  ledSetup();

  lcdUpdate.restart();
}

void loop() {

  updateButtons(); //update all the buttons state
   
  if( r.isRecording() == false){ //update encoder only when not recording
    updatePotentiometer(); //update the potentiometer value
  };

  if (r.isIdle() ) //verify if its time to stop the recording
  {
    idleDisplay();

    if ( startButton.fell())
    {
      r.startCountdown(); //start the 10 seconds countdown before the recording starts
      displayIndex = 1; //switch the idle display mode
    }

    if (r.updateCountdown() == true)
    {
      //this happens when the countdown is over
      r.stopCountdown();
      r.resetCountdown();
      r.setupRecording(); //Give permission to setup recording in the next loop
      recordingLCDIndex = 0; //make sure the recording animation start at 0
    }
  }
  else if (r.isReadyToStop() ) //verify if its time to stop the recording
  {
    if (r.endDelayNotStarted)
    {
      r.startEndDelay(); //start delay to display info on lcd
      sprintf(lcdLine1, "%s", lcdRecordingOver);
    }

    if (r.updateEndDelay() == true)
    {
      r.stopEndDelay();
      endRecordingSession(); //goes trought all the steps to end the recording
    }
  }
  else  if (r.isReadyToStart() && fileOpen == false) //verify if it can start the recording
  {
    String nameInfo = infoEmotion + " " + filename[3] + filename[4] + filename[5] + " Laurence"; //compose the subjectName header line
    r.setSubjectName(nameInfo);
    r.setSignals(signalTypes);
    setupRecording(); //goes through all the steps to setup and start the recording
  }
  else  if ( r.isRecording() ) //verify if it's in recording state
  {
    if ( startButton.fell()) //verify if the stop button was pressed
    {
      r.stopProcess = true;
    }
    recordingDisplay();
    noInterrupts(); //prevents from interrupting until interrupts() is called to transfer to the buffer
    
    if (bufferA.isFull() && readyToWrite == false) //verify if it's ready to transfer the buffer
    {
      transferBuffer(); //transfer the buffer to the temporary write buffer
    }
    interrupts(); //permits interrupts again

    if (readyToWrite == true ) //verify if write buffer is ready to be written to SD card
    {
      writeToCard(); //write the temps buffer to the SD card
    }
  }

  if(lcdUpdate.hasPassed(46))
  {  
    lcdUpdate.restart();
    updateLCD(); //update lcd display buffers
  }
  FastLED.show(); 

}
