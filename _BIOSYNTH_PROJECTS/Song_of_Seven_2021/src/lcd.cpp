#include "lcd.h"
#include <ArduinoLog.h>

namespace screen{

    LiquidCrystal_I2C lcd(0x27, 16 , 2);
   
    char buffer_line_1[17]{};
    char buffer_line_2[17]{};


    void initialize(){
        lcd.init();
        lcd.backlight();
        Log.infoln("LCD display initialized");
    }

    void update(){
       
        lcd.setCursor(0, 0);
        lcd.print(buffer_line_1);
        lcd.setCursor(0, 1);
        lcd.print(buffer_line_2);

    }

}//namespace screen