//ultrasonic sensor on--------------
int trig_on =13 ;
int echo_on =12 ;
//ultrasonic sensor back----------------
int trig_back =22 ;
int echo_back =23 ;
//ultrasonic sensor left-------------------
int trig_left =24 ;
int echo_left =25 ;
//ultrasonic sensor back------------------------
int trig_right =27 ;
int echo_right =26 ;

long temps;
float distance;

void setup() {
  pinMode(trig_on,OUTPUT);
  pinMode(echo_on,INPUT);
  pinMode(trig_back,OUTPUT);
  pinMode(echo_back,INPUT);
  pinMode(trig_left,OUTPUT);
  pinMode(echo_left,INPUT);
  pinMode(trig_right,OUTPUT);
  pinMode(echo_right,INPUT);
  digitalWrite(trig_on,LOW);
  digitalWrite(trig_back,LOW);
  digitalWrite(trig_left,LOW);
  digitalWrite(trig_right,LOW);
  Serial.begin(9600);

}

void loop() {
  int distance_on = distance_value(trig_on , echo_on);
  int distance_back = distance_value(trig_back , echo_back);
  int distance_left = distance_value(trig_left , echo_left);
  int distance_right = distance_value(trig_right , echo_right);

  //print onsensor distance---------------------------------------
  Serial.print("distance_on : ");
  Serial.print(distance_on);
  Serial.print(" | ");
  //print back sensor distance----------------------------------
  Serial.print("distance_back : ");
  Serial.print(distance_back);
  Serial.print(" | ");
  //print left sensor distance----------------------------------
  Serial.print("distance_left : ");
  Serial.print(distance_left);
  //print right sensor distance---------------------------------
  Serial.print(" | ");
  Serial.print("distance_right : ");
  Serial.println(distance_right);
}




float distance_value(int trig_pin , int echo_pin){
  digitalWrite(trig_pin,HIGH);
  delayMicroseconds(1000);
  digitalWrite(trig_pin,LOW);
  temps=pulseIn(echo_pin,HIGH);
  temps=temps/2;
  distance=(3400*temps)/100000;
  distance = distance -2 ;
  //delay(50);
  return distance;
}
