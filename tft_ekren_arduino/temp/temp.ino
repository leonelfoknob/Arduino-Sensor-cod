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

char tep[4];
void setup() {
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  dht.begin();
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.setTextSize(2);
}

void loop() {
  delay(1000);
  float t = dht.readTemperature();
  int redRandom = random(0, 255);
  int greenRandom = random (0, 255);
  int blueRandom = random (0, 255);
  TFTscreen.stroke(redRandom, greenRandom, blueRandom);
  TFTscreen.text("Temperature :", 1, 1);
  String temp = String(t);
  temp.toCharArray(tep, 8);
  TFTscreen.text(tep, 6,57 );

  
  
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
