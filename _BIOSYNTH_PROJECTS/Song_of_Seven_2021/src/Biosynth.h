/*
This file contain all the code to make the user Biosynth work
It handles the encoder, the buttons,  the screen and the Leds 
*/

#define PLOT_SENSOR  false //Set to true to print sensor value in the serial plotter
#define FOOT_PEDAL false //set to true if using the foot pedal in the project
#define REVERSE_ENCODER true

//BIO SYNTH HARDWARE PINS
#define LED_PIN 0
#define HEART_SENSOR_PIN A7
#define GSR1_PIN A6
#define GSR2_PIN A2
#define RESP_SENSOR_PIN A3

#define ENCODER_PHASE_A 5
#define ENCODER_PHASE_B 6
#define ENCODER_SWITCH 2
#define FOOT_PEDAL_PIN 3

#define NUM_LEDS 4
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 16 

#define BUTTON_REFRESH_RATE 1

//#define FASTLED_INTERNAL //turn off build messages
#include <FastLED.h>
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

class Biosynth
{
private:
    

    CRGB leds[NUM_LEDS];  //array holding led colors data

    //Here you can modify the RGB Values to determine the colors of the leds 0=HEART 1=GSR1 2=RESP 3=
    int ledColors [NUM_LEDS][3] = {{252, 28, 3},{252, 186, 3},{10, 48, 240},{140, 10, 240}};

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
        pinMode(ENCODER_SWITCH , INPUT_PULLUP);
        encoderButton.attach(ENCODER_SWITCH);
        encoderButton.interval(BUTTON_REFRESH_RATE);
        //encoderButton.update();
        //encoderButton.read();
        
                    pinMode( FOOT_PEDAL_PIN , INPUT_PULLUP);
            footPedal.attach(FOOT_PEDAL_PIN);
            footPedal.interval(BUTTON_REFRESH_RATE);
        if(FOOT_PEDAL)
        {   

        }
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
        if(FOOT_PEDAL)
        {
           
        }
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
//---------------
    void setLedBrightness(int led , float brightness)
    {/*!
     @function    setLedBrightness
     @abstract    set the brightness of the given LED
     @param  led  index of the led to change the brightness
     @param  brightness  value of the brightness to set (0.0-1.0)
     */
        leds[led].setRGB(ledColors[led][0]*brightness,ledColors[led][1]*brightness,ledColors[led][2]*brightness);
    }
//---------------    
    void setupSensors()
    {/*!
     @function    setupSensors
     @abstract    setup the used sensor on startup
     */   
        for( int i = 0 ; i < 4 ; i++ )
        {
            switch(i)
            {
                case 0:
                    if(connectedSensors[i])
                    {
                        pinMode(HEART_SENSOR_PIN,INPUT);
                        heart.reset();
                    }
                        break;
                case 1:
                    if(connectedSensors[i])
                    {
                        pinMode(GSR1_PIN,INPUT);
                        sc1.reset();
                    }
                    break;    
                case 2:
                    if(connectedSensors[i])
                    {
                        pinMode(GSR2_PIN,INPUT);
                        sc2.reset();
                    }
                    break;
                case 3:
                    if(connectedSensors[i])
                    {
                        pinMode(RESP_SENSOR_PIN,INPUT);
                        resp.reset();
                    }
                    break;    
            }
        }
    }
//---------------   
    void updateSensors() 
    {/*!
     @function    updateSensors
     @abstract    sample the value of the used sensor every loop. To link sensors with audio, add interaction here
     */  
        float sensorData[4] = {0.0,0.0,0.0,0.0}; 

        for( int i = 0 ; i < 4 ; i++ )
            {    
                switch(i)
                {
                    case 0:
                        if(connectedSensors[i])
                        {   
                            heart.update();
                            sensorData[i] = heart.getNormalized();
                            setLedBrightness(i , sensorData[i]);
                            waveform3.amplitude((float)sensorData[i]/2);

                        }
                        break;

                    case 1:
                        if(connectedSensors[i])
                        {

                            sc1.update();
                            sensorData[i] = sc1.getRaw();
                            Serial.println(sensorData[i]);
                            sine_fm2.amplitude((float)sensorData[i]/1024 -0.2); //clamp the sensorData down a bit to avoid clipping

                            setLedBrightness(i , sensorData[i]);
                        }
                        else
                        {
                            setLedBrightness(i , 0);
                        }
                        break;
                    case 2:
                        if(connectedSensors[i])
                        {
                            sc2.update();
                            sensorData[i] = sc2.getSCR();
                            setLedBrightness(i , sensorData[i]);
                        }
                        else
                        {
                            setLedBrightness(i , 0);
                        }
                        break;
                    case 3:
                        if(connectedSensors[i])
                        {
                            resp.update();
                            sensorData[i] = resp.getNormalized();
                            setLedBrightness(i , sensorData[i]);
                        }
                        else
                        {
                            setLedBrightness(i , 0);
                        }
                        break;    
                }
            }

        if(PLOT_SENSOR) //used for debugging purpose
        {
        Serial.printf("%.2f,%.2f,%.2f,%.2f",sensorData[0],sensorData[1],sensorData[2],sensorData[3] );
        Serial.println();
        }    
    }


/************************************************************************/
public:

    Biosynth(bool heart , bool gsr1 , bool gsr2 , bool resp)
    {
        connectedSensors[0] = heart;
        connectedSensors[1] = gsr1;
        connectedSensors[2] = gsr2;
        connectedSensors[3] = resp;

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
    }
//---------------
    void update()
    { /*!
     @function    update
     @abstract    wrapper function running all the hardware update routines and the necessary verifications
     */  
        updateSensors();
        updateButtons();
        updateEncoder();

        //lcd state verifications
        sectionChange();
        verifyNoTouch();

        if( lcdUpdate.hasPassed(lcdInterval))
        {   
            lcdUpdate.restart();
            updateLCD(); //update lcd display buffers  
            FastLED.show(); 
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
