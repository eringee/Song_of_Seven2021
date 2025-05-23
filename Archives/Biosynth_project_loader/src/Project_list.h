/**
 * @file Project_list.h
* @author Etienne Montengro
 * @brief  File containing all the projects you want to have as options in the biosynth
 * @version 0.1
 * @date 2022-11-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "Projects.h"

//TO ADD A PROJECT ADD ITS NAME TO THE ENUM ProjectList and include its file in this file.

enum ProjectList : int
{
    SONG_OF_SEVEN,
    RECORDER,
    DATA_OSC, 
    AFFECT_FLOW
};

#include "Song_of_seven.h"
#include "Recorder.h"
#include "Affect_Flow.h"
#include "Data_OSC.h"