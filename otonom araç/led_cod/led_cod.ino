int red_led_on_left = 52;
int green_led_on_left = 53;
int red_led_on_right =9 ;
int green_led_on_right =8 ;
int red_led_back_left = 50;
int green_led_back_left = 51;
int red_led_back_right =48 ;
int green_led_back_right =49 ;

int t1 = 3000;
int t2 = 2000;

void setup() {
  pinMode(red_led_on_left,OUTPUT);
  pinMode(green_led_on_left,OUTPUT);
  pinMode(red_led_on_right,OUTPUT);
  pinMode(green_led_on_right,OUTPUT);
  pinMode(red_led_back_left,OUTPUT);
  pinMode(green_led_back_left,OUTPUT);
  pinMode(red_led_back_right,OUTPUT);
  pinMode(green_led_back_right,OUTPUT);
}

void loop() {

  go_on_led();
  delay(t1);
  go_back_led();
  delay(t1);
  turn_right_led();
  delay(t1);
  turn_left_led();
  delay(t1);
  stop_led();
  delay(t2);

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
