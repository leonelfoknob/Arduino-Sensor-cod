/*
 * 3.3v and gnd use 10 nanofarad condansator and if you want change ce and cns pin
 * NRF24L01  PNA/LNA Arduino uno and Arduino nano connection pin
     ----------------
GND |GND         VCC | 3.3V        
7   |CE          CNS | 8    ---------   
13  |SCK         MOSI| 11      ANTEN  |
12  |MISO        IRQ |      ---------
     ----------------

* NRF24L01 Arduino uno and Arduino nano connection pin
     ----------------
GND |GND         VCC | 3.3V        
7   |CE          CNS | 8    ---  
13  |SCK         MOSI| 11     | ANTEN
12  |MISO        IRQ |      ---
     ----------------
*/
//alıcı 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 20, 4);

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

int data[6];

int data1;
int data2;
int data3;
int data4;
int data5;
int data6;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.begin (20, 4);
  //Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}
void loop() {
  if (radio.available()) {
    radio.read(data, sizeof(data));

   data1 = data[0];
   data2 = data[1];
   data3 = data[2];
   data4 = data[3];
   data5 = data[4];
   data6 = data[5];

   lcd.setCursor(0, 0);
   lcd.print("data1:");
   lcd.setCursor(7, 0);
   lcd.print(data1);

   lcd.setCursor(10, 0);
   lcd.print("data2:");
   lcd.setCursor(16, 0);
   lcd.print(data2);

   lcd.setCursor(0, 1);
   lcd.print("data3:");
   lcd.setCursor(7, 1);
   lcd.print(data3);

   lcd.setCursor(10, 1);
   lcd.print("data4:");
   lcd.setCursor(16, 1);
   lcd.print(data4);

   lcd.setCursor(0, 2);
   lcd.print("data5:");
   lcd.setCursor(7, 2);
   lcd.print(data5);

   lcd.setCursor(10, 2);
   lcd.print("data6:");
   lcd.setCursor(16, 2);
   lcd.print(data6);
   delay(20);
   
/*

    
    /*char text[32] = "";
    radio.read(&text, sizeof(text));
    lcd.setCursor(1, 0);
    lcd.print(text);
    lcd.setCursor(1, 1);
    lcd.print("                  ");
    //Serial.println(text);
    delay(1000);*/
  }
    else{
    lcd.setCursor(1, 0);
    lcd.print("   veri yok");
    lcd.setCursor(1, 1);
    lcd.print("ver kontrol edin");
    delay(1000);
    lcd.clear();
    delay(1000);
    }
}
