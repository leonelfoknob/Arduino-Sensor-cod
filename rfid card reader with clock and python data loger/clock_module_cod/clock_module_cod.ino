   /*  
pin connection 

SDA --> A4
SCL --> A5
GND --> GND
VCC --> 5V

after connectiıon donlowd ibrary here : https://electropeak.com/learn/interfacing-ds3231-real-time-clock-rtc-module-with-arduino/
*/

#include <Wire.h>
#include <ds3231.h>
 
struct ts t; 
 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);
  /*----------------------------------------------------------------------------
  insert the now time and date value!
  ----------------------------------------------------------------------------*/
  t.hour=3; 
  t.min=0;
  t.sec=0;
  t.mday=14;
  t.mon=11;
  t.year=2020;
 
  DS3231_set(t); 
}

void loop() {
  DS3231_get(&t);
  Serial.print("Date : ");
  Serial.print(t.mday);
  Serial.print("/");
  Serial.print(t.mon);
  Serial.print("/");
  Serial.print(t.year);
  Serial.print("\t Hour : ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(".");
  Serial.println(t.sec);

  delay(1000);
}
