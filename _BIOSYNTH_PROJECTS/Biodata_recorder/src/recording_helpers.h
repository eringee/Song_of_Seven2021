/////////////////////////////////Recording helpers//////////////////////////////////
/*
    This file contains all helpers function for the recording class. These could potentially be added to the class

    -setupRecording()
    -endRecordingSession()
    -recordingDisplay()

*/
//------------------------------------------------------------------------------------------------

void setupRecording() {
  // this function runs the necessary code to setup the recording before it starts

  //Serial.println("Open File");
  recFile = SD.open( filename , FILE_WRITE);
  fileOpen = true;

  r.startRecording();
  r.headerPrinted = false;
  //Serial.println("Start recording");
  captureData.begin( updateData, REFRESH_RATE);
  stamp = millis();
  //Serial.println("debug1");
}

//------------------------------------------------------------------------------------------------

void endRecordingSession() {
  // this function runs the necessary code to setup the recording before it starts

  //Serial.println("Writing last data to card");
  recFile.flush();

  r.recordingLength(timestamp);
  if ( r.headerPrinted == false) {

    recFile.seek(0);
    r.clearHeaderBuffer();
    recFile.println(r.formatHeader1());
    r.clearHeaderBuffer();
    recFile.println(r.formatHeader2());
    r.clearHeaderBuffer();
    recFile.println(r.formatHeader3());
    r.clearHeaderBuffer();
    recFile.println(r.formatHeader4());
    r.clearHeaderBuffer();
    recFile.println();
    recFile.println(r.channelNames());

    r.headerPrinted = true;
  }
  bufferA.clear(); //potentially unecessary
  recFile.close();
  //Serial.println("File Closed");
  filenameAvailable = false;
  fileOpen = false; //uncomment when sure not to overite
  displayIndex = 0;
  filenameNotChecked = true;
  r.readyToStartAgain();

}

//------------------------------------------------------------------------------------------------

void recordingDisplay(){
    //This function format the lcd display for the recording mode
  
    if ( recordingUpdate.hasPassed(500)) 
      {
        recordingUpdate.restart();
        recordingLCDIndex++;
      }
      
    recordingLCD[recordingLCDIndex % 4].toCharArray(lcdLine1, 17); //keep .toCharArray because it scrolls trough an array of string. need to convert in 2d char array
    sprintf(lcdLine2,"%s" , emptyLine );
}
