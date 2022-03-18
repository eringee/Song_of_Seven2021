/*
This file contain all the code to make the user Biosynth work
It handles the encoder, the buttons,  the screen and the Leds 
*/

#define PLOT_SENSOR  false //Set to true to print sensor value in the serial plotter
#define FOOT_PEDAL false //set to true if using the foot pedal in the project


//BIO SYNTH HARDWARE PINS


#define FOOT_PEDAL_PIN 3


//#define FASTLED_INTERNAL //turn off build messages
#include <FastLED.h>

class Interface
{
private:
  
    bool connectedSensors[4] = {};
    

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
                        pinMode(RESP_SENSOR_PIN,INPUT);
                        resp.reset();
                    }
                    break;
                case 3:
                    if(connectedSensors[i])
                    {
                        pinMode(GSR2_PIN,INPUT);
                        sc2.reset();
                    }
                    break;    
            }
        }
    }
//---------------
//can probably be removed   
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
                            heartbpm = heart.getBPM();
                            heartamp=heart.amplitudeChange()
                            setLedBrightness(i , sensorData[i]);
                        }
                        break;

                    case 1:
                        if(connectedSensors[i])
                        {
                            sc1.update();
                           // sensorData[i] =sc1.getSCR();
                            scr = sc1.getSCR();
                            scl = sc1.getSCL();
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
                            resp.update();
                            sensorData[i] = resp.getNormalized();
                            respbpm = resp.getbpm();
                            respamp = resp.amplitudeChange()
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
                            //not used in this case
                            sc2.update();
                            sensorData[i] = sc2.getSCR();
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

    Biosynth(bool heart , bool gsr1 , bool resp , bool gsr2)
    {
        connectedSensors[0] = heart;
        connectedSensors[1] = gsr1;
        connectedSensors[2] = resp;
        connectedSensors[3] = gsr2;

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
   /*! void openingMessage()
    {
     @function    openingMessage
     @abstract    update the lcd buffers with the greeting message
     
        sprintf(lcdLine1Buffer, "Hello!");
        sprintf(lcdLine2Buffer, "I am board #%d",BOARD_ID);
        lcd.setCursor(0, 0);
        lcd.print(lcdLine1Buffer);
        lcd.setCursor(0, 1);
        lcd.print(lcdLine2Buffer);
    }  */
};
