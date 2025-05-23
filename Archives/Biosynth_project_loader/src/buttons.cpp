#include "buttons.h"

namespace button{

    Bounce2::Button encoder{};
    Bounce2::Button foot_pedal{};

    unsigned long pedal_start_time{0}; 
    bool foot_pedal_confirmed{false};

    void initialize(){
        encoder.attach(pins::hardware::encoder_button, INPUT_PULLUP);
        encoder.interval(refresh_rate);
 
        foot_pedal.attach(pins::hardware::foot_pedal, INPUT_PULLUP);
        foot_pedal.setPressedState(LOW);
        foot_pedal.interval(refresh_rate);

        Serial.println("Buttons are initialized");
    } 

void update() {
    encoder.update();
    foot_pedal.update();
    foot_pedal_confirmed = false;

if (foot_pedal.changed() && !foot_pedal.isPressed() && foot_pedal.previousDuration() >= 500) {
   foot_pedal_confirmed = true;
}
}
    
bool getEncoder(){
    encoder.update();
    return encoder.read();
}

}//namespace button