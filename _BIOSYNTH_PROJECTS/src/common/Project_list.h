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
    SONGOFSEVEN,
    AFFECTFLOW,
    NOMUSIC
};

#ifdef AFFECT_FLOW
#include "Affect_Flow.h"
#endif
#ifdef SONG_OF_SEVEN
#include "Song_of_seven.h"
#endif
#ifdef NO_MUSIC
#include "No_music.h"
#endif