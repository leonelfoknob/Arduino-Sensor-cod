//HC12 send string data
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial HC12(5,6);
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

int d1;
int d2;
int d3;
int d4;
char separator =',';
String send_string_data;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.setTimeout(1000);
  
  lcd.init();
  lcd.backlight();
  lcd.begin (16, 2);

}

void loop() {
  d1++;
  d2=d2+2;
  d3=d3+3;
  d4=d4+4;
  send_string_data = String(d1)+String(separator)+String(d2)+String(separator)+String(d3)+String(separator)+String(d4);
  /*Serial.print("send_string_data :");
  Serial.println(send_string_data);*/
  //Serial.println(send_string_data);
  HC12.print(send_string_data);

  lcd.setCursor(0, 0);
  lcd.print("d1:");
  lcd.setCursor(3, 0);
  lcd.print(d1);
  lcd.print("  ");

  lcd.setCursor(8, 0);
  lcd.print("d2:");
  lcd.setCursor(11, 0);
  lcd.print(d2);
  lcd.print("  ");

  lcd.setCursor(0, 1);
  lcd.print("d3:");
  lcd.setCursor(3, 1);
  lcd.print(d3);
  lcd.print("  ");

  lcd.setCursor(8, 1);
  lcd.print("d4:");
  lcd.setCursor(11, 1);
  lcd.print(d4);
  lcd.print("  ");
delay(1000);
}
