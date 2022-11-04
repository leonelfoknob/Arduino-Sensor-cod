#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include<Servo.h>


LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
Servo servo1;
Servo servo2;

int jx = A0;
int jy = A1;


void setup() {
  servo1.attach(3);
  servo2.attach(5);
  lcd.init();
  lcd.backlight();
  lcd.begin (16, 2);
  Serial.begin(9600);
}

void loop() {
  int valx = analogRead(jx);
  int valy = analogRead(jy);
  int servoval1 = map(valy,0,1023,0,180);
  int servoval2 = map(valx,0,1023,0,180);
  servo1.write(servoval1);
  servo2.write(servoval2-90);
  
    lcd.setCursor(1, 0);
    lcd.print(valx);
    lcd.print("    ");
    lcd.setCursor(1, 1);
    lcd.print(valy);
    lcd.print("    ");

}
