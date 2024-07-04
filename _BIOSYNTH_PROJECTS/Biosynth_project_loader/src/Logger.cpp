/**
 * @file Logger.cpp
 * @author Etienne Montenegro
 * @brief Implementation file of the logger class
 * @version 0.1
 * @date 2022-04-02
 */
#include "Logger.h"
#include "pins.h"
#include <RingBuf.h>
#include "lcd.h"


void logger::initialize(){

  SPI.setMOSI(pins::sd::mosi); 
  SPI.setSCK(pins::sd::sck);
        
  if (!SD.sdfs.begin(SdSpiConfig(pins::sd::cs, SHARED_SPI, SD_SCK_MHZ(24))))
  { 
    
    //wrap this in function or find a way to display this messsage from biosynth class 
    sprintf(screen::buffer_line_1, "   NO SD CARD   ");
    sprintf(screen::buffer_line_2, "                ");
    screen::update();  

    while (1)// stop here, but print a message repetitively 
    {

      Serial.println("Unable to access the SD card");

      delay(500);
    }
  }
    Serial.println("sd card initialized");
}

//add filename as argument
void logger::create_file(){

  char buff[64]{};
  int count {1};

  sprintf(buff, "%s_%d.%s",filename,count,extension);

  while(SD.sdfs.exists(buff)){
    Serial.println("File already exist");
    count++;
    sprintf(buff, "%s_%d.%s",filename,count,extension);
    Serial.printf("Trying new filename %s\n", buff);
  }

  recording = SD.sdfs.open(buff, O_WRITE | O_CREAT);

  
  unsigned int len = recording.fileSize();
  Serial.printf("%s created\n", buff); //make this print dynamic

  if (len > 0) {
    // reduce the file to zero if it already had data
    Serial.println("File is being truncated because it already contained data");
    recording.truncate();
  }

  if (recording.preAllocate(file_size*1024*1024)) {
    Serial.printf("Allocating %d megabytes for %s\n",file_size,buff); //make this print dynamic
  } else {
    Serial.println("unable to preallocate memory for this file");
    recording.close();
    return;
  }

 
  //recording.close();   
}

void logger::log_data(const int heart, const int gsr, const int resp){

  //static uint64_t last_cur_pos{0};
  static bool finalize{false};

  //recording = SD.sdfs.open("session.txt", O_WRITE); //make this dynamic with file name
  //recording.seekCur(last_cur_pos);
  if(logging){
    finalize = true;
    recording.print(heart);
    recording.write(',');
    recording.print(gsr);
    recording.write(',');
    recording.println(resp);
  }

   //last_cur_pos = recording.curPosition();

  if(!logging && finalize){
    recording.sync();
    recording.truncate();
    recording.rewind();
    recording.close();
    Serial.println("File finalized");
    finalize = false;
  }


  
}

void logger::start_logging(){
  logging = true;
}

void logger::stop_logging(){
  logging = false;
}

bool logger::is_logging(){
  return logging;
}
