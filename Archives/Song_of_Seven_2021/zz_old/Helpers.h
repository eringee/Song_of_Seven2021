#include <Global.h>

// void setupSD(){ //move to sesson recorder
//         SPI.setMOSI(SDCARD_MOSI_PIN);  // Audio shield has MOSI on pin 7
//         SPI.setSCK(SDCARD_SCK_PIN);  // Audio shield has SCK on pin 14
        
//         if (!SD.sdfs.begin(SdSpiConfig(SDCARD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(24))))//verify if sd card is inserted 
//         { 
//             while (1)// stop here, but print a message repetitively 
//             {
//               Serial.println("Unable to access the SD card");
//               delay(500);
//             }
//         }
//       Serial.println("SD CARD INITIALIZED");
      
//       session = SD.sdfs.open("session.txt", O_WRITE | O_CREAT);
//       unsigned int len = session.fileSize();
      
//       Serial.print("session.log started with ");
//       Serial.print(len);
//       Serial.println(" bytes");
//       if (len > 0) {
//         // reduce the file to zero if it already had data
//         Serial.println("Truncating file");
//         session.truncate();
//       }
//       if (session.preAllocate(20*1024*1024)) {
//         Serial.print("  Allocate 40 megabytes for session.log");
//       } else {
//         Serial.print("  unable to preallocate this file");
//       }
//       //session.seekCur(0);
//       session.println("Song of seven session recording xxxx-xx-xx");
//       session.close();
     
// }

// void setupAudioShield() //move to audio manager
// {
//   pinMode(VOL_POT_PIN,INPUT);  
//   sgtl5000_1.enable();
//   sgtl5000_1.volume(0.5); //set master volume here
//   Serial.println("Audio shield is setup");
// }

// void setupSounds() //move to audio manager  //initial sounds for Section A
// {

//     //GSR dependent variables

//     sine_fm2.frequency(sectionGlobal[0][BOARD_ID-1]); 
//     sine_fm2.amplitude(0.5);
    
//     //HEART dependent variables
//     waveform3.begin(0.01, 0.005, WAVEFORM_SINE);
//     sine_fm4.frequency(622);
//     sine_fm4.amplitude(0.02);

//     //ATMOSPHERIC SINES
//     waveform2.begin(0.004, 1, WAVEFORM_SINE);
//     sine_fm3.frequency(311);           //atmospheric sines
//     sine1.frequency(424);
//     sine_fm3.amplitude(0.1);
//     sine1.amplitude(0.05);

// }

// void openingMessage() //move to LCDScreen //TO REMOVE
// {
//     if(openingMessageTimer.hasPassed(openingMessageTime))
//     {    
//         biosynth.setLCDState(2);
//         openingMessageTimer.restart();
//         openingMessageTimer.stop();
//     }
// }

// void checkSectionChange()  //this is where we change sections AND frequencies...
// {
//     if( encoderButton.read() == 0 && biosynth.getLCDState() == 1)
//     {   //section has been confirmed

//         lastSection = current_section;
//         current_section = biosynth.getEncoderValue();
//         updateLCDBool = true;

//         /// THIS IS WHERE YOU NEED TO UPDATE THE FREQUENCY VALUES
//         sine_fm2.frequency(sectionGlobal[current_section][BOARD_ID-1]);
//         if (current_section==0){
//           sine_fm3.frequency(311);   //atmospheric sine1
//           sine1.frequency(424);      //atmospheric sine2
//         }
//         else if (current_section==1){
//           sine_fm3.frequency(311);   //atmospheric sine1
//           sine1.frequency(369);      //atmospheric sine2
//         }
//         else if (current_section==2){
//           sine_fm3.frequency(261.63);   //atmospheric sine1
//           sine1.frequency(329.63);      //atmospheric sine2
//           sine_fm3.amplitude(0.1);  
//           sine1.amplitude(0.1);
//         }
//         else if (current_section==3){
//           waveform2.frequency(1);
//           sine_fm3.frequency(659);   //atmospheric sine1
//           sine1.frequency(985);      //atmospheric sine2
//           sine_fm3.amplitude(0.1);  
//           sine1.amplitude(0.1);
//         }
//         biosynth.setLCDState(2);
//     }
// }
