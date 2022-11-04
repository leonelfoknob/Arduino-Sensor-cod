//ultrasoic sensor
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

//ledler
int red_led_on_left = 52;
int green_led_on_left = 53;
int red_led_on_right =9 ;
int green_led_on_right =8 ;
int red_led_back_left = 50;
int green_led_back_left = 51;
int red_led_back_right =48 ;
int green_led_back_right =49 ;

//ultrasonic sensor on--------------
/*int trig_on = ;
int echo_on = ;
//ultrasonic sensor back----------------
int trig_back = ;
int echo_back = ;
//ultrasonic sensor left-------------------
int trig_left = ;
int echo_left = ;
//ultrasonic sensor back------------------------
int trig_right = ;
int echo_right = ;*/
//Weel left-------------------
int ena =2 ;//right side
int IN1 =3 ;
int IN2 =4 ;
//Wee lrigth--------------------------
int enb =7 ;//left side
int IN3 =5 ;
int IN4 =6 ;
// wheel speed value---------------------
int speed_value = 150;
//--------------------------------------------------> setup function <---------------------------------
void setup(){
  //utrasonic sensor
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
  //ledler
  pinMode(red_led_on_left,OUTPUT);
  pinMode(green_led_on_left,OUTPUT);
  pinMode(red_led_on_right,OUTPUT);
  pinMode(green_led_on_right,OUTPUT);
  pinMode(red_led_back_left,OUTPUT);
  pinMode(green_led_back_left,OUTPUT);
  pinMode(red_led_back_right,OUTPUT);
  pinMode(green_led_back_right,OUTPUT);
  //tekeler
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
}
//--------------------------------------------------> loop function <---------------------------------
void loop() {
  int distance_on = distance_value(trig_on , echo_on);
  int distance_back = distance_value(trig_back , echo_back);
  int distance_left = distance_value(trig_left , echo_left);
  int distance_right = distance_value(trig_right , echo_right);
if(distance_on < 10){
    back();
  delay(300);
  left();
  delay(300);
}
else{
  forward();
}
  /*
  forward();
  delay(2000);
  stoped();
  delay(1000);
  back();
  delay(2000);
  stoped();
  delay(1000);
  left();
  delay(2000);
  stoped();
  delay(1000);
  right();
  delay(2000);
  stoped();
  delay(1000);
  stoped();
  delay(5000);
  */
}

// go forward fonction--------------------
void forward(){
  analogWrite(ena,speed_value);
  analogWrite(enb,speed_value);
  digitalWrite(IN1,1);
  digitalWrite(IN2,0);
  digitalWrite(IN3,1);
  digitalWrite(IN4,0);
  go_on_led();
}
// go back fonction-------------------
void back(){
  analogWrite(ena,speed_value);
  analogWrite(enb,speed_value);
  digitalWrite(IN1,0);
  digitalWrite(IN2,1);
  digitalWrite(IN3,0);
  digitalWrite(IN4,1);
  go_back_led();
}
//turn left function--------------
void left(){
  analogWrite(ena,speed_value);
  analogWrite(enb,75);
  digitalWrite(IN1,1);
  digitalWrite(IN2,0);
  digitalWrite(IN3,1);
  digitalWrite(IN4,0);
  turn_left_led();
}
//turn rigth fonction------------------
void right(){
  analogWrite(ena,75);
  analogWrite(enb,speed_value);
  digitalWrite(IN1,1);
  digitalWrite(IN2,0);
  digitalWrite(IN3,1);
  digitalWrite(IN4,0);
  turn_right_led();
}
//stop fonction-------------------
void stoped(){
  analogWrite(ena,0);
  analogWrite(enb,0);
  digitalWrite(IN1,0);
  digitalWrite(IN2,0);
  digitalWrite(IN3,0);
  digitalWrite(IN4,0);
  stop_led();
}
//led_go_on
void go_on_led(){
  digitalWrite(green_led_on_left,1);
  digitalWrite(green_led_on_right,1);
  digitalWrite(green_led_back_left,1);
  digitalWrite(green_led_back_right,1);
  
  digitalWrite(red_led_on_left,0);
  digitalWrite(red_led_on_right,0);
  digitalWrite(red_led_back_left,0);
  digitalWrite(red_led_back_right,0);
}
//led_go_back
void go_back_led(){
  digitalWrite(green_led_on_left,0);
  digitalWrite(green_led_on_right,0);
  digitalWrite(green_led_back_left,1);
  digitalWrite(green_led_back_right,1);
  
  digitalWrite(red_led_on_left,1);
  digitalWrite(red_led_on_right,1);
  digitalWrite(red_led_back_left,1);
  digitalWrite(red_led_back_right,1);
}
//led_turn right
void turn_right_led(){
  digitalWrite(green_led_on_left,0);
  digitalWrite(green_led_on_right,1);
  digitalWrite(green_led_back_left,0);
  digitalWrite(green_led_back_right,1);
  
  digitalWrite(red_led_on_left,0);
  digitalWrite(red_led_on_right,0);
  digitalWrite(red_led_back_left,0);
  digitalWrite(red_led_back_right,0);
}
//led_turn_left
void turn_left_led(){
  digitalWrite(green_led_on_left,1);
  digitalWrite(green_led_on_right,0);
  digitalWrite(green_led_back_left,1);
  digitalWrite(green_led_back_right,0);
  
  digitalWrite(red_led_on_left,0);
  digitalWrite(red_led_on_right,0);
  digitalWrite(red_led_back_left,0);
  digitalWrite(red_led_back_right,0);
}
//led_stop
void stop_led(){
  digitalWrite(green_led_on_left,0);
  digitalWrite(green_led_on_right,0);
  digitalWrite(green_led_back_left,0);
  digitalWrite(green_led_back_right,0);
  
  digitalWrite(red_led_on_left,1);
  digitalWrite(red_led_on_right,1);
  digitalWrite(red_led_back_left,1);
  digitalWrite(red_led_back_right,1);;
}
//ultrasonic sensor cod
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
