/**
 * @file encoder.h
 * @author Etienne Montenegro
 * @brief Module interfacing the Rotary encoder
 * @version 1.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include <Encoder.h>

namespace encoder{

    extern Encoder encoder;
   
    /** @brief Initialization routine of the component
     */
    void initialize();

    /**
     * @brief Set the encoder to the passed value 
     * @param value to set the encoder to
     */
    void set_value(const int& value);

    /** @brief update and read the encoder value
     *  @return the current encoder value
     */
    int update(int number_section);

    /** @brief limits the encoder value inside the passed bounds
     *  @param pos value to test for limits
     *  @param min_pos minimum value accepted
     *  @param max_pos maximum value accepted
     *  @return the passed value after the limit test
     */
    int limit(const int& pos,const int& min_pos, const int& max_pos); //maybe add option to wraparound
    
}//namespace encoder