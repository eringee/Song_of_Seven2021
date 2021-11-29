/////////////////////////////////GLOBAL VARIABLES//////////////////////////////////
/*
    This file contains all the #include statements, all the define statements
    and all the global variable definition

*/

//----------------------------------DEFINE STATEMENTS-------------------------------//

#define CS_ETH 4
   
#define BUFFER_SIZE 768 //set the buffer size here. it needs the be a multiple of the number of columns ex: timestamps, marker, heart, gsr, resp -- 5 columns so the buffer is 640
#define NUM_SIGNALS 8 //HEART HEARTAMP HEARTBPM SCR SCL RESP RESPAMP RESPBPM  //Set number of signal recorded here 

#define START_BUTTON_PIN 2
#define MARKER_BUTTON_PIN 3

#define NUM_EMOTIONS  7
#define REFRESH_RATE 5000 //IN MICROSECOND DIVIDE 1000000 BY THE REFRESH RATE IN HZ ( EXEMPLE FOR 200HZ 1000000/200 = 5000)

#define LED_PIN 0
#define NUM_LEDS 4
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 16 

//------------ADDING LIBRARIES-------------------------------//
#include <FastLED.h>
CRGB leds[NUM_LEDS];  //array holding led colors data

//Here you can modify the RGB Values to determine the colors of the leds 0=HEART 1=GSR1 2=RESP 3=GSR2
int ledColors [NUM_LEDS][3] = {{252, 28, 3},{252, 186, 3},{10, 48, 240},{140, 10, 240}};
#include <LiquidCrystalFast.h>
#include <Chrono.h>
#include <LightChrono.h>
#include "Recording.h" //include Custom recording class


//include sd library
#include <SD.h>
//#include <SD_t3.h>
#include <SPI.h>

//include Ethernet library
#include <Ethernet.h>
#include <EthernetUdp.h>

//include biodata library
#include <Respiration.h>
#include <SkinConductance.h>
#include <Heart.h>

//include circular buffer library
#include <CircularBuffer.h>

//include OSC library
//could probaly just keep a couple of these
#include <OSCBoards.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <OSCMatch.h>
#include <OSCMessage.h>
#include <OSCTiming.h>
#include <SLIPEncodedSerial.h>
#include <SLIPEncodedUSBSerial.h>

#include <Bounce2.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//-----------------------------GLOBAL VARIABLES DEFINITION--------------------------//
bool connectedSensors[4] = {true,true,true,false};
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

IntervalTimer captureData; //create instance of Interval timer for sensors
Recording r(NUM_SIGNALS); //create instance of Recording object


Heart heart(A7); // create instance for heart sensor
SkinConductance sc1(A6); // create instance for gsr sensor1
SkinConductance sc2(A2); // create instance for gsr sensor2
Respiration resp(A3); //create instance for respiration sensor

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

File recFile; // create an instance of File object for the recording file
bool fileOpen = false; // boolean to keep track of if a file is open to write
const int chipSelect = 10; // cs pin for sd card 10 for TeensyAudio, 4 for Ethernet.

String fileDigits = "000";
String fileExtension = ".txt";
char filename[11] = "rec199.txt";  //initial filename here

String signalTypes[NUM_SIGNALS] = {"HEART", "HEARTAMP", "HEARTBPM", "SCR", "SCL", "RESP", "RESPAMP", "RESPBPM"};


// Enter da MAC address and IP address for your controller below.
// The IP adress will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // default Arduino mac adress
IPAddress ip(169, 254, 2, 78); // Microcontroller ip adress

unsigned int localPort = 8888;      // local port to listen on
int computerPort = 8888; //check if can replace this for localPort in the code ( most probably yes ) because they hold the same value
IPAddress computerIP ; // ip adress of the computer we're talking to

//OSCBundle sample; // create an osc bundle object to send the sample @ 1000hz
EthernetUDP Udp; // an EthernetUDP instance to let us send and receive packets over UDP

CircularBuffer<float , BUFFER_SIZE >bufferA; //Create an instance of a circular buffer to store data before sending
CircularBuffer<unsigned long , BUFFER_SIZE >bufferTime;
float writeBuffer[BUFFER_SIZE] = {0}; // Temporary buffer that holds the data while writing to card
unsigned long writeBufferTime[BUFFER_SIZE] = {0};
//These bools can be used as protection to prevent starting a recording without all the infos for the header
bool Connected = false;
bool nameReceived = false;
bool locReceived = false;
bool dateReceived = false;
bool rateReceived = false;

bool readyToWrite = false; // boolean to prevent writing to the card when it's not time

unsigned long stamp = 0; // holds the millis timestamp when the recording starts

bool filenameAvailable = false;

Bounce startButton =  Bounce();
Bounce markerButton = Bounce();

String emotionFilename;
String selectedEmotion ;
String emotions[NUM_EMOTIONS] = {" Happiness      " , " Sadness        " , " Fear           " , " Anger          " , " Arousal        " , " Surprise       " , " Neutral        "} ; //spaces in the strings are use to format text to display
String emotionsName[NUM_EMOTIONS] = {"Happiness" , "Sadness" , "Fear" , "Anger" , "Arousal" , "Surprise" , "Neutral"} ; //spaces in the strings are use to format text to display
String emotionsFile[NUM_EMOTIONS] = {"HAP" , "SAD" , "FEA" , "ANG" , "ARO" , "SUR" , "NEU"} ;
String recordingLCD[4] = { "  Recording     ", "  Recording.  ", "  Recording..  ", "  Recording...  "};
char feelingIt[17] = "   Feeling it   ";
int recordingLCDIndex = 0;
char emptyLine[17] = "                ";
String infoEmotion;

int displayIndex = 0;
int potVal = 0;
unsigned long timestamp;
Chrono recordingUpdate;
Chrono lcdUpdate;
//buffers use one more char than the screen can display for a null terminator
char lcdLine1[17];
char lcdLine2[17];

bool filenameNotChecked = true;
char idleLine1[17] = "Select emotion: ";
char lcdRecordingOver[17] = "End of recording";
