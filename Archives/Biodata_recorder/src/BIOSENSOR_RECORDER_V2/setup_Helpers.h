/////////////////////////////////Recording helpers//////////////////////////////////
/*
    This file contains all helpers function for the recording class. These could potentially be added to the class

    -setupRecording()
    -endRecordingSession()
    -recordingDisplay()

*/
//------------------------------------------------------------------------------------------------

void lcdSetup() {
  lcd.init();                      // initialize the lcd
  lcd.backlight();
}

//------------------------------------------------------------------------------------------------

void setupButtons(int intervalms) {
  //initialize all the buttons
  startButton.attach(ENCODER_SWITCH , INPUT_PULLUP);
  startButton.interval(intervalms);
  
  markerButton.attach(3, INPUT_PULLUP);
  markerButton.interval(intervalms);

}

//------------------------------------------------------------------------------------------------

void setupAllSensors() {
  // this function runs the necessary code to setup the sensors

  heart.reset();
  sc1.reset();
  sc2.reset();
  resp.reset();

}

//------------------------------------------------------------------------------------------------

void udpSetup() {
  //This function runs the necessary code to setup the UDP connection

  Ethernet.init(CS_ETH);    // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.begin(mac, ip);  // start the Ethernet
  detectHardware();
  detectCable();
  Udp.begin(localPort); //start UDP
}

//------------------------------------------------------------------------------------------------

void ledSetup()
    {/*!
     @function    ledSetup
     @abstract    setup the ledstrip on startup
     */  
      pinMode(LED_PIN, OUTPUT);
      FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
	    FastLED.setBrightness( BRIGHTNESS );
    

      //set the default colors of the led of the used sensor
      for ( int i = 0 ; i <NUM_LEDS ; i++ )
      {   
          if(connectedSensors[i])
          {
              leds[i].setRGB( ledColors[i][0],ledColors[i][1],ledColors[i][2]); 
          }
          else
          {
              leds[i].setRGB(0,0,0);   
          }     
      }
    }


    