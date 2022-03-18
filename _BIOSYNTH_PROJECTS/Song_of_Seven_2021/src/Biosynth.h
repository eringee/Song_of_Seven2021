/*
This file contain all the code to make the user Biosynth work
It handles the encoder, the buttons,  the screen and the Leds 
*/

#define PLOT_SENSOR  false //Set to true to print sensor value in the serial plotter
#define FOOT_PEDAL true //set to true if using the foot pedal in the project
#define REVERSE_ENCODER true

//BIO SYNTH HARDWARE PINS
#define LED_PIN 31 
#define HEART_SENSOR_PIN A7
#define GSR1_PIN A6
#define GSR2_PIN A2
#define RESP_SENSOR_PIN A3

#define ENCODER_PHASE_A 5
#define ENCODER_PHASE_B 6
#define ENCODER_SWITCH 2
#define FOOT_PEDAL_PIN 3


////////////LED STUFF//////////////////////
#define NUM_LEDS 4
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 16 

#define BUTTON_REFRESH_RATE 1

#include <WS2812Serial.h>

#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>

#if REVERSE_ENCODER == true
    Encoder myEnc(ENCODER_PHASE_B, ENCODER_PHASE_A);
#else
    Encoder myEnc(ENCODER_PHASE_A, ENCODER_PHASE_B);
#endif

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16 , 2); // set the LCD address to 0x27 for a 16 chars and 2 line display


#include <Respiration.h>
#include <SkinConductance.h>
#include <Heart.h>
#define HEART_SAMPLE 5

Heart heart(HEART_SENSOR_PIN);
SkinConductance sc1(GSR1_PIN);
SkinConductance sc2(GSR2_PIN);
Respiration resp(RESP_SENSOR_PIN);

#include <Bounce2.h>
Bounce encoderButton = Bounce();
Bounce footPedal = Bounce();

static float smoothHeart = 0.5; //default value for smoothing out heart signal for EMA
static float smoothGSR = 0.5;   //default value for smoothing out sc1 signal for EMA
float heartSig;
float GSRsig;


byte drawing_memory[NUM_LEDS*3];         //  3 bytes per LED
DMAMEM byte display_memory[NUM_LEDS*12]; // 12 bytes per LED
WS2812Serial leds(NUM_LEDS, display_memory, drawing_memory, LED_PIN, WS2812_GRB);

  
class Biosynth
{
private:

    WS2812Serial* leds_ptr{nullptr};



    //Here you can modify the RGB Values to determine the colors of the leds 0=HEART 1=GSR1 2=RESP 3=GSR2
    
    int ledColors [NUM_LEDS][3] = {{250, 0, 250},{100, 255, 250},{10, 48, 240},{140, 10, 240}};

    int lcdState = 0;
    long currentEncoderValue = 0;
    
    Chrono  confirmTimer; //timer used to reset lcd state if section change not confirmed 
    Chrono  lcdUpdate;
    int lcdInterval = 40;
    //buffers use one more char than the screen can display for a null terminator
    char lcdLine1Buffer[17];
    char lcdLine2Buffer[17];

    char sections[NUM_SECTIONS][2] = {"A","B","C","D"};
    bool connectedSensors[4] = {};
    
    bool confirmBlink = false;
    Chrono confirmBlinkTimer;
    int  confirmBlinkInterval = 500;

