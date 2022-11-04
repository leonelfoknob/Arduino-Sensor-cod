//serial receive string data and split it
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

int r1;
int r2;
int r3;
int r4;
char separator =',';
String receive_string_data;

  /*int d1;
  int d2;
  int d3;
  int d4;*/

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.begin (16, 2);
}

void loop() {
  /*d1=d1+1;
  d2=d2+2;
  d3=d3+3;
  d4=d4+4;
  receive_string_data = String(d1)+String(separator)+String(d2)+String(separator)+String(d3)+String(separator)+String(d4);
  Serial.print("receive_string_data :");
  Serial.println(receive_string_data);*/
  /**********************************/
  if (Serial.available() > 0) {
  receive_string_data = Serial.readString();
  String receive_data1 = getValue(receive_string_data,separator, 0);
  String receive_data2 = getValue(receive_string_data,separator, 1);
  String receive_data3 = getValue(receive_string_data,separator, 2);
  String receive_data4 = getValue(receive_string_data,separator, 3);
  
  r1 = receive_data1.toInt();//convert data to integer
  r2 = receive_data2.toInt(); 
  r3 = receive_data3.toInt(); 
  r4 = receive_data4.toInt();
  
  /*Serial.print("r1: ");Serial.println(r1);
  Serial.print("r2: ");Serial.println(r2);
  Serial.print("r3: ");Serial.println(r3);
  Serial.print("r4: ");Serial.println(r4);*/

  /**********************************/

  lcd.setCursor(0, 0);
  lcd.print("r1:");
  lcd.setCursor(3, 0);
  lcd.print(r1);
  lcd.print("  ");

  lcd.setCursor(8, 0);
  lcd.print("r2:");
  lcd.setCursor(11, 0);
  lcd.print(r2);
  lcd.print("  ");

  lcd.setCursor(0, 1);
  lcd.print("r3:");
  lcd.setCursor(3, 1);
  lcd.print(r3);
  lcd.print("  ");

  lcd.setCursor(8, 1);
  lcd.print("r4:");
  lcd.setCursor(11, 1);
  lcd.print(r4);
  lcd.print("  ");
  }

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
