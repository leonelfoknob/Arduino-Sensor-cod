#include<Servo.h>

Servo servo1;
Servo servo2;

#define xPin A3
int xDeger = 0, derece=0;

void setup() {
  servo1.attach(7);
  servo2.attach(3);
  servo1.write (0); 
  servo2.write (0);

}

void loop() {
  xDeger = analogRead (xPin);  
  derece = map (xDeger,0 ,1023 ,0, 180);
  servo1.write (derece-90); 
  servo2.write (derece+90);
}
