/**
 * @file audio_manager.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The audio_manager handles audio hardware in the project
 * @version 1.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "configuration.h"

struct sample;

namespace audio_manager{
   
    /** @brief Initialization routine for pjrc audio shield
     */
    void audio_shield_initialization();
    void setVolume(float v);
    void mute(bool on);

}//namespace audio_manager