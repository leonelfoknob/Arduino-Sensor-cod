/*
 ********* For Arduino mega ********
 */
//sender from arduino mega to arduino uno
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);
SoftwareSerial HC12(10, 11);//tx,rx

float s1;
float s2;
float s3;

float r1;
float r2;
float r3;

String Receive;
char serparator = ',';

void setup() {
  HC12.begin(9600);//Sets the hc 12 communication to 9600 baud, which is their factory default
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.begin (16, 2);
}
void loop() {
  s1 = s1+0.5;
  s2 ++;
  s3 = s3+1.5;

  Serial.println("Sended...");
  Serial.print("S1 : ");Serial.println(s1);
  Serial.print("S2 : ");Serial.println(s2);
  Serial.print("S3 : ");Serial.println(s3);
  Serial.println(" ");
  
//----------->send  
  HC12.print(s1);//sends the variables
  HC12.print(",");
  HC12.print(s2);
  HC12.print(",");
  HC12.print(s3);
  HC12.println("");
  //delay(1);
//----------->receive 
  if(HC12.available()){
  Receive = HC12.readStringUntil('\n');
  if (Receive.length() > 0){
        r1 = (getValue(Receive,serparator, 0)).toFloat();
        r2 = (getValue(Receive,serparator, 1)).toFloat();
        r3 = (getValue(Receive,serparator, 2)).toFloat();

        Serial.println("Received...");
        Serial.print("Receive : ");Serial.println(Receive);
        Serial.println(" ");
        Serial.print("R1 : ");Serial.println(r1);
        Serial.print("R2 : ");Serial.println(r2);
        Serial.print("R3 : ");Serial.println(r3);
        Serial.println(" ");
        
//delay(1);
      }
  }

     //lcd print
   lcd.setCursor(0, 0);
   lcd.print("R:");
   lcd.setCursor(2, 0);
   lcd.print(Receive);

   lcd.setCursor(0, 1);
   lcd.print("S1:");
   lcd.setCursor(3, 1);
   lcd.print(s1);
   delay(1000);
}

//function to separate string separate by one charatere for each position's data
String getValue(String data, char separator, int index){
int found = 0;
int strIndex[] = {0, -1};
int maxIndex = data.length()-1;
for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
    found++;
    strIndex[0] = strIndex[1]+1;
    strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
}
return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}
