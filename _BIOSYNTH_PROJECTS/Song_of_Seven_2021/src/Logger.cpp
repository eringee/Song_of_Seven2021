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
#include <ArduinoLog.h>

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

      Log.errorln("Unable to access the SD card");

      delay(500);
    }
  }
    Log.infoln("sd card initialized");
}

//add filename as argument
void logger::create_file(){

  char buff[64]{};
  int count {1};

  sprintf(buff, "%s_%d.%s",filename,count,extension);

  while(SD.sdfs.exists(buff)){
    Log.traceln("File already exist");
    count++;
    sprintf(buff, "%s_%d.%s",filename,count,extension);
    Log.traceln("Trying new filename %s", buff);
  }

  recording = SD.sdfs.open(buff, O_WRITE | O_CREAT);

  
  unsigned int len = recording.fileSize();
  Log.infoln("%s created", buff); //make this print dynamic

  if (len > 0) {
    // reduce the file to zero if it already had data
    Log.warningln("File is being truncated because it already contained data");
    recording.truncate();
  }

  if (recording.preAllocate(file_size*1024*1024)) {
    Log.infoln("Allocating %d megabytes for %s",file_size,buff); //make this print dynamic
  } else {
    Log.errorln("unable to preallocate memory for this file");
    recording.close();
    return;
  }

 
  //recording.close();   
}

void logger::log_data(sample signals){

  //static uint64_t last_cur_pos{0};
  static bool finalize{false};

  //recording = SD.sdfs.open("session.txt", O_WRITE); //make this dynamic with file name
  //recording.seekCur(last_cur_pos);
  if(logging){
    finalize = true;
    recording.print(signals.heart);
    recording.write(',');
    recording.print(signals.gsr);
    recording.write(',');
    recording.println(signals.respiration);
  }

   //last_cur_pos = recording.curPosition();

  if(!logging && finalize){
    recording.sync();
    recording.truncate();
    recording.rewind();
    recording.close();
    Log.infoln("File finalized");
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
