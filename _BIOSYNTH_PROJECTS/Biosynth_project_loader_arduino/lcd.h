/**
 * @file lcd.h
 * @author Etienne Montenegro
 * @brief Module to interface with 16x2 char LCD screen. Modify the content of the buffer and call update
 * @version 1.1
 * @date 2022-04-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <LiquidCrystal_I2C.h>

namespace screen{
    
    extern LiquidCrystal_I2C lcd; // set the LCD address to 0x27 for a 16 chars and 2 line display

    //buffers use one more char than the screen can display for a null terminator
    extern char buffer_line_1[17];
    extern char buffer_line_2[17];

    /** @brief  Initialization routine
     */
    void initialize();
    
    /** @brief Update the screen with the content of the buffers when called
     */
    void update();

    void bootMessage();
    void clear();
}//namespace lcd