    Chrono biosynthSensorTimer;
    

//-----------------LCD------------------//
    void lcdSetup()
    {/*!
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
    {/*!
     @function    updateLCD
     @abstract    display the good message on the lcd every frame depending on its state
     */

        //select the good message to display and update buffers

        if(currentSection != lastSection && updateLCDBool == true)
        {

            switch(lcdState)
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
    {/*!
     @function    verifyNoTouch
     @abstract    Verify if the encoder has not been touched in changeSection state 
                  and set back the lcd to the current section state
     */
        if(confirmTimer.hasPassed(NO_TOUCH_DELAY) && getLCDState() == 1 )
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
    {/*!
     @function    sectionChange
     @abstract    Verify if the program needs to put the lcd in sectionChange mode
     */
        if( getEncoderValue() != currentSection && getLCDState() == 2)
        {      
            updateLCDBool = true;      
            setLCDState(1);
            confirmTimer.start();
        }
    }
 
//---------------
    void sectionConfirmMessage()
    {/*!
     @function    sectionConfirmMessage
     @abstract    update the lcd buffers section change message
     */
        sprintf(lcdLine1Buffer, "  Section : %s",sections[getEncoderValue()]);
        sprintf(lcdLine2Buffer, "   Confirm ?   ");

        updateLCDBool = false;    
    }        
//---------------
    void currentSectionMessage()
    {/*!
     @function    currentSectionMessage
     @abstract    update the lcd buffers to display the current song section
     */
        sprintf(lcdLine1Buffer, "  Section  %s    ",sections[currentSection]);
        sprintf(lcdLine2Buffer, "                ");
        updateLCDBool = false;  
    }
//---------------
    void buttonSetup()
    {/*!
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
    {/*!
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
    {/*!
     @function    updateEncoder
     @abstract    update the state of the encoder every loop
     */

        long newPosition = myEnc.read()/4; //divided by 4 because its a quadrature encore
        
        //prevents the encoder from going out of bounds
        if(newPosition < 0)
        {   
                setEncoder(0); 
                newPosition = myEnc.read()/4;
        }
        else if( newPosition > NUM_SECTIONS-1)
        {   
                setEncoder(NUM_SECTIONS -1);
                newPosition = myEnc.read()/4;
        }

        if (newPosition != currentEncoderValue) //encoder has been moved  
        {            
                    lastSection = -1;
                    updateLCDBool = true;
                    currentEncoderValue = constrain(newPosition,0,NUM_SECTIONS-1);          
        }
    }
//---------------
    void ledSetup()
    {/*!
     @function    ledSetup
     @abstract    setup the ledstrip on startup
     */  
        pinMode(LED_PIN, OUTPUT);
        leds_ptr -> begin();

       
        //set the default colors of the led 
        for ( int i = 0 ; i <NUM_LEDS ; i++ )
        {   
            
                leds_ptr->setPixel(i,0,0,0);
              
        }
    }
//---------------
    void setLedBrightness(int led , float brightness)
    {/*!
     @function    setLedBrightness
     @abstract    set the brightness of the given LED
     @param  led  index of the led to change the brightness
     @param  brightness  value of the brightness to set (0.0-1.0)
     */
        leds_ptr->setPixel(led,ledColors[led][0]*brightness,ledColors[led][1]*brightness,ledColors[led][2]*brightness );
    }
//---------------    
    void setupSensors()
    {/*!
     @function    setupSensors
     @abstract    setup the used sensor on startup
     */   

     pinMode(HEART_SENSOR_PIN,INPUT);
     heart.reset();

     pinMode(GSR1_PIN,INPUT);
     sc1.reset();

    }
//---------------   
    void updateSensors() 
    {/*!
     @function    updateSensors
     @abstract    sample the value of the used sensors  
     */ 
    heart.update();
    sc1.update();
    Serial.print(GSRsig);
    Serial.print("\t");
    Serial.println(smoothGSR); 
    }
    
     void updateSoundsLights()
     {/*!
     @function updateSoundsLights
     @abstract    to link sensors with audio, add interaction here. Smooth signals.
     */

     //read volume of gain knob on device
        
        float vol = analogRead(VOL_POT_PIN);
        vol = (vol/1024)*0.8; //make sure the gain doesn't go louder than 0.8 to avoid clipping
        
        for( int i = 0 ; i < 4 ; i++ )
            {    
                mixerMain.gain(i, vol);  //set all four channels of main mixer to follow gain knob
            }    
        //retrieve signals   
        heartSig = heart.getNormalized();
        GSRsig = (sc1.getSCR());

        //smooth signals
        smoothGSR += 0.5 * (GSRsig - smoothGSR);
        
  
        smoothHeart += 0.1 * (heartSig - smoothHeart);

  

        //used smoothed signals to transform audio
        amp1.gain(smoothGSR); //
        amp2.gain(smoothHeart);

        //set brightness of LEDs with smoothed signals  
        setLedBrightness(0 , smoothHeart);
        setLedBrightness(1 , smoothGSR);                    
        setLedBrightness(2 , 0);
        setLedBrightness(3 , 0);

       /* if(PLOT_SENSOR) //used for debugging purpose
        {
        Serial.printf("%.2f,%.2f,%.2f,%.2f",sensorData[0],sensorData[1],sensorData[2],sensorData[3] );
        Serial.println();
        }   */ 
    }


/************************************************************************/
public:

    Biosynth(WS2812Serial* _led_ptr):leds_ptr{_led_ptr}
    {

    }
//---------------
    void setup()
    {/*!
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

        if (biosynthSensorTimer.hasPassed(10)) {  //this should only update once in a while
          updateSensors(); 
          biosynthSensorTimer.start();
        }

        //lcd state verifications
        sectionChange();
        verifyNoTouch();

        if( lcdUpdate.hasPassed(lcdInterval))
        {   
            lcdUpdate.restart();
            updateLCD(); //update lcd display buffers  
            leds_ptr->show();
        }

    }
//---------------
    int getEncoderValue()
    {/*!
     @function    getEncoderValue
     @abstract    return the current encoder Value
     */
        return currentEncoderValue;
    }
//---------------
    void setEncoder( int value)
    {/*!
     @function    setEncoder
     @abstract    set the encoder the the passed value
     @param    value    integer to set the encoder to
     */
        myEnc.write(value * 4); //multiply by 4 because it is a quadrature encoder
    }
   //--------------- 
    void setLCDState( int state )
    {/*!
     @function    setLCDState
     @abstract    set the lcd to the given state
     @param    state    integer representing the state of the lcd
     */
        lcdState = state;
    }
//---------------
    int getLCDState()
    {/*!
     @function    getLCDState
     @abstract    return an integer representing the lcd state
     */
        return lcdState;
    }
    //---------------
    void openingMessage()
    {/*!
     @function    openingMessage
     @abstract    update the lcd buffers with the greeting message
     */
        sprintf(lcdLine1Buffer, "Hello!");
        sprintf(lcdLine2Buffer, "I am board #%d",BOARD_ID);
        lcd.setCursor(0, 0);
        lcd.print(lcdLine1Buffer);
        lcd.setCursor(0, 1);
        lcd.print(lcdLine2Buffer);
    }  
};
