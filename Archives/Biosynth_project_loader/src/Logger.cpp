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

char buff[64]{};
sprintf(buff, "biosynth_%d_session_", configuration::board_id);

int maxSession = 0;
File root = SD.open("/");
while (File file = root.openNextFile()) {
  char filename[64];
  strcpy(filename, file.name()); // Use strcpy() to copy the filename
  if (strncmp(filename, buff, strlen(buff)) == 0) {
    int session = atoi(filename + strlen(buff));
    if (session > maxSession) {
      maxSession = session;
    }
  }
  file.close(); // Don't forget to close the file after you're done with it
}
session = maxSession + 1;
sprintf(buff, "biosynth_%d_session_%d_recording_", configuration::board_id, session);
}

void logger::create_file() {
    char buff[64]{};
    int count {1};

    sprintf(buff, "biosynth_%d_session_%d_recording_%d.%s", configuration::board_id, session, count, extension);

    while (SD.sdfs.exists(buff)) {
        Serial.println("File already exists");
        count++;
        sprintf(buff, "biosynth_%d_session_%d_recording_%d.%s", configuration::board_id, session, count, extension);
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

    recording.println("NumSamples,HeartRaw,HeartNormalized,HeartBeatDetected,HeartAmplitudeChange,HeartBPM,HeartBPMChange,SkinRaw,SkinSCR,SkinSCL,RespRaw,RespNormalized,RespScaled,RespisExhaling,RespRawAmplitude,RespNormalizedAmplitude,RespScaledAmplitude,RespAmplitudeLevel,RespAmplitudeChange,RespAmplitudeVariability,RespInterval,RespRpm,RespNormalizedRpm,RespScaledRpm,RespRpmLevel,RespRpmChange,RespRpmVariability");
    recording.flush();  // Ensure the header is written to the file
}


void logger::log_data() {
// Serial.println("log data");
// Serial.println(logging.load());
recording.print(numSamples);
recording.write(', ');
recording.print(biosensors::heart.getRaw());
recording.write(' ');
recording.print(biosensors::heart.getNormalized());
recording.write(' ');
recording.print(biosensors::heart.beatDetected());
recording.write(' ');
recording.print(biosensors::heart.amplitudeChange());
recording.write(' ');
recording.print(biosensors::heart.getBPM());
recording.write(' ');
recording.print(biosensors::heart.bpmChange());
recording.write(' ');
recording.print(biosensors::sc1.getRaw());
recording.write(' ');
recording.print(biosensors::sc1.getSCR());
recording.write(' ');
recording.print(biosensors::sc1.getSCL());
recording.write(' ');
recording.print(biosensors::resp.getRaw());
recording.write(' ');
recording.print(biosensors::resp.getNormalized());
recording.write(' ');
recording.print(biosensors::resp.getScaled());
recording.write(' ');
recording.print(biosensors::resp.isExhaling());
recording.write(' ');
recording.print(biosensors::resp.getRawAmplitude());
recording.write(' ');
recording.print(biosensors::resp.getNormalizedAmplitude());
recording.write(' ');
recording.print(biosensors::resp.getScaledAmplitude());
recording.write(' ');
recording.print(biosensors::resp.getAmplitudeLevel());
recording.write(' ');
recording.print(biosensors::resp.getAmplitudeChange());
recording.write(' ');
recording.print(biosensors::resp.getAmplitudeVariability());
recording.write(' ');
recording.print(biosensors::resp.getInterval());
recording.write(' ');
recording.print(biosensors::resp.getRpm());
recording.write(' ');
recording.print(biosensors::resp.getNormalizedRpm());
recording.write(' ');
recording.print(biosensors::resp.getScaledRpm());
recording.write(' ');
recording.print(biosensors::resp.getRpmLevel());
recording.write(' ');
recording.print(biosensors::resp.getRpmChange());
recording.write(' ');
recording.print(biosensors::resp.getRpmVariability());
    #if FOOT_PEDAL
    // recording.write(',');
    // recording.print(!button::foot_pedal.read());
    #endif
    recording.write(';');
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
