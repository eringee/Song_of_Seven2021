/*
  This file contain all the code to make the user Biosynth work
  It handles the encoder, the buttons, the screen, the Leds, the sensors, and the audio mapping
*/

#define PLOT_SENSOR  false //Set to true to print sensor value in the serial plotter
#define FOOT_PEDAL false //set to true if using the foot pedal in the project
#define REVERSE_ENCODER true

//LCD SCREEN STUFF///////////////
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16 , 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

//ENCODER AND FOOTPEDAL STUFF/////////////
#define ENCODER_PHASE_A 5
#define ENCODER_PHASE_B 6
#define ENCODER_SWITCH 2
#define FOOT_PEDAL_PIN 3
#define BUTTON_REFRESH_RATE 1

#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

#if REVERSE_ENCODER == true
Encoder myEnc(ENCODER_PHASE_B, ENCODER_PHASE_A);
#else
Encoder myEnc(ENCODER_PHASE_A, ENCODER_PHASE_B);
#endif

#include <Bounce2.h>
Bounce encoderButton = Bounce();
Bounce footPedal = Bounce();

////////////LED STUFF//////////////////
#define NUM_LEDS 4
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 24

#include <WS2812Serial.h>
byte drawing_memory[NUM_LEDS*3];         //  3 bytes per LED
DMAMEM byte display_memory[NUM_LEDS*12]; // 12 bytes per LED
#define LED_PIN 31 // pin on bottom of Teensy writes data from serial. Attached to pin 0 via hardwire.
WS2812Serial leds(NUM_LEDS, display_memory, drawing_memory, LED_PIN, WS2812_GRB);

///////////BIODATA LIBRARY STUFF////////
#include <Respiration.h>
#include <SkinConductance.h>
#include <Heart.h>

//BIO SYNTH SENSOR PINS

#define HEART_SENSOR_PIN A7
#define GSR1_PIN A6
#define RESP_SENSOR_PIN A3
#define GSR2_PIN A2

Heart heart(HEART_SENSOR_PIN);
SkinConductance sc1(GSR1_PIN);
Respiration resp(RESP_SENSOR_PIN);
SkinConductance sc2(GSR2_PIN);

///Variables for smoothing out biosignals
float smoothHeart = 0.5; //default value for smoothing heart signal for EMA
float smoothHeartAmp = 0.5; //default value for smoothing heart signal for EMA
float smoothHeartBPM = 0.5; //default value for smoothing heart signal for EMA

float smoothGSR = 0.1;   //default value for smoothing sc1 signal for EMA

float smoothResp = 0.5;  //default value for smoothing resp signal
float smoothResp2 = 0.5;  //default value for smoothing resp signal
float smoothResp3 = 0.5;  //default value for smoothing resp signal
float smoothRespAmp = 0.5;  //default value for smoothing resp signal
float smoothRespBPM = 0.5;  //default value for smoothing resp signal

//variables for storing last sensor reading
float heartSig;
float heartAmp;
float heartBPM;  
float GSRsig;
float GSRlowpass;
float respSig;
float respAmp;
float respBPM;
float smoothGSRreduced;

int GSRfilt; // value for storing filter data relative to GSR

///EXTRA AUDIO LIBRARY VARIABLES//////////////

