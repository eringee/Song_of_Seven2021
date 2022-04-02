/**
 * @file audio_manager.h
 * @author Erin Gee & Etienne Montenegro
 * @brief The audio_manager regroup everything that as to do with sound for this project. its in this file that you'll modify
 *        the default sounds for the synth and the how the different scenes sounds
 * @version 1.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <configuration.h>

struct sample;

namespace audio_manager{
   
    extern int piece_sections[configuration::number_of_sections][configuration::number_of_boards];

    /** @brief Initialization routine for pjrc audio shield
     */
    void audio_shield_initialization();

    /** @brief Default state of the sound played by the biosynth
     */
    void synths_initialization();

    /** @brief Set the volume of the audioboard with potentiometer
     */
    void set_volume();

    /** @brief Modify the sound of the synth according to the selected scene. 
     *         Modify the sound for every scene in this section
     *  @param scene part of the piece selected by the user
     */
    void change_scene(const int& scene);

    /** @brief Modify the audio output of the synth with the passed data.
     *         Write how the biodata signal affect the sound in this function
     *  @param signals sampled biosensor data
     */
    void update(const sample& signals);

}//namespace audio_manager