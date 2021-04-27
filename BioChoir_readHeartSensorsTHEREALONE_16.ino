#define smooth 100
#include <Average.h>    // library for averaging data
#include <Bounce2.h>    // debouncing library

// Colored buttons/LEDs used to configure (in configuration mode).

#define redButton 8
#define greenButton 9
#define blueButton 10

// This button toggles configuration vs performance mode.
#define blackButton 3

Bounce debouncerRed   = Bounce();
Bounce debouncerGreen = Bounce();
Bounce debouncerBlue  = Bounce();
Bounce debouncerBlack = Bounce();

Average<float> GSRsmooth(50);
Average<float> GSRaverager(1250);
Average<float> GSRlightav(100);

Average<float> HBPMav(1000);
Average<float> Hampav(1000);
Average<float> gsrLighter(50);
Average<float> gsrNoter(50);

int heartOn   = 300;
float gsrNote = 0.0;

////////////////////////////////////////////////////////////
int respLight;
int heartLight;
int gsrLight;

int blinkPin = 13;                // pin to blink led at each beat

// Colored LEDs that either display sensor data (performance mode) OR show the corresponding
// button that is toggled (configuration mode).
int heartLed =  4;    //red
int gsrLed   =  5;    //green
int respLed  =  6;    //blue

boolean configMode  = false;

boolean redToggle   = false;   // you can separate different sounds by using these boolean logics
boolean greenToggle = false;   
boolean blueToggle  = false;

/////////////////////////////GSR SENSOR VARS/////////////////////////////////////////////
int GSRsignal;     //Raw signal of GSR
int GSRmaster = 5; // The number of GSR relative to the average
int GSRmastered;   //The final GSR 
int GSRav;         //
int GSRmax;
int gsrLightFinal;
int maxat = 0;

/////////////////////////////HEART SENSOR VARS/////////////////////////////////////////////

unsigned long HsampleCounter = 0;          // used to determine pulse timing
unsigned long lastBeatTime = 0;           // used to find IBI
int HP = 200;                      // used to find peak in pulse wave, seeded
int HT = 200;                     // used to find trough in pulse wave, seeded
int Hthresh = 200;                // used to find instant moment of heart beat, seeded
boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
boolean secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

int HBPM;                   // used to hold the pulse rate
int HeartSignal;            // holds the incoming raw data
int HeartAmp = 75;
boolean HeartBeatX = false;
int hIBI = 200;             // holds the time between beats, must be seeded!
boolean Pulse = false;     // true when pulse wave is high, false when it's low
boolean Breath = false;
boolean HQS = false;        // becomes true when Arduino finds a beat.
int unscaledHeart;

int rate[10];                    // array to hold last ten IBI values.  I know it's old code.
boolean Heart = true;            // tells the pulseBreath function which signal it is reading.  This is also old.
int Ampdev;
int HN;

/////////////////////////////AUDIO VARS//////////////////////////
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=157,382
AudioSynthNoiseWhite     noise1;         //xy=186,242
AudioSynthWaveform       waveform3;      //xy=197,497
AudioSynthWaveform       waveform1;      //xy=231,126
AudioSynthWaveformSineModulated sine_fm3;       //xy=314,386
AudioSynthWaveformSine   sine1;          //xy=318,426
AudioSynthWaveformSineModulated sine_fm4;       //xy=336,493
AudioSynthWaveformSineModulated sine_fm2;       //xy=341,237
AudioSynthWaveformSineModulated sine_fm1;       //xy=372,124
AudioEffectEnvelope      envelope1;      //xy=513,128
AudioMixer4              mixer1;         //xy=564,223
AudioMixer4              mixer2;         //xy=618,416
AudioOutputAnalog        dac1;           //xy=740,237
AudioConnection          patchCord1(waveform2, sine_fm3);
AudioConnection          patchCord2(noise1, sine_fm2);
AudioConnection          patchCord3(waveform3, sine_fm4);
AudioConnection          patchCord4(waveform1, sine_fm1);
AudioConnection          patchCord5(sine_fm3, 0, mixer2, 1);
AudioConnection          patchCord6(sine1, 0, mixer2, 2);
AudioConnection          patchCord7(sine_fm4, 0, mixer2, 3);
AudioConnection          patchCord8(sine_fm2, 0, mixer1, 1);
AudioConnection          patchCord9(sine_fm1, envelope1);
AudioConnection          patchCord10(envelope1, 0, mixer1, 0);
AudioConnection          patchCord11(mixer1, 0, mixer2, 0);
AudioConnection          patchCord12(mixer2, dac1);
// GUItool: end automatically generated code


