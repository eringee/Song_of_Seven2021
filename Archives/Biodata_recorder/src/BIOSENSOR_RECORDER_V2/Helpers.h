////////////////////////////////Helpers funtions//////////////////////////////////
/*
    This file contains all the misc helpers functions necessary:
    -createFilename()
    -checkFileName()
    -datalog()
    -writeToCard()
    -transferBuffer()
    -emotionSelection()
    -idleDisplay()
    
*/

//------------------------------------------------------------------------------------------------

void createFilename() 
{
  String _name = emotionFilename + fileDigits + fileExtension;
  _name.toCharArray(filename, sizeof(filename));
}
//------------------------------------------------------------------------------------------------

void checkFileName() 
{

  while ( filenameAvailable == false )
  {
    //check if file already exist
    if (SD.exists(filename)) 
    {
      // if yes, make a different name
      char endDigit = filename[5];
      char middleDigit = filename[4];
      char startDigit = filename[3];

      if ( endDigit >= 57) 
      {
        endDigit = 48;
        middleDigit = middleDigit + 1;

        if (middleDigit >= 57)
        {
          middleDigit = 48;
          startDigit = startDigit + 1;
        }
      }
      else
      {
        endDigit = endDigit + 1;
      }
      filename[5] = endDigit;
      filename[4] = middleDigit;
      filename[3] = startDigit;
    }
    else
    {
      filenameAvailable = true;
    }
  }
}

//------------------------------------------------------------------------------------------------

void datalog(float bufferArg[BUFFER_SIZE],unsigned long buffertime[BUFFER_SIZE]) 
{
  // This function formats the data and log it to the file on the sd card

  int numChan = NUM_SIGNALS ; // +1 because of the timestamp
  float formatBuffer[NUM_SIGNALS] = {0};
  unsigned long timestamp = 0 ;
  int timestampIndex{};

  if (recFile) 
  {

    for ( int i = 0 ; i < BUFFER_SIZE ; i++) 
    {
      int formatIndex = i % numChan;

      if (formatIndex == 0 ) 
      {
        timestamp = buffertime[timestampIndex];
        timestampIndex++;
        formatBuffer[formatIndex] =  bufferArg[i];
      } 
      else 
      {
        formatBuffer[formatIndex] =  bufferArg[i];
      }

      if ( formatIndex == numChan -1 ) 
      {
        recFile.println(r.formatData(timestamp, formatBuffer));
      }
    }

    recFile.flush();
  } 
  else 
  {
    //Serial.println("cant write to file");
  }
}

//------------------------------------------------------------------------------------------------

void writeToCard() 
{
  // write the data to the sd card

  datalog(writeBuffer, writeBufferTime);

  if (r.stopProcess) 
  {
    captureData.end();
    bufferA.clear();
    bufferTime.clear();
    r.stopRecording();
  }

  readyToWrite = false;
}

//------------------------------------------------------------------------------------------------

void transferBuffer()
{
  //transfer bufferA to write buffer

  for ( int i = 0 ; i < BUFFER_SIZE ; i++ ) 
  {
    writeBuffer[i] = bufferA[i];
    writeBufferTime[i] = bufferTime[i];
  }
  bufferA.clear(); //clear the buffer so it is not full
  bufferTime.clear();
  readyToWrite = true; //tells the board we are ready to write
}

//------------------------------------------------------------------------------------------------

int emotionSelection( int potValue) 
{
  int emotionIndex = constrain(potValue, 0 , NUM_EMOTIONS - 1) ;
  return emotionIndex;
}

//------------------------------------------------------------------------------------------------

void idleDisplay()
{

  if (displayIndex == 0) 
  {
    int emotionIndex = potVal;
    emotionFilename = emotionsFile[emotionIndex];  //select the filename prefix based on the emotionIndex
    selectedEmotion = emotions[emotionIndex]; //select the string to display on the lcd based on the emotionIndex
    infoEmotion = emotionsName[emotionIndex]; //select the string to put in the header based on the emotionIndex

    //update lcd buffers
    sprintf(lcdLine1,"%s",idleLine1);
    selectedEmotion.toCharArray(lcdLine2, 16);
  }
  else if ( displayIndex == 1 )
  {
    sprintf(lcdLine1, "Start in %-6d", r.ctdwnIndex); //compose message and update lcd buffer

    if (filenameNotChecked == true)
    {
      //create and check filename based on selected emotion
      filenameNotChecked = false;
      createFilename();
      checkFileName();
    }
    sprintf(lcdLine2, "File: %s", filename); //Compose the string that's gonna show on the second line of the lcd
  }
}
//------------------------------------------------------------------------------------------------

void setLedBrightness(int led , float brightness)
{/*!
  @function    setLedBrightness
  @abstract    set the brightness of the given LED
  @param  led  index of the led to change the brightness
  @param  brightness  value of the brightness to set (0.0-1.0)
  */
    leds[led].setRGB(ledColors[led][0]*brightness,ledColors[led][1]*brightness,ledColors[led][2]*brightness);
}

//------------------------------------------------------------------------------------------------

void setEncoder( int value)
{/*!
  @function    setEncoder
  @abstract    set the encoder the the passed value
  @param    value    integer to set the encoder to
  */
    myEnc.write(value * 4); //multiply by 4 because it is a quadrature encoder
}