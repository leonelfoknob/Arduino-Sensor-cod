//transmitter code-----> dht11 data's send by nrf24l01
/*
*     Example 1 - Transmitter Code
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";
void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.println(F("DHTxx test!"));
  dht.begin();
}
void loop() {
   delay(1000);
  float h = dht.readHumidity();
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
  Serial.println(F("°F"));
  int tp = t; // convert float to integer but i dont use it in this code
  char temper[5];// creer une variable charactere qui va stoker la valeur a envoyer par radio
  String temp = String(t); // converti la temperature (float) en chaine de charactere
  temp.toCharArray(temper,5); // comverti notre chaine de charactere en charactere
  radio.write(&temper, sizeof(temper));
  //const char text[] = "Hello World";
  //radio.write(&text, sizeof(text));
  //delay(1000);
}
