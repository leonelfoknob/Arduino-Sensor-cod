#include "DHT.h"
#include <TFT.h>  
#include <SPI.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define cs   10
#define dc   9
#define rst  8 

DHT dht(DHTPIN, DHTTYPE);
TFT TFTscreen = TFT(cs, dc, rst);

char tep[8];
void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255,0,155);
  TFTscreen.setTextSize(2);
  TFTscreen.text("Temperature", 15, 1);
  TFTscreen.setTextSize(4);
  
}

void loop() {
  //ecrit le signe degré
  TFTscreen.stroke(255,255,255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("o", 140, 30);
  TFTscreen.setTextSize(4);

  //mesure la temperature e l'ecris
  float t = dht.readTemperature();
  TFTscreen.stroke(255,255,255);
  String temp = String(t);
  temp.toCharArray(tep, 8);
  TFTscreen.text(tep, 20,40 );
  delay(10000);  
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text(tep, 20, 40);
//ecrit MF
  TFTscreen.stroke(0,255,0);
  TFTscreen.setTextSize(3);
  TFTscreen.text("MF", 65, 90);
  TFTscreen.setTextSize(4);
//ecrit makinafleo
  TFTscreen.stroke(0,255,0);
  TFTscreen.setTextSize(1);
  TFTscreen.text("MakinaFleo", 50, 117);
  TFTscreen.setTextSize(4);
  //TFTscreen.text("text", x, y);

/****************** pin *************
LED --------------> 3.3 V
SCK --------------> 13
SDA --------------> 11
A0 or DC ---------> 9
RESET ------------> 8
CS -------------->  10
GND -------------> GND
VCC -------------> 5 V

**********************/

  
  
  /*float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
   Serial.println(F("Failed to read from DHT sensor!"));
   return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));*/
}
