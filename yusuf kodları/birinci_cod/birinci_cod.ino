
void setup() {
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(13,OUTPUT);

}

void loop() {
  digitalWrite(10,1);
  digitalWrite(11,0);
  digitalWrite(12,0);
  digitalWrite(13,0);
  delay(500);
  digitalWrite(10,0);
  digitalWrite(11,1);
  digitalWrite(12,0);
  digitalWrite(13,0);
  delay(500);
}
