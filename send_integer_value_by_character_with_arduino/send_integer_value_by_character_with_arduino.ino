int h = 36; //intrger value1
int t = 50; //intrger value1
float p = 3.22; //intrger value1


void setup() {
  Serial.begin(9600); //serial monitor begin

}

void loop() {
  /************************ HOW send integer value******************/
  String hum = String(h) ; //convert 1.value integer to string 
  String tem = String(t) ; //convert 2.value integer to string 
  String pre = String(p) ; //convert 2.value integer to string 
  
  String command(hum + " " + tem + " " + pre); //concatanation of 1,2 and 3 value to one string
  char gidendegen[11]; // create a char table which will store our concatanate string value to send it char by char
  command.toCharArray(gidendegen,11); // convert our string value to char table for send it char by char
  Serial.print("sending value : ");
  Serial.println(gidendegen); // print our value as receiver will receive it
  Serial.print("sending value end----------------------> ");
  for(int i=0 ; i<11;i++){
      Serial.println(gidendegen[i]);
  }
}
