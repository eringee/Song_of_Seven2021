/**
 * @file buttons.h
 * @author Etienne Montenegro
 * @brief Module interfacing with the different buttons in the project
 * @version 1.1
 * @date 2022-04-02
 */
#pragma once
#include <Bounce2.h>
#include "pins.h"

namespace button{

    class BiosynthButton : public Bounce2::Button
    {
    public:
        bool longPress(int duration);
    };

    extern BiosynthButton encoder;
    extern BiosynthButton foot_pedal;
    const float refresh_rate{1};

    /** @brief Initialization routine of the component
     */
    void initialize();

    /** @brief Update the button state
     */
    void update();

    bool getEncoder();

}//namespace button