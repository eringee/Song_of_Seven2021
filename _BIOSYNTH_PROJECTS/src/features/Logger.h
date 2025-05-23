/**
 * @file Logger.h
 * @author Etienne Montengro
 * @brief Logger class that manages files creation and writting to it
 *        Modify the log_data method to accept as argument what you want to log and its formating.
 * @version 1.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <SD.h>
#include <RingBuf.h>
#include <atomic>

#include "biosensors.h"

class FsFile;


//add name checking not to overwrite older log in sd card
class logger{

  IntervalTimer logTimer;
  FsFile recording;

  const char* extension {"txt"};

  const int file_size{8}; // in megabyte
  const int LOG_FILE_SIZE{file_size * 1024 * 1024};

  int numSamples{0};
  int session {1};


   
private:
  std::atomic<bool> logging{false}; // Declare as atomic and initialize to false
   
   public:
  /**
   * @brief intilialize the sd card, blocks into an infinite while if it can't
   */
  void initialize();

  /**
   * @brief Create a file on the sd card
   */
  void create_file();

  /**
   * @brief write the data to the file.
   * @param signals sample of data to log
   */
  void log_data();  

  /**
   * @brief starts datalogging
   */
  void start_logging();

  /**
   * @brief stops data logging
   */
  void stop_logging();


  /**
   * @brief return true if device is currently logging data
   */
  bool is_logging();

  /**
   * @brief erase the file on the sd card
   */
  void erase_file();

  inline int get_num_samples(){
    return numSamples;
    };
};