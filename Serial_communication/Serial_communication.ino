// Leonel serial communication

char coming_byte;

void setup() {
  Serial.begin(9600); // begin Serial communication to 9600 baud
   while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
}

void loop() {
   if (Serial.available() > 0) {
    coming_byte = Serial.read();// read the incoming byte:
    // say what you got:
    Serial.print("I received: ");
    Serial.println(coming_byte, DEC);
  }
}
