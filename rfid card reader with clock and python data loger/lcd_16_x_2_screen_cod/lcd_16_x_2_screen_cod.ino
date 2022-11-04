   /*  
lcd 16 x 2 screen cod
pin connection
SDA --> A4
SCL --> A5
GND --> GND
VCC --> 5V

you will also intall LiquidCrystal_I2C library on arduino official web site
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.begin (16, 2);
  lcd.setCursor(0, 0);
  lcd.print("first line");
  lcd.setCursor(0, 1);
  lcd.print("second line");
}

void loop() {

}
