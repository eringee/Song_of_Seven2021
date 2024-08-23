/**
 * @file biosensors.h
 * @author Erin Gee & Etienne Montenegro
 * @brief  Module interfacing with the biosensors
 * @version 1.1
 * @date 2022-04-02
 */
#pragma once
#include <Respiration.h>
#include <SkinConductance.h>
#include <Heart.h>
// #include "ExternalADC.h"

//struc used to hold sampled data. It makes it easier to pass the data to various functions
//acees struct members using name of variable .heart or .gsr or .respiration

struct sample{
    
    struct heart{
        float sig;
        float amp;
        float bpm;
    }heart;
    

    struct gsr{
        float sig;
        float scl;
        float scr;
    } gsr;

    struct gsr2{
        float sig;
        float scl;
        float scr;
    }gsr2;

    struct resp{
        float sig;
        float bpm;
        float amp;

    }resp;

};


namespace biosensors{
    
    extern Heart heart;
    extern SkinConductance sc1;
    extern SkinConductance sc2;
    extern Respiration resp;

    // extern ADS1115 ADS;

   /** @brief Initialize the biosensors
    */
    void initialize();

    /** @brief update the biosensors state 
     */
    void update();

    /** @brief  sample, smooth and returns the biosensors data
     *  @return sample containing the 3 sensors data
     */

}//namespace biosensors