// code to concatenate string and divide(split) it
int a = 25;
int b = 35;
int c = 85;
int d = 50;

char serparator1 = '\n';
char serparator2 = ' ';
char serparator3 = '\t';
char serparator4 = ',';

void setup() {
  Serial.begin(9600);
}

void loop() {
  //convert integer value to string and character to string and concatenate it for one string
  String string_data = String(a)+String(serparator4)+String(b)+String(serparator4)+String(c)+String(serparator4)+String(d);
  Serial.println(string_data); //print concatenated string

  String data = getValue(string_data,serparator4, 3); //separate(split) string data for one one data 
  Serial.println(data);//print data
  int val4 = data.toInt();//convert data to integer 
  int top = val4+1;
  Serial.println(top);//print data+1
  delay(10000);
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