void setup(void)
{
  
  noise1.amplitude(0.3);
  sine_fm2.frequency(622);      //harmonics of Eb -  1-622 2-933 
                                  // 3-1244 4- 1555 5-1866 
                                  // 6- 2177 7- 2488  
  sine_fm2.amplitude(0.0);
  
  Serial.begin(9600);  
  AudioMemory(10);   //This might need to be adjusted, depending.
  
  waveform1.begin(0.4, 1020, WAVEFORM_SINE);  //something below 32Hz displays a clear beating pattern rathern than modulation
  sine_fm1.frequency(500);   //this is the same amplitude as one of the atmospheric sine waves
  sine_fm1.amplitude(0.05);  

  waveform2.begin(0.004, 1, WAVEFORM_SINE);
  sine_fm3.frequency(311);           //atmospheric sines
  sine1.frequency(424);
  sine_fm3.amplitude(0.1);
  sine1.amplitude(0.03);


  waveform3.begin(0.05, 1, WAVEFORM_SINE);
  sine_fm4.frequency(622);
  sine_fm4.amplitude(0.075);
  envelope1.attack(1);            //attack envelope for heartbeat
  envelope1.decay(200);
  envelope1.release(100);
  
  pinMode(blinkPin,OUTPUT);       // pin that will blink to heartbeat
  pinMode(heartLed, OUTPUT);      //led for heart signal
  pinMode(respLed, OUTPUT);       //ostensibly this led would be resp signal but screw that for now
  pinMode(gsrLed, OUTPUT);        // led for gsr above average reporting

  pinMode(redButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(blackButton, INPUT_PULLUP);

  //setting up the Bounce instance :
  debouncerRed.attach(redButton);
  debouncerRed.interval(20);        // inverval in ms
  debouncerGreen.attach(greenButton);
  debouncerGreen.interval(20);      // inverval in ms
  debouncerBlue.attach(blueButton);
  debouncerBlue.interval(20);       // inverval in ms
  debouncerBlack.attach(blackButton);
  debouncerBlack.interval(20);      // inverval in ms

}

elapsedMillis every4;

void loop() {
  
  debouncerBlack.update();
  debouncerRed.update();
  debouncerGreen.update();
  debouncerBlue.update();
  
  if (debouncerBlack.fell() ) {
    configMode = !configMode;
  }

  if (configMode) {
    if (debouncerRed.fell() ) redToggle = !redToggle;
    if (debouncerGreen.fell() ) greenToggle = !greenToggle;
    if (debouncerBlue.fell() ) blueToggle = !blueToggle;

// USE THESE TOGGLES TO ACTIVATE DIFFERENT SOUND SETTINGS IN MAIN LOOP WITH BOOLEAN LOGIC // 

    if (redToggle) analogWrite(heartLed, 255);
    else analogWrite(heartLed, 0);
    
    if (greenToggle) analogWrite(gsrLed, 255);
    else analogWrite(gsrLed, 0);
    
    if (blueToggle) analogWrite(respLed, 150);  // this LED's brightness needs to get toned down
    else analogWrite(respLed, 0);

    if (!redToggle && !greenToggle && !blueToggle) {  //moving to canada
      waveform2.frequency(1);
      sine_fm3.frequency(311);   //atmospheric sine1
      sine1.frequency(424);      //atmospheric sine2
      sine_fm3.amplitude(0.1);  
      sine1.amplitude(0.05);
  
      sine_fm2.frequency(622);      //sweat harmonics -  
                                      // 1-622 2-933 
                                      // 3-1244 4- 1555 5-1866 
                                      // 6- 2177 7- 2488  
    }
    
    if (!redToggle && greenToggle && !blueToggle) {  // cat died
      waveform2.frequency(1);
      sine_fm3.frequency(311);   //atmospheric sine1
      sine1.frequency(369);      //atmospheric sine2
      sine_fm3.amplitude(0.1);  
      sine1.amplitude(0.03);
  
      sine_fm2.frequency(174);      //sweat harmonics -  
                                      // 1-174 2-348 
                                      // 3-1244 4- 1555 5-1566 
                                      // 6- 1740 7- 2488  
    }
    if (!redToggle && !greenToggle && blueToggle) {  //baby rabbit
      waveform2.frequency(1);
      sine_fm3.frequency(261.63);   //atmospheric sine1
      sine1.frequency(329.63);      //atmospheric sine2
      sine_fm3.amplitude(0.1);  
      sine1.amplitude(0.1);
  
      sine_fm2.frequency(392);      //sweat harmonics - pentatonic
                                      // 1-392 2-416 
                                      // 3-448 4- 587 5-659 
                                      // 6- 1046 7- 1174  
    }
    
    if (!redToggle && greenToggle && blueToggle) {  // board games
      waveform2.frequency(1);
      sine_fm3.frequency(659);   //atmospheric sine1
      sine1.frequency(985);      //atmospheric sine2
      sine_fm3.amplitude(0.1);  
      sine1.amplitude(0.1);
  
      sine_fm2.frequency(1148);      //sweat harmonics - pentatonic
                                      // 1-1148 2-1312 
                                      // 3-1476 4- 1640 5-1804 
                                      // 6- 1968 7- 2132  
    }
  }

  else {
    analogWrite(respLed, 0);

  int harmonies = HBPM*1.25;
  waveform3.frequency(HeartAmp/100);
  Serial.print(HeartAmp/100);
  sine_fm4.frequency(harmonies);  
  //perform this loop every 4 milliseconds
    if (every4 >= 4) {    
      every4 = every4 - 4;
      SensorCode();    //envelope1 is activated INSIDE sensor code ATM when heartbeat is detected.
    }
  
    //debugging
    
    /*
    Serial.print("Mean: "); Serial.print(HBPMav.mean()); Serial.print("   "); 
    Serial.print("HeartSignal "); Serial.print(HeartSignal); Serial.print("     ");
    Serial.print("BPM "); Serial.print(HBPM); Serial.print("     ");
    Serial.print(" Amplitude  "); Serial.print(HeartAmp); Serial.print("     ");
    Serial.print("AmpDeviation: "); Serial.print(Hampav.stddev());
    Serial.print("    ");
    Serial.print("   GSR   ");  // don't forget to Serial.println the last one
    Serial.print(GSRmastered);
    Serial.print("   ");
*/
    
    LEDwriting();
    
    if(heartOn < 300) heartOn++;
    if(heartOn == 299) envelope1.noteOff();
    
    gsrNote = (float) GSRmastered/66.111;
    gsrNoter.push(gsrNote); 
    
    float finalSine = gsrNoter.mean(); 
    if (finalSine > 0.5) finalSine = 0.5;
    sine_fm2.amplitude(finalSine); 
    Serial.println(finalSine); 
    
    if(HQS) {
      HBPMav.push(HBPM);     //smooth out the Heartbeat per minute data
      Hampav.push(HeartAmp); //smooth out amplitude of heart signal data
      waveform1.frequency(1000+HeartAmp);
      envelope1.noteOn();
      heartOn = 0;
      HQS = false;           //reset quantified heart
    }
  }
}


////////////////////////////////FUNCTIONS/////////////////////////////////////////////////////////////////////

void SensorCode() {
  
  GSRcoder();
  
  HeartSignal = analogRead(A8);
  HsampleCounter += 4;  
  HN = HsampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
  
    //  find the peak and trough of the pulse wave
  if(HeartSignal < Hthresh && HN > (hIBI/4)*3){       // avoid dichrotic noise by waiting 3/5 of last IBI
    if (HeartSignal < HT) HT = HeartSignal;   // keep track of lowest point in pulse wave 
  }

  if(HeartSignal > Hthresh && HeartSignal > HP) HP = HeartSignal; // keep track of highest point in pulse wave
  // find the peak and trough of the respiration
  
 //////////////////////////////////////////////////////////////////////////////////////////////
 
  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT  
  if (HN > 250){                                   // avoid high frequency noise
    if ( (HeartSignal > Hthresh) && (Pulse == false) && (HN > (hIBI/4)*3) ){        
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
      digitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
      HeartBeatX = true;                          //useful variable to say if the heartbeat is happening
      hIBI = HsampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = HsampleCounter;               // keep track of time for next pulse

      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for(int i=0; i<=9; i++) rate[i] = hIBI; // seed the running total to get a realisitic BPM at startup
      }

      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        return;                              // IBI value is unreliable so discard it
      }   

      // keep a running total of the last 10 IBI values
      HBPM = BPMcalc(hIBI);
      HQS = true;   //we have a valid value so we'll mark it as useable in the array
    }                       
  }

