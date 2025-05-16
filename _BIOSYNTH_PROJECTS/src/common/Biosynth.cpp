/**
 * @file Biosynth.cpp
 * @author Erin Gee & Etienne Montenegro
 * @brief  implementation file of the Biosynth class
 * @version 1.1
 * @date 2022-04-02
 */
#include "Biosynth.h"

#include <Chrono.h>
#include <atomic>

#include "Project_list.h"
#include "biosensors.h"
#include "buttons.h"
#include "enc.h"
#include "lcd.h"
#include "led.h"

#ifdef AUDIO
#include "audio_manager.h"
#endif

enum state
{
  BOOT,
  CHANGESECTION,
  CURRENTSECTION,
  CONFIRMLOGGING,
  LOGGING,
  STOPLOGGING,
  DISPLAYDATA
};

state lcd_state = BOOT;
Chrono lcdUpdate;

void Biosynth::initialize() {
    Serial.println("Erin Gee's Biosynth");

    screen::initialize();
    encoder::initialize();
    button::initialize();
    loadProject();
    led::initialize();
    biosensors::initialize();

    #ifdef AUDIO
    audio_manager::audio_shield_initialization();
    audio_manager::mute(true);
    #endif

    #ifdef LOG
    session_log.initialize();
    #endif

    project->setup();

    #ifdef AUDIO
    audio_manager::mute(false);
    #endif

    screen::clear();
    updateLCD();
    lcdUpdate.restart();
}

void Biosynth::loadProject()
{
  Serial.println("Select project");

  switch (selected_project)
  { // add new projects to this switch case (just copy paste the case and change the title and class name)
  case SONGOFSEVEN:{
    #ifdef SONG_OF_SEVEN
      project = new SongOfSeven(&biosensors::heart, &biosensors::sc1,
                                &biosensors::resp, &biosensors::sc2);
    #endif
    break;
  }
  case AFFECTFLOW:{
    #ifdef AFFECT_FLOW
    project = new AffectFlow(&biosensors::heart, &biosensors::sc1,
                            &biosensors::resp, &biosensors::sc2);
    #endif
    break;
  }

  case NOSHOW:{
    #ifdef NO_SHOW
      project = new NoShow(&biosensors::heart, &biosensors::sc1,
                            &biosensors::resp, &biosensors::sc2);
    #endif
    break;
   }

  }
  Serial.printf("Project loaded: %s\n", project->getName());
}

void Biosynth::update() {
  project->update();
  button::update(); 
  biosensors::update();
  current_encoder_value = encoder::update(project->getNumberOfSection());

  if (lcdUpdate.hasPassed(40, true)){
  led::update(project->getLedProcessed());
  #ifdef DISPLAY_DATA
  updateLCD();
  #endif
  lcdUpdate.restart();
  }

  #ifdef AUDIO
  audio_manager::setVolume(updatePotentiometer());
  #endif
    
  #ifdef LOG
  handle_logging();
  #endif

  #ifdef ADVANCE_WITH_ENCODER
  advanceWithEncoder();
  #endif

  #ifdef ADVANCE_WITH_PEDAL
  advanceWithPedal();
  #endif

  #ifdef SEND_OVER_SERIAL
  send_over_serial(&Serial);
  #endif
}

void Biosynth::updateLCD() {
  switch (lcd_state) {
    case BOOT:{
      Chrono waitTime;
      sprintf(screen::buffer_line_1, "    Biosynth   ");
      sprintf(screen::buffer_line_2, project->getName());
      screen::update();
      waitTime.restart();
      while (!waitTime.hasPassed(2000))
      {
        // wait in function
      }
      lcd_state = CURRENTSECTION;
      updateLCD();
      break;
      } 

    case CURRENTSECTION:{
      Chrono waitTime;
      sprintf(screen::buffer_line_1, "%s", project->getSectionTitle(current_section));
      sprintf(screen::buffer_line_2, "   BIOSYNTH %d ", configuration::board_id);
      screen::update();
      waitTime.restart();
      #ifdef DISPLAY_DATA
      while (!waitTime.hasPassed(500))
      {
        // wait in function
      }
      lcd_state = DISPLAYDATA;
      updateLCD();
      #endif
      break;
      } 

    case CHANGESECTION:{
      sprintf(screen::buffer_line_1, "%s", project->getSectionTitle(current_encoder_value));
      sprintf(screen::buffer_line_2, "   Confirm ?    ");
      screen::update();
      break;
      } 

    case CONFIRMLOGGING:{
      sprintf(screen::buffer_line_1, "  Record on SD? ");
      sprintf(screen::buffer_line_2, "                ");
      screen::update();
      break;
      } 

    case LOGGING:{
      sprintf(screen::buffer_line_1, "  Now Logging   ");
      sprintf(screen::buffer_line_2, "                ");
      screen::update();
      break;
      } 

    case STOPLOGGING:{
      sprintf(screen::buffer_line_1, "  Stop Logging  ");
      sprintf(screen::buffer_line_2, "                ");
      screen::update();
      break;
      } 

    case DISPLAYDATA:{
        #ifdef DISPLAY_DATA
        sprintf(screen::buffer_line_1, "H: %4d G: %4d", biosensors::heart.getRaw(), biosensors::sc1.getRaw());
        sprintf(screen::buffer_line_2, "RT: %5d%s", biosensors::resp.getRaw(), 
        #ifdef LOG
        session_log.is_logging() ? "   REC" : "      "
        #else
        "      "
        #endif
        );
        screen::update();
        #endif
      break;
      } 
  } 

} 

