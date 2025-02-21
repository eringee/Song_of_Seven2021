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
#include "audio_manager.h"
#include "biosensors.h"
#include "buttons.h"
#include "enc.h"
#include "lcd.h"
#include "led.h"

enum lcd_state
{
  BOOT = 0,
  CHANGE_SECTION,
  CURRENT_SECTION,
  START_LOGGING,
  LOGGING
};


void Biosynth::initialize() {
    Serial.println("Erin Gee's Biosynth");

    screen::initialize();
    encoder::initialize();
    button::initialize();
    loadProject();
    audio_manager::audio_shield_initialization();
    audio_manager::mute(true);
    led::initialize();
    biosensors::initialize();

    #if LOG
        session_log.initialize();
    #endif

    project->setup();
    audio_manager::mute(false);
    screen::clear();
}

void Biosynth::loadProject()
{
  Serial.println("Select project");
  selected_project = selectProject(2000);

  switch (selected_project)
  { // add new projects to this switch case (just copy paste the case and change the title and class name)
  case SONG_OF_SEVEN:
    project = new SongOfSeven(&biosensors::heart, &biosensors::sc1,
                              &biosensors::resp, &biosensors::sc2);
    break;

  case RECORDER:
    project = new Recorder(&biosensors::heart, &biosensors::sc1,
                           &biosensors::resp, &biosensors::sc2);
    break;

  case DATA_OSC:
    project = new DataOSC(&biosensors::heart, &biosensors::sc1,
                           &biosensors::resp, &biosensors::sc2);
    break;

  case AFFECT_FLOW:
    project = new AffectFlow(&biosensors::heart, &biosensors::sc1,
                            &biosensors::resp, &biosensors::sc2);
    break;
   }

  Serial.printf("Project loaded: %s\n", project->getName());

  selectedProjectMessage(1000); // get stuck when trying to update lcd
}

void Biosynth::update() {
// static Chrono updateTimer(Chrono::MICROS);
  project->update();
  button::update(); 


  // if (updateTimer.hasPassed((configuration::biosensors_sample_rate_us - 500), true)) {
    biosensors::update();

    #if SEND_OVER_SERIAL
      send_over_serial(&Serial);
    #endif
  
    audio_manager::setVolume(updatePotentiometer());
    current_encoder_value = encoder::update(project->getNumberOfSection());
    
#if LOG
    handle_logging();
    #endif

#if ADVANCE_WITH_ENCODER
  maybe_confirm_section_change();
#else
  if (button::foot_pedal.pressed() && lcd_state == CURRENT_SECTION)
  {
    advance_section();
    Serial.println("Foot pedal pressed. Advanced section");
  }
#endif

  if (lcdUpdate.hasPassed(40, true))
  {
    opening_message();

#if DISPLAY_DATA
    allowDataOnLCD = true;
#endif

#if LOG || DISPLAY_DATA
    displayDataOnScreen();
#endif

#if ADVANCE_WITH_ENCODER
    section_change();
    verify_no_touch();
#endif
    
    led::update(project->getLedProcessed());
  }


}

#if LOG
void Biosynth::handle_logging() {
    switch (lcd_state) {
        case CURRENT_SECTION:
          if (!button::encoder.pressed()) {
              allowDataOnLCD = true;
            } else if (button::encoder.pressed() && !session_log.is_logging()) {
               allowDataOnLCD = false;
                Serial.println("Ask user to record on SD?");
                session_log.create_file();
                sprintf(screen::buffer_line_1, "Record on SD?  ");
                sprintf(screen::buffer_line_2, "               ");
                screen::update();
                lcd_state = START_LOGGING;
            } 
            break;

        case START_LOGGING:
            if (button::encoder.pressed() && !session_log.is_logging() && !nowLogging.isRunning()) {
                Serial.println("Starting logging");
                session_log.start_logging();
                sprintf(screen::buffer_line_1, "  Now Logging  ");
                sprintf(screen::buffer_line_2, "              ");
                screen::update();
                lcd_state = LOGGING;
                nowLogging.restart();
            }
            break;

        case LOGGING:
            if (nowLogging.isRunning() && !allowDataOnLCD) {
                if (nowLogging.hasPassed(1000, true)) {
                    allowDataOnLCD = true;
                    nowLogging.stop();
                }
            }

            if (button::encoder.pressed() && session_log.is_logging() && !endLogging.isRunning()) {
                allowDataOnLCD = false;
                Serial.println("Ending session");
                session_log.stop_logging();
                Serial.printf("Number of samples recorded: %d\n", session_log.get_num_samples());
                endLogging.restart();
                sprintf(screen::buffer_line_1, "Logging Stopped");
                sprintf(screen::buffer_line_2, "               ");
                screen::update();
            }

            if (endLogging.isRunning()) {
                if (endLogging.hasPassed(2000, true)) {
                    current_section_message();
                    endLogging.stop();
                }
            }
            break;
    }
}
#endif

