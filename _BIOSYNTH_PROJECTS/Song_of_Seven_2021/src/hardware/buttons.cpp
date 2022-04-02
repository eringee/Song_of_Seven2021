#include <hardware/buttons.h>
#include <ArduinoLog.h>

namespace button{

    Button encoder{};
    Button foot_pedal{};

    void initialize(){
        encoder.attach(pins::hardware::encoder_button, INPUT_PULLUP);
        encoder.interval(refresh_rate);
 
        foot_pedal.attach(pins::hardware::foot_pedal, INPUT_PULLUP);
        foot_pedal.interval(refresh_rate);

        Log.infoln("Buttons are initialized");
    } 

    void update(){
        encoder.update();
        foot_pedal.update();
    }  

}//namespace button