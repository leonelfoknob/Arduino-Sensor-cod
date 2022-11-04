
int buzer = 2;

void setup() {
  pinMode(buzer,OUTPUT);
}

void loop() {
  digitalWrite(buzer,1);
  delay(200);
  digitalWrite(buzer,0);
  delay(5000);

}