void Biosynth::opening_message()
{
  static bool do_once{false};

  if (!do_once)
  {
    sprintf(screen::buffer_line_1, "Hello!");
    sprintf(screen::buffer_line_2, "I am board #%d", configuration::board_id + 1);
    screen::update();
    do_once = true;
  }
  else if (openingtimer.hasPassed(configuration::opening_message_time) && do_once)
  {
    current_section_message();
    openingtimer.restart();
    openingtimer.stop();
  }
}

void Biosynth::maybe_confirm_section_change()
{
  if (button::encoder.pressed() && lcd_state == CHANGE_SECTION)
  {
    Serial.println("Section change confirmed");
    last_section = current_section;
    current_section = current_encoder_value;
    project->changeSection(current_encoder_value);
    current_section_message();
    confirmTimer.restart();
    confirmTimer.stop();
  }
}

void Biosynth::verify_no_touch()
{
  if (confirmTimer.hasPassed(configuration::confirmation_delay) &&
      lcd_state == CHANGE_SECTION)
  {
    encoder::set_value(current_section);
    current_section_message();
    confirmTimer.restart();
    confirmTimer.stop();
  }
}

void Biosynth::section_change()
{
  if (current_encoder_value != current_section)
  {
    section_confirm_message(current_encoder_value);
    if (!confirmTimer.isRunning())
    {
      confirmTimer.start();
    }
  }
}

void Biosynth::section_confirm_message(const int encoder_value)
{
  sprintf(screen::buffer_line_1, "%s", project->getSectionTitle(current_encoder_value));

  sprintf(screen::buffer_line_2, "   Confirm ?   ");
  lcd_state = CHANGE_SECTION;
  screen::update();
}

void Biosynth::current_section_message()
{
  sprintf(screen::buffer_line_1, "%s", project->getSectionTitle(current_section));
  sprintf(screen::buffer_line_2, "   BIOSYNTH %d ", configuration::board_id + 1);
  lcd_state = CURRENT_SECTION;
  screen::update();
}

void Biosynth::advance_section()
{
  current_section++;
  current_section = current_section % project->getNumberOfSection();
  project->changeSection(current_section);
  current_section_message();
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

ProjectList Biosynth::selectProject(
    const int &timeout)
{ // need to be modified if more than two project
  Chrono waitTime;
  waitTime.restart();
  int project = 0;
  while (!waitTime.hasPassed(timeout))
  {
    project = button::getEncoder();
  }

  if (project == 1)
  { // project selected when button not pressed on boot
    return AFFECT_FLOW; // project 1 is AFFECT_FLOW, change if needed
  }
  else
  { // project selected when button pressed on boot
    return SONG_OF_SEVEN;
  }
}

void Biosynth::selectedProjectMessage(const int &displayTime)
{
  Chrono waitTime;
  sprintf(screen::buffer_line_1, "    Biosynth   ");
  sprintf(screen::buffer_line_2, project->getName());
  screen::update();
  waitTime.restart();
  while (!waitTime.hasPassed(displayTime))
  {
    // wait in function
  }
}

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

void Biosynth::displayDataOnScreen(){
    if(allowDataOnLCD){

    if(!lcd_timer.isRunning()){
      lcd_timer.start();
    }

    if(lcd_timer.hasPassed(200, true)){
     //LCD
     sprintf(screen::buffer_line_1, "H: %4d G: %4d", biosensors::heart.getRaw(), biosensors::sc1.getRaw());
     #if FOOT_PEDAL
     sprintf(screen::buffer_line_2, "R: %.2f FI: %d", biosensors::resp.getTemperature(), !button::foot_pedal.read());
     #else
     sprintf(screen::buffer_line_2, "RT: %5d        ", biosensors::resp.getRaw());
     #endif

    screen::update();
    }
  }
}
        

#if PLOT_SENSOR
void Biosynth::plot_sampled_data()
{
  Serial.printf("%.2f,%.2f,%.2f,%.2f", biosensors::heart.getNormalized(), biosensors::sc1.getSCR(), biosensors::sc2.getSCR(), biosensors::resp.getNormalized());
  Serial.println();
}
#endif

