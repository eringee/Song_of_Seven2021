/**
 * @file Logger.cpp
 * @author Etienne Montenegro
 * @brief Implementation file of the logger class
 * @version 0.1
 * @date 2022-04-02
 */
#include "Logger.h"
#include "configuration.h"
#include "pins.h"
#include "buttons.h"
#include <RingBuf.h>
#include "lcd.h"

void logger::initialize() {
    SPI.setMOSI(pins::sd::mosi); 
    SPI.setSCK(pins::sd::sck);

    logTimer.priority(0);

    if (!SD.sdfs.begin(SdSpiConfig(pins::sd::cs, SHARED_SPI, SD_SCK_MHZ(24)))) { 
        sprintf(screen::buffer_line_1, "   NO SD CARD   ");
        sprintf(screen::buffer_line_2, "                ");
        screen::update();  

        while (1) {
            Serial.println("Unable to access the SD card");
            delay(500);
        }
    }
    Serial.println("SD card initialized");
}

void logger::create_file() {
    char buff[64]{};
    int count {1};

    sprintf(buff, "%s_%d.%s", filename, count, extension);

    while (SD.sdfs.exists(buff)) {
        Serial.println("File already exists");
        count++;
        sprintf(buff, "%s_%d.%s", filename, count, extension);
        Serial.printf("Trying new filename %s\n", buff);
    }

    recording = SD.sdfs.open(buff, O_WRITE | O_CREAT);

    unsigned int len = recording.fileSize();
    Serial.printf("%s created\n", buff);

    if (len > 0) {
        Serial.println("File is being truncated because it already contained data");
        recording.truncate();
    }

    if (recording.preAllocate(file_size * 1024 * 1024)) {
        Serial.printf("Allocating %d megabytes for %s\n", file_size, buff);
    } else {
        Serial.println("Unable to preallocate memory for this file");
        recording.close();
        return;
    }
}


void logger::log_data() {
// Serial.println("log data");
// Serial.println(logging.load());
    recording.print(biosensors::heart.getRaw());
    recording.write(',');
    recording.print(biosensors::sc1.getRaw());
    recording.write(',');
    recording.print(biosensors::resp.getRaw());
    #if FOOT_PEDAL
    recording.write(',');
    recording.print(!button::foot_pedal.read());
    #endif
    recording.write('\n');
    numSamples++;
}


void logger::start_logging(){
//   Serial.println("start logging");
  numSamples = 0;
   logTimer.begin([this](){log_data();}, configuration::biosensors_sample_rate_us);
   logging.store(true);
    // Serial.print("logging set to: ");
    // Serial.println(logging.load());
}

void logger::stop_logging(){
    logTimer.end();
    recording.sync();
    recording.truncate();
    recording.rewind();
    recording.close();
    logging.store(false);
}

bool logger::is_logging() {
    bool state = logging;
    return state;
}