float Biosynth::updatePotentiometer()
{
  float knob2 = analogRead(pins::audio_shield::volume);
  if (knob2 != vol)
  {
    vol = (knob2 / 1023) * 0.8;
  }
  return vol;
}

#ifdef LOG
void Biosynth::handle_logging() {
  bool pressed;
  #ifdef FOOT_PEDAL_LOG
  pressed = button::foot_pedal.longPress(500);
  #endif
  #ifdef ENCODER_LOG
  pressed = button::encoder.pressed();
  #endif

    switch (lcd_state) {
        case DISPLAYDATA:{
        // fallthrough
        }
        case CURRENTSECTION:{
           if (pressed && !session_log.is_logging()) {
                Serial.println("Ask user to record on SD?");
                session_log.create_file();
                lcd_state = CONFIRMLOGGING;
                updateLCD();
                idleTimer.restart();
            } else  {
              lcd_state = DISPLAYDATA;
            }  
            }

        case LOGGING:{
            if (nowLogging.isRunning()) {
                if (nowLogging.hasPassed(1000, true)) {
                    lcd_state = DISPLAYDATA;
                    updateLCD();
                    nowLogging.stop();
                }
            }
            if (pressed && session_log.is_logging() && !endLogging.isRunning()) {
                Serial.println("Ending session");
                session_log.stop_logging();
                Serial.printf("Number of samples recorded: %d\n", session_log.get_num_samples());
                endLogging.restart();
                lcd_state = STOPLOGGING;
                updateLCD();
            }
            break;
            }

        case CONFIRMLOGGING:{
            if (idleTimer.hasPassed(5000, true)) {
                lcd_state = CURRENTSECTION;
                updateLCD();
                idleTimer.restart();
            }
            if (pressed && !session_log.is_logging() && !nowLogging.isRunning()) {
                Serial.println("Starting logging");
                session_log.start_logging();
                lcd_state = LOGGING;
                updateLCD();
                nowLogging.restart();
            }
            break;
            }

        case STOPLOGGING:{
            if (endLogging.isRunning()) {
                if (endLogging.hasPassed(2000, true)) {
                  #ifdef DISPLAY_DATA
                    lcd_state = DISPLAYDATA;
                  #else
                    lcd_state = CURRENTSECTION;
                  #endif
                    updateLCD();
                    endLogging.stop();
                }
            }
            break;
            }
    }
}
#endif

#ifdef ADVANCE_WITH_ENCODER
void Biosynth::advanceWithEncoder() {
  if (button::encoder.pressed() && lcd_state == CHANGE_SECTION)
  {
    Serial.println("Section change confirmed");
    last_section = current_section;
    current_section = current_encoder_value;
    project->changeSection(current_encoder_value);
    lcd_state = CURRENT_SECTION;
    updateLDC();
    confirmTimer.restart();
    confirmTimer.stop();
  }
  if (current_encoder_value != current_section)
  {
    lcd_state = CHANGE_SECTION;
    updateLDC();
    if (!confirmTimer.isRunning())
    {
      confirmTimer.start();
    }
  }

   if (confirmTimer.hasPassed(configuration::confirmation_delay) &&
      lcd_state == CHANGE_SECTION)
  {
    encoder::set_value(current_section);
    lcd_state = CURRENT_SECTION;
    updateLDC();
    confirmTimer.restart();
    confirmTimer.stop();
  }
}
#endif

#ifdef ADVANCE_WITH_PEDAL
void Biosynth::advanceWithPedal() {
  if (button::foot_pedal.longPress(500) && lcd_state == CURRENT_SECTION){
    current_section++;
    current_section = current_section % project->getNumberOfSection();
    project->changeSection(current_section);
    lcd_state = CURRENT_SECTION;
    updateLDC();
    Serial.println("Foot pedal pressed. Advanced section");
    }
}
#endif

#ifdef SEND_OVER_SERIAL
void Biosynth::send_over_serial(Print *output)
{
  output->printf("%d,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%.2f,%d,%.2f,%.2f,%d,%d,%.2f,%.2f,%.2f, %.2f, %.2f,%d, %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", 
                 biosensors::heart.getRaw(), 
                 biosensors::heart.getNormalized(),
                 biosensors::heart.amplitudeChange(),
                 biosensors::heart.getBPM(),
                 biosensors::heart.bpmChange(),
                 
                 biosensors::sc1.getRaw(),
                 biosensors::sc1.getSCR(),
                 biosensors::sc1.getSCL(),

                 biosensors::resp.getRaw(),
                 biosensors::resp.getNormalized(),
                 biosensors::resp.getScaled(),
                 biosensors::resp.isExhaling(),
                 biosensors::resp.getRawAmplitude(),
                 biosensors::resp.getNormalizedAmplitude(),
                 biosensors::resp.getScaledAmplitude(),
                 biosensors::resp.getAmplitudeLevel(),
                 biosensors::resp.getAmplitudeChange(),
                 biosensors::resp.getAmplitudeVariability(),
                 biosensors::resp.getInterval(),
                 biosensors::resp.getRpm(),
                 biosensors::resp.getNormalizedRpm(),
                 biosensors::resp.getScaledRpm(),
                 biosensors::resp.getRpmLevel(),
                 biosensors::resp.getRpmChange(),
                 biosensors::resp.getRpmVariability());
}
#endif