float respAmpFun;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Biosynth
{
  private:
    Chrono biosynthSensorTimer; // interval timer for how often to sample biosignals
    
    WS2812Serial* leds_ptr{nullptr};

    //Here you can modify the RGB Values to determine the colors of the leds 0=HEART 1=GSR1 2=RESP 3= GSR2
    int ledColors [NUM_LEDS][3] = {{250, 0, 250}, {100, 255, 250}, {10, 48, 240}, {0, 0, 0}};
  
    //Variables related to the LCD screen, normally you shouldn't need to touch these
    int lcdState = 0;
    long currentEncoderValue = 0;
    
    Chrono  lcdUpdate;
    int lcdInterval = 40;
    char sections[NUM_SECTIONS][2] = {"A", "B", "C", "D"};  //Use numbers or letters to describe sections
    
    //buffers use one more char than the screen can display for a null terminator
    char lcdLine1Buffer[17];
    char lcdLine2Buffer[17];

    //variables for Section change confirmation
    bool confirmBlink = false;
    Chrono  confirmTimer; //timer used to reset lcd state if section change not confirmed 
    Chrono confirmBlinkTimer; //timing the blinking state of LSC screen
    int  confirmBlinkInterval = 500;


    //-----------------LCD------------------//
    void lcdSetup()
    { /*!
        @function    lcdSetup
        @abstract    setting up the lcd on startup
      */
      lcd.init();
      lcd.backlight();
      confirmBlinkTimer.restart();
      confirmTimer.restart();
      confirmTimer.stop();

    }
    //---------------
    void updateLCD()
    { /*!
        @function    updateLCD
        @abstract    display the good message on the lcd every frame depending on its state
      */

      //select the good message to display and update buffers

      if (currentSection != lastSection && updateLCDBool == true)
      {
        switch (lcdState)
        {
          case 1:
            sectionConfirmMessage();
            lcd.setCursor(0, 0);
            lcd.print(lcdLine1Buffer);
            lcd.setCursor(0, 1);
            lcd.print(lcdLine2Buffer);
            break;
          case 2:
            currentSectionMessage();
            lcd.setCursor(0, 0);
            lcd.print(lcdLine1Buffer);
            lcd.setCursor(0, 1);
            lcd.print(lcdLine2Buffer);
        }
      }
    }

    //---------------
    void verifyNoTouch()
    { /*!
        @function    verifyNoTouch
        @abstract    Verify if the encoder has not been touched in changeSection state
                   and set back the lcd to the current section state
      */
      if (confirmTimer.hasPassed(NO_TOUCH_DELAY) && getLCDState() == 1 )
      {
        setEncoder(currentSection);
        setLCDState(2);
        updateLCDBool = true;
        confirmTimer.restart();
        confirmTimer.stop();
      }
    }
    //---------------
    void sectionChange()
    { /*!
        @function    sectionChange
        @abstract    Verify if the program needs to put the lcd in sectionChange mode
      */
      if ( getEncoderValue() != currentSection && getLCDState() == 2)
      {
        updateLCDBool = true;
        setLCDState(1);
        confirmTimer.start();
      }
    }

    //---------------
    void sectionConfirmMessage()
    { /*!
        @function    sectionConfirmMessage
        @abstract    update the lcd buffers section change message
      */
      sprintf(lcdLine1Buffer, "   Setting : %s", sections[getEncoderValue()]);
      sprintf(lcdLine2Buffer, "   Confirm ?   ");

      updateLCDBool = false;
    }
    //---------------
    void currentSectionMessage()
    { /*!
        @function    currentSectionMessage
        @abstract    update the lcd buffers to display the current song section
      */
      sprintf(lcdLine1Buffer, "   Setting  %s    ", sections[currentSection]);
      sprintf(lcdLine2Buffer, "                ");
      updateLCDBool = false;
    }
    //---------------
    void buttonSetup()
    { /*!
        @function    buttonSetup
        @abstract    setup the buttons on startup
      */
      encoderButton.attach(ENCODER_SWITCH, INPUT_PULLUP);
      encoderButton.interval(BUTTON_REFRESH_RATE);
      //encoderButton.update();
      //encoderButton.read();

      footPedal.attach(FOOT_PEDAL_PIN, INPUT_PULLUP);
      footPedal.interval(BUTTON_REFRESH_RATE);
    }
    //---------------
    void updateButtons()
    { /*!
        @function    updateButtons
        @abstract    update the state of the buttons every loop
      */
      encoderButton.update();
      //Serial.println(encoderButton.read());
      footPedal.update();
      //Serial.println(footPedal.read());
    }
    //---------------
    void updateEncoder()
    { /*!
        @function    updateEncoder
        @abstract    update the state of the encoder every loop
      */

      long newPosition = myEnc.read() / 4; //divided by 4 because its a quadrature encore

      //prevents the encoder from going out of bounds
      if (newPosition < 0)
      {
        setEncoder(0);
        newPosition = myEnc.read() / 4;
      }
      else if ( newPosition > NUM_SECTIONS - 1)
      {
        setEncoder(NUM_SECTIONS - 1);
        newPosition = myEnc.read() / 4;
      }

      if (newPosition != currentEncoderValue) //encoder has been moved
      {
        lastSection = -1;
        updateLCDBool = true;
        currentEncoderValue = constrain(newPosition, 0, NUM_SECTIONS - 1);
      }
    }
    //---------------
    void ledSetup()
    { /*!
        @function    ledSetup
        @abstract    setup the ledstrip on startup
      */
      pinMode(LED_PIN, OUTPUT);
      leds_ptr -> begin();
      
      //set the default colors of the led of the used sensor
      for ( int i = 0 ; i < NUM_LEDS ; i++ ){
          leds_ptr->setPixel(i,0,0,0);
      }
    }
    
    //---------------
    void setLedBrightness(int led, float brightness)
    { /*!
        @function    setLedBrightness
        @abstract    set the brightness of an individual LED
        @param  led  index of the led to change the brightness
        @param  brightness  value of the brightness to set (0.0-1.0)
      */
      leds_ptr->setPixel(led,ledColors[led][0]*brightness,ledColors[led][1]*brightness,ledColors[led][2]*brightness );
    }
    
    //---------------
    void setupSensors()
    { /*!
        @function    setupSensors
        @abstract    setup the sensors on startup
      */

              pinMode(HEART_SENSOR_PIN, INPUT);
              heart.reset();
  
              pinMode(GSR1_PIN, INPUT);
              sc1.reset();

              pinMode(RESP_SENSOR_PIN, INPUT);
              resp.reset();
    }
    //---------------

  void updateSensors()
  { /*!
    @function    updateSensors
    @abstract    sample the value of the used sensor according to biosynthUpdateTimer.
  */
    heart.update();
    sc1.update();
    resp.update();

    //Retrieve sensor values
    heartSig = heart.getNormalized();
    heartAmp = heart.amplitudeChange();
    heartBPM = heart.bpmChange();
      
    GSRsig = sc1.getSCR();
      
    respSig = resp.getNormalized();
    respAmp = resp.amplitudeChange();
    respBPM = resp.bpmChange();
    
    //Run your Serial print commands here
    Serial.println(heart.getRaw());
  }
  
    //------------------

    void updateVolume()
    { /*!
        @function    updateVolume
        @abstract    update main volume of the device through volume potentiometer
      */    
      float vol = analogRead(VOL_POT_PIN);
      vol = (vol / 1024) * 0.95; //make sure the gain doesn't go louder than 0.6 to avoid clipping

    
      for ( int i = 0 ; i <= 3 ; i++ ) {  //set all four channels of main mixer to follow gain knob
        mainMixer.gain(i, vol);    
        if (i == 0) mainMixer.gain(i, vol*smoothGSRreduced*1.5); //clamp down heartbeat volume a bit when GSR is low 
        if (i == 3) mainMixer.gain(i, vol/10);  //just a little distortion for coloring the respiration data
      }
    }
      
    void updateSoundsLights()
    { /*!
        @function    update and smooth sound and light ouput
        @abstract    To link sensors with audio, add interaction here. 
                     Smooths out the sensor data every loop.
      */
      
      ////////////////////////smooth signals       
      smoothHeart += 0.005 * (heartSig - smoothHeart);
    
      smoothGSR += 0.8 * (GSRsig - smoothGSR); // run EMA filter
      smoothGSRreduced = max((smoothGSR-0.1), 0);  //subtract a bit from GSR value but don't pass 0  
 
      smoothResp += 0.001 * (respSig - smoothResp);
      smoothResp2 += 0.0001 * (respSig - smoothResp2);
      smoothResp3 += 0.00005 * (respSig - smoothResp3);
      
      float finalResp = max((smoothResp-0.25), 0);
     
      if (GSRfilt == 0)
        amp1.gain(0);
      else amp1.gain(0.1+(smoothHeart/2)); //
      
      GSRfilt = GSRsig*100;
      int y = map(GSRfilt, 1, 100, 100, 10000);
      GSRfilter.frequency(y);

      respAmpFun = (respBPM*10)-0.5;

      waveform3.frequency(respAmpFun+respTone);
      waveform4.frequency(respAmp*100);  
      for (int x=0; x <=3; x++) {
        respMixer.gain(x, max((finalResp-0.1), 0));
        if(x==2) respMixer.gain(x, smoothResp2*0.25);
      }
      
      amp2.gain(smoothResp3);
      
      respFilter.frequency(respTone*(respAmp)); // moving bandpass around the respTone

      if (heart.beatDetected()){  
        envelope1.noteOn();
      }
      
      waveform5.frequency(heartBPM*8);  //the frequency of the heartsignal LFO
      
      
        
      //AudioInterrupts();///////////////////////////////////////////////////////////////////
       
      // set brighness of LEDs with smoothed signals
      setLedBrightness(0, smoothHeart*0.7); //making this signal a bit dimmer because red is naturally powerful
      setLedBrightness(1, smoothGSRreduced);
      setLedBrightness(2, finalResp*1.2);  // making this signal brighter because blue is naturally weaker
      setLedBrightness(3, 0);
}


/************************************************************************/
/************************************************************************/
public:
    Biosynth(WS2812Serial* _led_ptr):leds_ptr{_led_ptr}
    {
    }
//-----------------------
void setup()
{ /*!
    @function    setup
    @abstract    wrapper function running all the hardware setup routines
  */
  lcdSetup();
  buttonSetup();
  ledSetup();
  setupSensors();
  biosynthSensorTimer.start();
}
//---------------
void update()
{ /*!
    @function    update
    @abstract    wrapper function running all the hardware update routines and the necessary verifications
  */

  updateSoundsLights();
  updateButtons();
  updateEncoder();
  updateVolume();
  
  if (biosynthSensorTimer.hasPassed(40)) {  //force a reasonable delay between readings
    updateSensors(); 
    biosynthSensorTimer.start();
  }

  //lcd state verifications
  sectionChange();
  verifyNoTouch();

  if ( lcdUpdate.hasPassed(lcdInterval))
  {
    lcdUpdate.restart();
    updateLCD(); //update lcd display buffers
    leds_ptr->show(); // update LED buffers
  }

}
//---------------
int getEncoderValue()
{ /*!
    @function    getEncoderValue
    @abstract    return the current encoder Value
  */
  return currentEncoderValue;
}
//---------------
void setEncoder( int value)
{ /*!
    @function    setEncoder
    @abstract    set the encoder the the passed value
    @param    value    integer to set the encoder to
  */
  myEnc.write(value * 4); //multiply by 4 because it is a quadrature encoder
}
//---------------
void setLCDState( int state )
{ /*!
    @function    setLCDState
    @abstract    set the lcd to the given state
    @param    state    integer representing the state of the lcd
  */
  lcdState = state;
}
//---------------
int getLCDState()
{ /*!
    @function    getLCDState
    @abstract    return an integer representing the lcd state
  */
  return lcdState;
}
//---------------
void openingMessage()
{ /*!
    @function    openingMessage
    @abstract    update the lcd buffers with the greeting message
  */
  sprintf(lcdLine1Buffer, "  Wave Testing.");
  sprintf(lcdLine2Buffer, "I am board #%d", BOARD_ID+1);
  lcd.setCursor(0, 0);
  lcd.print(lcdLine1Buffer);
  lcd.setCursor(0, 1);
  lcd.print(lcdLine2Buffer);
}

};