//////////////////////////////////////////////////////////////////////////////////////////////////
  if(HeartSignal < Hthresh && Pulse == true) {  // when the values are going down, the beat is over
   digitalWrite(blinkPin,LOW); // turn off pin 13 LED
   HeartBeatX = false;
   HeartAmp = Amplitude(Hthresh, HP, HT); 
   Pulse = false;
  }

  if (HN > 1500){                           // if 2.5 seconds go by without a beat
    if (Hthresh < 1000) Hthresh = Hthresh+50;
    else (Hthresh = 100);
    // set thresh default
    HP = Hthresh;                               // set P default
    HT = Hthresh;                               // set T default
    lastBeatTime = HsampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }  
}   // every 4 millis sensor code end

void GSRcoder() {
  int FakeSignal;
  FakeSignal = analogRead(A1); //
  GSRsignal = analogRead(A2);    //read the signal
  GSRsmooth.push(GSRsignal);    //put it into a smoothing array
  GSRmaster = GSRsmooth.mean();  //take the smoothed value
  GSRmaster = GSRmaster-1023;    //inverse the smoothed value
  GSRmaster = GSRmaster*-1;
  
  if(GSRmaster<0) GSRaverager.push(GSRmaster-10); //only the greater than average values matter, shove the lesser than values down quick
  else GSRaverager.push(GSRmaster);               //array for determining an average GSR over time
  GSRav = GSRaverager.mean();        
  GSRmastered = GSRmaster-GSRav;                 //Turn this all into a nice, easy value from 0 to 15   
  if (GSRmastered<0) GSRmastered = 0;
  if(GSRmaster>GSRmax) GSRmax = GSRmaster;      //keep track of the latest max value 
  else GSRmax = (GSRaverager.maximum(&maxat));
}

