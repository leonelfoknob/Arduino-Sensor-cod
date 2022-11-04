#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;
int d1;
int data2;
int data3;

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
  Serial.println("card initialized.");
}

void loop() {
  // kaydedecek olan datalar
  int data1 = d1++;
  data2 = data2+10;
  data3 = data3+20;
  //oluşturacak dosya ismin "datalog.txt"
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.print("data1:");dataFile.print(data1);
    dataFile.print("  data2:");dataFile.print(data2);
    dataFile.print("  data3:");dataFile.println(data3);
    dataFile.close();
    Serial.print("data1:");Serial.print(data1);
    Serial.print("  data2:");Serial.print(data2);
    Serial.print("  data3:");Serial.println(data3);
  }
  else {
    Serial.println("error opening datalog.txt");
  }
  delay(1000);//delay put by me you csn change it
}
