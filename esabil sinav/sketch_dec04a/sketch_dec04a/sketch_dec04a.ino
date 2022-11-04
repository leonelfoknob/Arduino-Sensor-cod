//Receiver Code

/*
* Arduino Wireless Communication Tutorial
*       Example 1 - Receiver Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* 
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.begin (16, 2);
  //Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    lcd.setCursor(1, 0);
    lcd.print(text);
    lcd.setCursor(1, 1);
    lcd.print("                  ");
    //Serial.println(text);
    delay(1000);
  }
    else{
    lcd.setCursor(1, 0);
    lcd.print("   veri yok");
    lcd.setCursor(1, 1);
    lcd.print("ver kontrol edin");
    }
}