volatile int Amplitude(int thresh, int P, int T){ 
  volatile int amp = P - T;              // get amplitude of the pulse wave
  thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
  HP = thresh;                         // reset these for next time
  HT = thresh;
  return amp;
}  

int BPMcalc(int numbers){
  word runningTotal = 0;                  // clear the runningTotal variable    
  for(int i=0; i<=8; i++){                // shift data in the rate array
    rate[i] = rate[i+1];                  // and drop the oldest IBI value 
    runningTotal += rate[i];              // add up the 9 oldest IBI values
  }
  rate[9] = numbers;                          // add the latest IBI to the rate array
  runningTotal += rate[9];                // add the latest IBI to runningTotal
  runningTotal /= 10;                     // average the last 10 IBI values                   
  int BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
  return BPM;
} 

void LEDwriting() { 
  //Serial.println("LED writing");
  if (GSRmastered>15) GSRmastered = 15;        //keep the mapping below 15 no matter what
  gsrLight=map(GSRmastered, 0, 15, 0, 255);
  gsrLighter.push(gsrLight);                   //average this out so the LED doesn't flicker
  if(GSRmaster<GSRav) gsrLightFinal = 0;       //less than average implies an absence of sweat
  else gsrLightFinal = gsrLighter.mean();      
  analogWrite(gsrLed, gsrLightFinal);

  heartLight=map(HeartSignal, HT, HP, 10, 255);
  analogWrite(heartLed, heartLight);
}

