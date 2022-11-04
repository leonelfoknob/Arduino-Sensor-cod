
#include <USB_CH376s.h>

#include <EEPROM.h>

//#include <hidboot.h>
//#include <usbhub.h>

#include <hid.h>  
#include <hiduniversal.h>  
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <hidboot.h>

#include <PID_v1.h>
#include <math.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "RTClib.h"  
//--------------
enum
{
  STRING_MAIN_MENU,
  STRING_MAIN_MENU2,
  STRING_WELDING,
  STRING_MANUAL,
  STRING_SECONDS,
  STRING_JOULES,
  SRTING_ERROR_TRY_AG,
  STRING_SEC,
  STRING_AUTO,
  STRING_CLK_OK_R,
  STRING_S_CIRCUIT,
  STRING_O_CIRCUIT,
  STRING_MAIN_V_ERROR,
  STRING_OVERHEAT_ERROR,
  STRING_WELDING_F,
  STRING_EMER_STOP,
  STRING_OK_RESTART,
  STRING_COOLING_T,
  STRING_MINS,
  STRING_SEL_SEC,
  STRING_OK_CONTINUE,
  STRING_SEL_VOLT,
  STRING_OK_START,
  STRING_RESET_TO_R,
  STRING_OK_TIME,
  STRING_PREV_LANG,
  STRING_NEXT_MEM,
  STRING_R_MAIN_MENU,
  STRING_OK_SAVE,
  STRING_MEM,
  STRING_OK_WRITE,
  STRING_NEXT_DEL,
  STRING_LANG,
  STRING_MEM_FULL,
  STRING_MEM_SET,
  STRING_USB_ERROR,
  STRING_USB_NOT_CON,
  STRING_WRITE_USB,
  STRING_ERROR,
  STRING_ERROR_OHM,
  STRING_VOUT_ERROR,
  STRING_OK_START_RESET,
  STRING_OK_AUTO,
  STRING_RES,
  STRING_MANU_BARCODE
};
enum
{
  LANGUAGE_ENGLISH,
  LANGUAGE_TURKISH,
  LANGUAGE_SPANISH,
  LANGUAGE_FRENCH
};
int languageSettings;

unsigned int LANGUAGE_EEPROM_ADRESS=1;
//--------------
int eepromDelay=2;
int EEPROM_DEVICE_ADRESS=0x50;
unsigned int MEMORY_COUNT_ADRESS=0;
unsigned int MEMORY_UNIT_SIZE=50;
unsigned int MEMORY_FULL_COUNT=4000;
unsigned int memory_count;

unsigned int MEMORY_ADRESS_OFFSET=5;

typedef union
{
  float number;
  uint8_t bytes[4];
} FLOATUNION_t;

enum
{
  WELDING_OK,
  WELDING_NOK,
  WELDING_OHM_ERROR,
  WELDING_STOPPED
};
uint8_t weldingStatus = WELDING_OK;
                                                      int barcode[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//--------------
RTC_DS1307 RTC;
//---------------
USB_CH376s usb2(&Serial3, &Serial);
String USB_FILE_NAME="log.txt";
//---------------
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp = 1.0, Ki = 7.0, Kd = 0.1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

double st=20.0;
double testOhm=1.0; 
int mainVoltMin=487;
//---------------
bool weldingInterruptStopped=false;
//---------------
int tempPin = A5;
int temperature;
int t_koef_1;
int t_koef_2;
//change 361 (ohms) &384 (seconds) &693 (volts)
int mainVoltagePin = A8;
float mainVoltage;

int overheatPin = A4;
int runPin = 11;
int errorPin = 12;
int startPin = 27;

double PWMMax=180.0;
double vKoef=0.142;
double ampKoef=0.079;
float Joules=0.0;
float totalJoules=0.0;

int mainV;
//---------------
enum
{
  STATE_MAIN_MENU,
  STATE_MANUAL_SELECTION,
  STATE_BARCODE_READING,
  STATE_READ_BARCODE_VALUES,
  STATE_WAIT_AUTO_CONFIRM,
  STATE_WELDING,
  STATE_WELDING_FINISHED,
  STATE_ERROR,
  STATE_SETTINGS,
  STATE_SETTINGS_TIME,
  STATE_SETTINGS_LANGUAGE,
  STATE_SETTINGS_MEMORY,
  STATE_SETTINGS_MEMORY_FULL,
  STATE_COOLING,
  STATE_COOLING_FINISHED,
  STATE_MANUAL_BARCODE
};
uint8_t state = STATE_MAIN_MENU;

enum
{
  MANUAL_SELECTION_SECONDS,
  MANUAL_SELECTION_VOLTS,
  MANUAL_SELECTION_READY
};
uint8_t manualSelectionState = MANUAL_SELECTION_SECONDS;
//---------------
#define PIN_OUTPUT 13

int sensorVPin = A7;
int sensorAPin = A6;
int sensorValue = 0;

int usbResetPin = A0;


float volt = 0.0;
float amp = 0.0;

// The LCD constructor - address shown is 0x27 - may or may not be correct for yours
// Also based on YWRobot LCM1602 IIC V1
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
//---------------
//buttons
#define PIN_BUTTON_OK 7
#define PIN_BUTTON_DEC 5
#define PIN_BUTTON_INC 4
#define PIN_BUTTON_BEF 3
#define PIN_BUTTON_NXT 6
#define PIN_BUTTON_RESET 2

#define NOT_PUSHED 0
#define MAYBE_PUSHED 1
#define PUSHED 2
#define MAYBE_RELEASED 3

uint8_t okBtnState = NOT_PUSHED;
uint8_t decBtnState = NOT_PUSHED;
uint8_t incBtnState = NOT_PUSHED;
uint8_t befBtnState = NOT_PUSHED;
uint8_t nxtBtnState = NOT_PUSHED;
uint8_t resetBtnState = NOT_PUSHED;
//---------------
//bar code reader
uint8_t barcodeReaderIndex = 0;
char barCodeChars[24];
class KbdRptParser : public KeyboardReportParser
{
  protected:
    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyPressed(uint8_t key);
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  //Serial.println(char(key));
  if (state != STATE_BARCODE_READING) {
    return;
  }
  if (key == 19) {
    state = STATE_READ_BARCODE_VALUES;
  } else {
    if (barcodeReaderIndex < 24) {
      barCodeChars[barcodeReaderIndex] = char(key);
    }
    barcodeReaderIndex++;
    //Serial.println(char(key));
  }
};

///////////////////////////////////////////////////////////////////////
USB     Usb;
USBHub     Hub(&Usb); //I enable this line
///////////////////////////////////////////////////////////////////////
//USB     Usb;
////USBHub     Hub(&Usb);
//HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

HIDUniversal      Hid(&Usb);                                  //Add this line so that the barcode scanner will be recognized, I use "Hid" below 
HIDBoot<HID_PROTOCOL_KEYBOARD>    Keyboard(&Usb);

uint32_t next_time;

KbdRptParser Prs;

bool barcodeReaderReady = false;
//--------------------------
#define MODE_AUTO 0
#define MODE_MANUAL 1

uint8_t mode = MODE_AUTO;
//-------------------------
#define CASE_A 0
#define CASE_B 1

uint8_t autoCase = CASE_A;
int coolingTime=0;
//--------------------------
float volts;
double ohms;
int seconds;

int inputVolts;
int inputAmps;

int manualVolts=20;
int manualSeconds=10;
//--------------------------
int numOfOhmMeasurements = 0;
float measuredOhms;

float ohmVolt=0.0;
float ohmAmp=0.0;

unsigned long startTime;
bool isOhmMeasured = false;
int currentWeldingSeconds = 0;
int totalWeldingSeconds = 0;
int secondsLeft = 0;
//--------------------------
unsigned long lastMainMenuTimeUpdateMS=0;
//--------------------------
unsigned int settingsYear;
unsigned int settingsMonth;
unsigned int settingsDay;
unsigned int settingsHour;
unsigned int settingsMinute;

unsigned int settingsTimePosition=0;
                                                                          unsigned int settingsIndexPosition=0;
                                                                          unsigned int secondsCursorIndex=3;
//--------------------------
int coolingSecondsLeft;
unsigned long coolingStartMS=0;
//--------------------------
void setup() {
  Serial.begin(115200);
  Serial3.begin(115200);
 
  //---------------
  Wire.begin();
  //-------------
  myPID.SetMode(AUTOMATIC);
  //---------------
  //buttons
  pinMode(PIN_BUTTON_OK, INPUT_PULLUP);
  pinMode(PIN_BUTTON_DEC, INPUT_PULLUP);
  pinMode(PIN_BUTTON_INC, INPUT_PULLUP);
  pinMode(PIN_BUTTON_BEF, INPUT_PULLUP);
  pinMode(PIN_BUTTON_NXT, INPUT_PULLUP);
  pinMode(PIN_BUTTON_RESET, INPUT_PULLUP);
  //--------------
  pinMode(overheatPin, INPUT_PULLUP);
  pinMode(runPin, OUTPUT);
  digitalWrite(runPin, LOW);
  pinMode(startPin, OUTPUT);
  digitalWrite(startPin, LOW);
  pinMode(errorPin, OUTPUT);
  digitalWrite(errorPin, LOW);
  //---------------
  //usb reset pin
  pinMode(usbResetPin, OUTPUT);
  digitalWrite(usbResetPin, LOW);
  //---------------
  //barcode reader
  if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
    barcodeReaderReady = false;
  } else {
    Serial.println("Barcode reader is ok");
    barcodeReaderReady = true;
  }
  delay( 200 );
  //HidKeyboard.SetReportParser(0, &Prs);
  Hid.SetReportParser(0, &Prs);
  //---------------
  //lcd
  lcd.begin(20, 4);
  //startAuto();
  //---------------
  analogWrite(PIN_OUTPUT, invertPWM(0));
  //---------------
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  //---------------
  int langSetByte=EEPROM.read(LANGUAGE_EEPROM_ADRESS);
  if(langSetByte!=0){
    languageSettings=LANGUAGE_TURKISH;
  } else {
    languageSettings=LANGUAGE_ENGLISH;
  }
  //---------------
  state = STATE_MAIN_MENU;
  initShowMainMenu();
  //---------------
}

void loop() {
  Usb.Task();
  //Serial.println(state);
  //Serial.println(STATE_MANUAL_SELECTION);
  if (state == STATE_WELDING) {

    if (digitalRead(PIN_BUTTON_RESET) == LOW) {
      weldingStatus = WELDING_STOPPED;
      stopWelding();
      weldingInterruptStopped=true;
      return;
    }
    
    unsigned long currentTime = millis() - startTime;
    
    //if (currentWeldingSeconds > seconds) {
    if (totalJoules > Joules && mode == MODE_AUTO) {
      weldingStatus = WELDING_OK;
      stopWelding();
      showStopWeldingData();
    } else if(currentWeldingSeconds > seconds && mode == MODE_MANUAL){
      stopWelding();
      showStopWeldingData();
    } else {
      
      volt=0.0;
      amp=0.0;
      for(int i=0;i<130;i++){
        volt +=analogRead(sensorVPin)*vKoef;
        amp +=analogRead(sensorAPin)*ampKoef;  
      }
      volt/=130;
      amp/=130;
      //Serial.print("V:");
      //Serial.println(volt);
      if(!isOhmMeasured){
        
        if(volt>(18+volt/20)){
          ohmVolt=0.0;
          ohmAmp=0.0;
          for(int i=0; i<130; i++){
            ohmVolt +=analogRead(sensorVPin);     
            ohmAmp +=analogRead(sensorAPin);  
          }
          ohmVolt/=130;
          ohmVolt*=vKoef;
          //Serial.print("ohmVolt:");
          //Serial.println(ohmVolt);
          ohmAmp/=130;
          ohmAmp*=ampKoef;
          //Serial.print("ohmAmp:");
          //Serial.println(ohmAmp);

          measuredOhms = ohmVolt/ohmAmp;
          Serial.print("ohm:");
          Serial.println(measuredOhms);
          double errorMargin = ohms *0.75;
          if (measuredOhms > (ohms + errorMargin) || measuredOhms < (ohms - errorMargin)) {
            //weldingStatus = WELDING_OHM_ERROR;
            //stopWelding();
            //showOhmError();
            //state = STATE_ERROR;
            //return;
          }
          showWeldingInfo();
          isOhmMeasured=true;
        }
      }
      if(amp>55.0){
        Input = Setpoint+(amp-55.0)*1.2;
      } else {
        Input = volt;
      }
      myPID.Compute();
      //Serial.println(PWMMax);
      //Serial.println(Output);
      double pwmOut = Output;
      
      if (pwmOut > PWMMax) {
        pwmOut = PWMMax;
      }
      
     // Serial.println(pwmOut);
      analogWrite(PIN_OUTPUT, invertPWM(pwmOut));
      
      if ((currentTime/1000) > currentWeldingSeconds) {
        currentWeldingSeconds++;
        secondsLeft=totalWeldingSeconds - currentWeldingSeconds;
        if(secondsLeft<0){
          secondsLeft=0;
        }
        if(currentWeldingSeconds>0){
          if(volt<1.0){
            weldingStatus = WELDING_NOK;
            stopWelding();
            showVOutError();
            state = STATE_ERROR;
            return;
          }
        }

        ohmVolt=0.0;
        ohmAmp=0.0;
        for(int i=0; i<130; i++){
          ohmVolt +=analogRead(sensorVPin);     
          ohmAmp +=analogRead(sensorAPin);  
        }
        ohmVolt/=130;
        ohmVolt*=vKoef;
        ohmAmp/=130;
        ohmAmp*=ampKoef;
        //Serial.println(ohmAmp);
        if (ohmAmp>60.0) {
          weldingStatus = WELDING_NOK;
          stopWelding();
          showAmpError();
          state = STATE_ERROR;
          return;
        }

         if (ohmAmp<1.0) {
            weldingStatus = WELDING_NOK;
            stopWelding();
            showAmpError2();
            state = STATE_ERROR;
            return;
           }
          
        float anVal=analogRead(mainVoltagePin)*1.05;
        PWMMax=calculatePWMMax(anVal);
        //Serial.println(anVal);
        //Serial.println(PWMMax);
        mainVoltage= calculateMainVoltage(anVal);

        float currentJoules=ohmVolt*ohmVolt;
        currentJoules/=ohms;
        totalJoules+=currentJoules;
        //totalJoules/=1000.0;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(lcdString(STRING_WELDING));
        lcd.setCursor(0, 1);
        lcd.print(lcdString(STRING_SECONDS));
        lcd.print(secondsLeft);
        lcd.setCursor(0, 2);
        if(mode != MODE_MANUAL){
          lcd.print(lcdString(STRING_JOULES));
          lcd.print(totalJoules);
        }
        lcd.setCursor(0,3);
        lcd.print("Vmain=");
        lcd.print((int)(round)(mainVoltage));
        lcd.print(" Vout=");
        lcd.print((int)(round)(volt));

        
        Serial.print("joules=");
        Serial.println(totalJoules);
      }
    }
    return;
  }
  
  if (state == STATE_MANUAL_SELECTION) {
    checkButtons();
    delay(20);
  } else if (state == STATE_BARCODE_READING) {
    //Serial.println("reading");

    checkButtons();
    delay(20);
  } else if (state == STATE_READ_BARCODE_VALUES) {
    //Usb.Task();
    if (barcodeReaderIndex != 24) {

      Serial.println("error");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(lcdString(SRTING_ERROR_TRY_AG));
      state = STATE_BARCODE_READING;

    } else {

      t_koef_1=barCodeChars[21] - '0';
      t_koef_2=barCodeChars[22] - '0';
      
      lcd.clear();

      int volt1 = barCodeChars[12] - '0';
      int volt2 = barCodeChars[13] - '0';
      if(volt1==9 && volt2==9){
        volts = 39.5;
      } else {
        volts = volt1 * 10 + volt2;
      }
      //Serial.print("Volts:");
      //Serial.println(volts);
      
      int ohm1 = barCodeChars[14] - '0';
      int ohm2 = barCodeChars[15] - '0';
      int ohm3 = barCodeChars[16] - '0';
      int ohm_comma_place=barCodeChars[11] - '0';
      if(ohm_comma_place==1){
        ohms = ohm1*100 + ohm2 * 10 + ohm3;
      } else if(ohm_comma_place==2 || ohm_comma_place==7){
        ohms = ohm1*10 + ohm2 + ohm3 * 0.1;
      } else if(ohm_comma_place==3 || ohm_comma_place==8) {
        ohms = ohm1 + ohm2 * 0.1 + ohm3 * 0.01;
      }
      //Serial.print("Ohms:");
      //Serial.println(ohms);
      //lcd.setCursor(0, 1);
      //lcd.print("Ohms:");
      //lcd.print(ohms);

     //ohms = testOhm;

      int diam1 = barCodeChars[8] - '0';
      int diam2 = barCodeChars[9] - '0';
      int diam3 = barCodeChars[10] - '0';
      int diameter = diam1*100 + diam2 * 10 + diam3;
      //Serial.print("Ohms:");
      //Serial.println(ohms);
      lcd.setCursor(0, 1);
      lcd.print("D:");
      lcd.print(diameter);
      lcd.print(" mm");
      lcd.print("  U=");
      lcd.print(volts);
      lcd.print("V");
      
      int sec1 = barCodeChars[18] - '0';
      int sec2 = barCodeChars[19] - '0';
      int sec3 = barCodeChars[20] - '0';

      int timeEnergyControl= barCodeChars[6] - '0';
      if(timeEnergyControl>3){
        if(timeEnergyControl==4){
          Joules=(sec1*10+sec2)*pow(10.0,sec3);
        } else {
          Joules=(sec1 * 100 + sec2 * 10 + sec3)*pow(10.0,(timeEnergyControl-4));
        }
        totalWeldingSeconds=Joules*ohms/(volts * volts);
        lcd.setCursor(0, 2);
        lcd.print(Joules);
        lcd.print("J");
      } else {
        if(sec1<9){
          seconds = sec1 * 100 + sec2 * 10 + sec3;
        } else {
          seconds=(sec2*10+sec3)*60;
        }
        totalWeldingSeconds=seconds;
        //Serial.print("Seconds:");
        //Serial.println(seconds);
        Joules=volts * volts;
        Joules*=seconds;
        Joules/=ohms;
        lcd.setCursor(0, 2);
        lcd.print(seconds);
        lcd.print(lcdString(STRING_SEC));
        lcd.print(Joules);
        lcd.print("J");
      }

      int t_koef_22=barCodeChars[21] - '0';
      int t_koef_23=barCodeChars[22] - '0';
      int diffTemp=0;
      float diffPerc=0;
      if(temperature<20){
        diffTemp=20-temperature;
        diffPerc=0.1f*t_koef_22*diffTemp;
        seconds+=diffPerc*seconds/100;
        Joules+=diffPerc*Joules/100;
      } else if(temperature>20){
        diffTemp=temperature-20;
        diffPerc=0.1f*t_koef_23*diffTemp;
        seconds-=diffPerc*seconds/100;
        Joules-=diffPerc*Joules/100;
      }
      Serial.print("temp corected Joules:");
      Serial.println(Joules);
      //seconds = 8;
      
      lcd.setCursor(0, 0);
      int firstChar=barCodeChars[0] - '0';
      Serial.println(firstChar);
      if(firstChar==9){
        autoCase = CASE_B;
        lcd.print(getLabelChar(barCodeChars[2],barCodeChars[3]));
        lcd.print(getLabelChar(barCodeChars[4],barCodeChars[5]));
        lcd.print("  ");
        int secChar=barCodeChars[1] - '0';
        if(secChar==0){
          lcd.print("*");
        } else if(secChar==1){
          lcd.print("<RAYT");
        } else if(secChar==2){
          lcd.print("JT.D.W.");
        } else if(secChar==3){
          lcd.print("Y reduc");
        } else if(secChar==4){
          lcd.print(".t.tap.");
        } else if(secChar==5){
          lcd.print("I CPL");
        } else if(secChar==6){
          lcd.print("[ s SCKT");
        } else if(secChar==7){
          lcd.print("T TEES");
        } else if(secChar==8){
          lcd.print("C ELBOW");
        } else if(secChar==9){
          lcd.print("ERROR");
        }

        int char6=barCodeChars[6] - '0';
        if(char6==0 || char6==1 || char6==2){
          coolingTime=0;
        } else {
          int coolingChar=barCodeChars[7] - '0';
          if(coolingChar==0){
            coolingTime=5;
          } else if(coolingChar==1){
            coolingTime=10;
          } else if(coolingChar==2){
            coolingTime=15;
          } else if(coolingChar==3){
            coolingTime=20;
          } else if(coolingChar==4){
            coolingTime=30;
          } else if(coolingChar==5){
            coolingTime=45;
          } else if(coolingChar==6){
            coolingTime=60;
          } else if(coolingChar==7){
            coolingTime=75;
          } else if(coolingChar==8){
            coolingTime=90;
          } else if(coolingChar==9){
            coolingTime=0;
          }
        }

        lcd.setCursor(14, 0);
        lcd.print("CT:");
        lcd.print(coolingTime);
        
      } else {
        autoCase = CASE_A;
        coolingTime=0;
        lcd.print(getLabelChar(barCodeChars[0],barCodeChars[1]));
        lcd.print(getLabelChar(barCodeChars[2],barCodeChars[3]));
        lcd.print(getLabelChar(barCodeChars[4],barCodeChars[5]));
        lcd.print(getLabelChar(barCodeChars[6],barCodeChars[7]));
        lcd.print("  ");
        if(firstChar>5){
          lcd.print("J s SCKT");
        } else if(firstChar>2){
          lcd.print("I CPL");
        } else {
          lcd.print(".t.tap.");
        }

        lcd.setCursor(14, 0);
        lcd.print("CT:");
        lcd.print(coolingTime);
        
      }
      
      lcd.setCursor(0, 3);
      lcd.print(lcdString(STRING_OK_START_RESET));
      

      state=STATE_WAIT_AUTO_CONFIRM;
      //startWelding();
    }
    barcodeReaderIndex = 0;

  } else if (state == STATE_WAIT_AUTO_CONFIRM) {
    checkButtons();
    delay(20);
  } else if (state == STATE_WELDING_FINISHED) {
    if(weldingInterruptStopped){
      showStopWeldingData();
      weldingInterruptStopped=false;
    }
    checkButtons();
    delay(20);
    /*
    unsigned long currentTime = millis() - startTime;
    if (currentTime > (1000 * 3)) {
      startAuto();
    }
    */
  } else if (state == STATE_ERROR) {
    checkButtons();
    delay(20);
  } else if (state == STATE_MAIN_MENU) {
    showMainMenu();
    checkButtons();
    delay(20);
  }                                                           else if (state == STATE_MANUAL_BARCODE) {
                                                              //showManualBarcode();
                                                              checkButtons();
                                                              delay(20);
  } else if (state == STATE_SETTINGS) {
    //showSettings();
    checkButtons();
    delay(20);
  } else if (state == STATE_SETTINGS_TIME) {
    showTimeSettings();
    checkButtons();
    delay(20);
  } else if (state == STATE_SETTINGS_MEMORY) {
    checkButtons();
    delay(20);
  } else if (state == STATE_SETTINGS_MEMORY_FULL) {
    checkButtons();
    delay(20);
  } else if (state == STATE_SETTINGS_LANGUAGE) {
    checkButtons();
    delay(20);
  } else if (state == STATE_COOLING) {
    lcd.setCursor(9, 2);
    coolingSecondsLeft=(coolingTime*60)-((millis()-coolingStartMS)/1000);
    if(coolingSecondsLeft<1){
      state = STATE_COOLING_FINISHED;
      showCoolingFinished();
    } else {
      lcd.print(coolingSecondsLeft);
      lcd.print("     ");
    }
    delay(20);
  } else if (state == STATE_COOLING_FINISHED) {
    checkButtons();
    delay(20);
  } 
  
}

void checkButtons(void) {

  //ok button
  if (okBtnState == NOT_PUSHED) {
    if (digitalRead(PIN_BUTTON_OK) == LOW) {
      okBtnState = MAYBE_PUSHED;
    }
  } else if (okBtnState == MAYBE_PUSHED) {
    if (digitalRead(PIN_BUTTON_OK) == LOW) {
      okBtnState = PUSHED;
    } else {
      okBtnState = NOT_PUSHED;
    }
  } else if (okBtnState == PUSHED) {
    if (digitalRead(PIN_BUTTON_OK) == HIGH) {
      okBtnState = MAYBE_RELEASED;
    }
  } else if (okBtnState == MAYBE_RELEASED) {
    if (digitalRead(PIN_BUTTON_OK) == HIGH) {
      okBtnState = NOT_PUSHED;
      //Serial.println("ok click");
      okClick();
    } else {
      okBtnState = PUSHED;
    }
  }
  
  //reset button
  if (resetBtnState == NOT_PUSHED) {
    if (digitalRead(PIN_BUTTON_RESET) == LOW) {
      resetBtnState = MAYBE_PUSHED;
    }
  } else if (resetBtnState == MAYBE_PUSHED) {
    if (digitalRead(PIN_BUTTON_RESET) == LOW) {
      resetBtnState = PUSHED;
    } else {
      resetBtnState = NOT_PUSHED;
    }
  } else if (resetBtnState == PUSHED) {
    if (digitalRead(PIN_BUTTON_RESET) == HIGH) {
      resetBtnState = MAYBE_RELEASED;
    }
  } else if (resetBtnState == MAYBE_RELEASED) {
    if (digitalRead(PIN_BUTTON_RESET) == HIGH) {
      resetBtnState = NOT_PUSHED;
      //Serial.println("ok click");
      resetClick();
    } else {
      resetBtnState = PUSHED;
    }
  }

  //dec button
  if (decBtnState == NOT_PUSHED) {
    if (digitalRead(PIN_BUTTON_DEC) == LOW) {
      decBtnState = MAYBE_PUSHED;
    }
  } else if (decBtnState == MAYBE_PUSHED) {
    if (digitalRead(PIN_BUTTON_DEC) == LOW) {
      decBtnState = PUSHED;
    } else {
      decBtnState = NOT_PUSHED;
    }
  } else if (decBtnState == PUSHED) {
    if (digitalRead(PIN_BUTTON_DEC) == HIGH) {
      decBtnState = MAYBE_RELEASED;
    }
  } else if (decBtnState == MAYBE_RELEASED) {
    if (digitalRead(PIN_BUTTON_DEC) == HIGH) {
      decBtnState = NOT_PUSHED;
      decClick();
    } else {
      decBtnState = PUSHED;
    }
  }

  //inc button
  if (incBtnState == NOT_PUSHED) {
    if (digitalRead(PIN_BUTTON_INC) == LOW) {
      incBtnState = MAYBE_PUSHED;
    }
  } else if (incBtnState == MAYBE_PUSHED) {
    if (digitalRead(PIN_BUTTON_INC) == LOW) {
      incBtnState = PUSHED;
    } else {
      incBtnState = NOT_PUSHED;
    }
  } else if (incBtnState == PUSHED) {
    if (digitalRead(PIN_BUTTON_INC) == HIGH) {
      incBtnState = MAYBE_RELEASED;
    }
  } else if (incBtnState == MAYBE_RELEASED) {
    if (digitalRead(PIN_BUTTON_INC) == HIGH) {
      incBtnState = NOT_PUSHED;
      incClick();
    } else {
      incBtnState = PUSHED;
    }
  }

  //bef button
  if (befBtnState == NOT_PUSHED) {
    if (digitalRead(PIN_BUTTON_BEF) == LOW) {
      befBtnState = MAYBE_PUSHED;
    }
  } else if (befBtnState == MAYBE_PUSHED) {
    if (digitalRead(PIN_BUTTON_BEF) == LOW) {
      befBtnState = PUSHED;
    } else {
      befBtnState = NOT_PUSHED;
    }
  } else if (befBtnState == PUSHED) {
    if (digitalRead(PIN_BUTTON_BEF) == HIGH) {
      befBtnState = MAYBE_RELEASED;
    }
  } else if (befBtnState == MAYBE_RELEASED) {
    if (digitalRead(PIN_BUTTON_BEF) == HIGH) {
      befBtnState = NOT_PUSHED;
      befClick();
    } else {
      befBtnState = PUSHED;
    }
  }

  //nxt button
  if (nxtBtnState == NOT_PUSHED) {
    if (digitalRead(PIN_BUTTON_NXT) == LOW) {
      nxtBtnState = MAYBE_PUSHED;
    }
  } else if (nxtBtnState == MAYBE_PUSHED) {
    if (digitalRead(PIN_BUTTON_NXT) == LOW) {
      nxtBtnState = PUSHED;
    } else {
      nxtBtnState = NOT_PUSHED;
    }
  } else if (nxtBtnState == PUSHED) {
    if (digitalRead(PIN_BUTTON_NXT) == HIGH) {
      nxtBtnState = MAYBE_RELEASED;
    }
  } else if (nxtBtnState == MAYBE_RELEASED) {
    if (digitalRead(PIN_BUTTON_NXT) == HIGH) {
      nxtBtnState = NOT_PUSHED;
      nextClick();
    } else {
      nxtBtnState = PUSHED;
    }
  }

}

void okClick() {
  if (state == STATE_MAIN_MENU){
    startAuto();
  } else if (state == STATE_BARCODE_READING) {
    startManual();
  }

                                                                else if (state == STATE_MANUAL_BARCODE){
                                                                  for(int p=0;p<24;p++){
                                                                  barCodeChars[p]=barcode[p]+'0';
                                                                  barcodeReaderIndex=p+1;
                                                                  }
                                                                  state = STATE_READ_BARCODE_VALUES;}
                                                                
  
  else if(state==STATE_WAIT_AUTO_CONFIRM){
    memory_count=i2c_eeprom_read_byte(MEMORY_COUNT_ADRESS);
    Serial.println(memory_count);
    delay(100);
    if(memory_count>=MEMORY_FULL_COUNT){
      state = STATE_SETTINGS_MEMORY_FULL;
      showMemoryFull();
    } else {
      startWelding();
    }
  } else if (state == STATE_ERROR) {
    digitalWrite(errorPin, LOW);
    startAuto();
  } else if (state == STATE_MANUAL_SELECTION) {
    if (manualSelectionState == MANUAL_SELECTION_SECONDS) {
      startManualVoltsSelection();
    } else if (manualSelectionState == MANUAL_SELECTION_VOLTS) {
      showManualReadyForWelding();
    } else {
      startWelding();
    }
  } else if (state == STATE_WELDING_FINISHED) {
    startAuto();
  } else if(state == STATE_SETTINGS) {
    state = STATE_SETTINGS_TIME;
    initShowTimeSettings();
  } else if(state == STATE_SETTINGS_TIME) {
    saveTimeSettings();
  } else if(state == STATE_SETTINGS_MEMORY) {
    writeToUsb();
  } else if(state == STATE_SETTINGS_MEMORY_FULL){
    state = STATE_SETTINGS_MEMORY;
    initShowMemorySettings();
  } else if(state == STATE_COOLING_FINISHED){
    state = STATE_MAIN_MENU;
    startAuto();
  }
}

void resetClick() {
  Serial.println("reset click");
  if (state == STATE_BARCODE_READING) {
    state = STATE_MAIN_MENU;
    initShowMainMenu();
  } else if(state == STATE_MAIN_MENU) {
    state = STATE_SETTINGS;
    initShowSettings();
  }

                                                                              else if (state == STATE_MANUAL_BARCODE){
                                                                              startAuto();
                                                                              state = STATE_MAIN_MENU;}
  
  else if(state == STATE_SETTINGS) {
    state = STATE_MAIN_MENU;
    initShowMainMenu();
  } else if(state == STATE_SETTINGS_TIME) {
    state = STATE_MAIN_MENU;
    initShowMainMenu();
  } else if(state == STATE_SETTINGS_MEMORY) {
    state = STATE_MAIN_MENU;
    initShowMainMenu();
  } else if (state == STATE_MANUAL_SELECTION) {
    startAuto();
  } else if(state==STATE_WAIT_AUTO_CONFIRM){
    startAuto();
  } else if(state == STATE_SETTINGS_LANGUAGE) {
    state = STATE_MAIN_MENU;
    initShowMainMenu();
  }
}

void befClick() {
  Serial.println("bef click");
  if(state == STATE_SETTINGS) {
    state = STATE_SETTINGS_LANGUAGE;
    initShowLanguageSettings();
    showLanguageSettings();
  }                                                                   else if (state == STATE_MANUAL_BARCODE){
                                                                      if(settingsIndexPosition==0){
                                                                      settingsIndexPosition=24;}
                                                                      settingsIndexPosition--;
                                                                      showBarcodeSettings();}  

                                                                      else if(state == STATE_MANUAL_SELECTION){
                                                                      if (manualSelectionState == MANUAL_SELECTION_SECONDS){
                                                                      if(secondsCursorIndex>0){
                                                                      secondsCursorIndex--;
                                                                      }
                                                                      showSecodsCursor();
                                                                      }
                                                                      
                                                                      
                                                                        }
}

void nextClick() {
  Serial.println("next click");
  if(state == STATE_SETTINGS) {
    state = STATE_SETTINGS_MEMORY;
    initShowMemorySettings();}                                        else if (state == STATE_MANUAL_BARCODE){
                                                                      settingsIndexPosition++;
                                                                      if(settingsIndexPosition==24){
                                                                      settingsIndexPosition=0;}
                                                                      showBarcodeSettings();}                    
                                                                      else if (state == STATE_MAIN_MENU){
                                                                      state=STATE_MANUAL_BARCODE;
                                                                      settingsIndexPosition=0;
                                                                      showManualBarcode();
                                                                      showBarcodeSettings();
                                                                      lcd.setCursor(12,1);
                                                                      lcd.print(lcdString(STRING_OK_AUTO));
                                                                      lcd.setCursor(12,3);
                                                                      lcd.print(lcdString(STRING_RES));
                                                                      }

                                                                      else if(state == STATE_MANUAL_SELECTION){
                                                                      if (manualSelectionState == MANUAL_SELECTION_SECONDS){
                                                                      if(secondsCursorIndex<3){
                                                                      secondsCursorIndex++;
                                                                      }
                                                                      showSecodsCursor();
                                                                      }
                                                                      
                                                                      
                                                                        }


                                                                      
  else if(state == STATE_SETTINGS_TIME) {
    settingsTimePosition++;
    if(settingsTimePosition>4){
      settingsTimePosition=0;
    }
    showTimeSettings();
  } else if(state == STATE_SETTINGS_MEMORY) {
    clearMemory();
  } else if(state == STATE_SETTINGS_LANGUAGE) {
    if(languageSettings==LANGUAGE_ENGLISH){
      languageSettings=LANGUAGE_TURKISH;
      EEPROM.write(LANGUAGE_EEPROM_ADRESS,1);
    } else if(languageSettings==LANGUAGE_TURKISH){
      languageSettings=LANGUAGE_SPANISH;
      EEPROM.write(LANGUAGE_EEPROM_ADRESS,0);
    } else if(languageSettings==LANGUAGE_SPANISH){
      languageSettings=LANGUAGE_FRENCH;
      EEPROM.write(LANGUAGE_EEPROM_ADRESS,0);
    } else{
      languageSettings=LANGUAGE_ENGLISH;
      EEPROM.write(LANGUAGE_EEPROM_ADRESS,0);
    }
    showLanguageSettings();
  }

  
  
}

void decClick() {
  Serial.println("dec click");
  if (state == STATE_MANUAL_SELECTION) {
    if (manualSelectionState == MANUAL_SELECTION_SECONDS) {
      decreaseSeconds();
    } else if (manualSelectionState == MANUAL_SELECTION_VOLTS) {
      decreaseVolts();
    }
  }

                                                                                      if(state == STATE_MANUAL_BARCODE) {
                                                                                      if(barcode[settingsIndexPosition]==0){
                                                                                      barcode[settingsIndexPosition]=10;}
                                                                                      barcode[settingsIndexPosition]--;
                                                                                      showManualBarcode();
                                                                                      showBarcodeSettings();
                                                                                      lcd.setCursor(12,1);
                                                                                      lcd.print(lcdString(STRING_OK_AUTO));
                                                                                      lcd.setCursor(12,3);
                                                                                      lcd.print(lcdString(STRING_RES));
                                                                                      }
  
  else if(state == STATE_SETTINGS_TIME) {
    if(settingsTimePosition==0){
      if(settingsYear>2000){
        settingsYear--;
      }
    } else if(settingsTimePosition==1){
      if(settingsMonth>1){
        settingsMonth--;
      }
    } else if(settingsTimePosition==2){
      if(settingsDay>1){
        settingsDay--;
      }
    } else if(settingsTimePosition==3){
      if(settingsHour>0){
        settingsHour--;
      }
    } else if(settingsTimePosition==4){
      if(settingsMinute>0){
        settingsMinute--;
      }
    }
    showTimeSettings();
  }
}

void incClick() {
  Serial.println("inc click");
  if (state == STATE_MANUAL_SELECTION) {
    if (manualSelectionState == MANUAL_SELECTION_SECONDS) {
      increaseSeconds();
    } else if (manualSelectionState == MANUAL_SELECTION_VOLTS) {
      increaseVolts();
    }
  } 
  
                                                                                    if(state == STATE_MANUAL_BARCODE) {
                                                                                      barcode[settingsIndexPosition]++;
                                                                                      if(barcode[settingsIndexPosition]==10){
                                                                                      barcode[settingsIndexPosition]=0;}
                                                                                      showManualBarcode();
                                                                                      showBarcodeSettings();
                                                                                      lcd.setCursor(12,1);
                                                                                      lcd.print(lcdString(STRING_OK_AUTO));
                                                                                      lcd.setCursor(12,3);
                                                                                      lcd.print(lcdString(STRING_RES));
                                                                                      }
  
  else if(state == STATE_SETTINGS_TIME) {
    if(settingsTimePosition==0){
      settingsYear++;
    } else if(settingsTimePosition==1){
      if(settingsMonth<12){
        settingsMonth++;
      }
    } else if(settingsTimePosition==2){
      if(settingsDay<31){
        settingsDay++;
      }
    } else if(settingsTimePosition==3){
      if(settingsHour<23){
        settingsHour++;
      }
    } else if(settingsTimePosition==4){
      if(settingsMinute<59){
        settingsMinute++;
      }
    }
    showTimeSettings();
  }
}

void startAuto() {
  mode = MODE_AUTO;
  
  digitalWrite(errorPin, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_AUTO));
  
  temperature=0.0;
  for(int i=0;i<10;i++){
    temperature+= analogRead(tempPin);
  }
  temperature/=18.6;
  lcd.setCursor(0,1);
  lcd.print("T=");
  lcd.print(temperature);
  lcd.print(" C   V=");
  
  int anVal=analogRead(mainVoltagePin);
  mainVoltage= calculateMainVoltage(anVal);
  lcd.print((int)(round)(mainVoltage));
  lcd.print(" V");

  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_MANUAL));

  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_R_MAIN_MENU));
  
  barcodeReaderIndex = 0;
  state = STATE_BARCODE_READING;
}

                                                void showManualBarcode(){
                                                    lcd.clear();
                                                    lcd.setCursor(0, 0);
                                                    for(int p=0;p<=11;p++){
                                                    lcd.print(barcode[p]);}
                                                    
                                                    lcd.setCursor(0, 2);
                                                    for(int p=12;p<=23;p++){
                                                    lcd.print(barcode[p]);}
                                                  }

void showOhmError(){
  digitalWrite(errorPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ohms:");
  lcd.print(measuredOhms);
  lcd.setCursor(0, 1);
  lcd.print(lcdString(STRING_ERROR_OHM));
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_CLK_OK_R));
  lcd.setCursor(0, 3);
  lcd.setCursor(0, 2);
}

void showVOutError(){
  digitalWrite(errorPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_VOUT_ERROR));
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_CLK_OK_R));
  lcd.setCursor(0, 3);
  lcd.setCursor(0, 2);
}

void showAmpError(){
  digitalWrite(errorPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_S_CIRCUIT));
  lcd.setCursor(0, 1);
  lcd.print(lcdString(STRING_ERROR));
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_CLK_OK_R));
}

void showAmpError2(){
  digitalWrite(errorPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString( STRING_O_CIRCUIT));
  lcd.setCursor(0, 1);
  lcd.print(lcdString(STRING_ERROR));
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_CLK_OK_R));
}
//------------------------
void showCoolingFinished(){
  digitalWrite(errorPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_CLK_OK_R));
}
//------------------------
void startWelding() {
  mainV=analogRead(mainVoltagePin);
  if(mainV<mainVoltMin){
    digitalWrite(errorPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdString(STRING_MAIN_V_ERROR));
    lcd.setCursor(0, 2);
    lcd.print(lcdString(STRING_CLK_OK_R));
    state = STATE_ERROR;
    return;
  }
  if(digitalRead(overheatPin) == LOW){
    digitalWrite(errorPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdString(STRING_OVERHEAT_ERROR));
    lcd.setCursor(0, 2);
    lcd.print(lcdString(STRING_CLK_OK_R));
    state = STATE_ERROR;
    return;
  }
  weldingInterruptStopped=false;
  totalJoules=0.0;

  //if(mode==MODE_AUTO){
    writeWeldingDataToMemory();
  //}
  
  state = STATE_WELDING;
  digitalWrite(runPin, HIGH);
  digitalWrite(startPin, HIGH);
  if(mode==MODE_MANUAL){
    isOhmMeasured=true;
  } else {
    isOhmMeasured=false;
  }
  //mainV=analogRead(mainVoltagePin);
  //double startPWM= 210.0 - mainV*0.2022;
  
  int anVal=analogRead(mainVoltagePin);
  PWMMax=calculatePWMMax(anVal);
  
  myPID.outputSum=0.0;
  Input = 0.0;
  //Setpoint = st+0.07*(st-20);
  //Setpoint = st+(mainV-400)*0.05;
  
 //Setpoint = st;
  Setpoint = volts;
  
  Serial.print("Setpoint=");
  Serial.println(Setpoint);
  //Setpoint = volts+0.07*(volts-20);
  currentWeldingSeconds = 0;
  startTime = millis();
}

void showWeldingInfo(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_WELDING));
  lcd.setCursor(0, 1);
  lcd.print(lcdString(STRING_SECONDS));

  lcd.setCursor(0, 2);
  lcd.print(currentWeldingSeconds);
}

void stopWelding() {
  state = STATE_WELDING_FINISHED;
  analogWrite(PIN_OUTPUT, invertPWM(0));
  digitalWrite(runPin, LOW);
  digitalWrite(startPin, LOW);
  startTime = millis();
  writeFinalDataToMemory();
}

void showStopWeldingData(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_WELDING_F));
  if(weldingInterruptStopped){
    lcd.setCursor(0, 1);
    lcd.print(lcdString(STRING_EMER_STOP));
    lcd.setCursor(0, 3);
    lcd.print(lcdString(STRING_OK_RESTART));
    digitalWrite(errorPin, HIGH);
  } else if(mode == MODE_AUTO && autoCase == CASE_B){
    lcd.setCursor(0, 1);
    lcd.print(lcdString(  STRING_COOLING_T));
    lcd.print(coolingTime);
    lcd.print(lcdString(STRING_MINS));
    digitalWrite(errorPin, LOW);
    state = STATE_COOLING;
    coolingSecondsLeft=coolingTime*60;
    coolingStartMS=millis();
  } else {
    lcd.setCursor(0, 3);
    digitalWrite(errorPin, HIGH);
    lcd.print(lcdString(STRING_OK_RESTART));
  }
}
//-------------------
void startManual() {
  mode = MODE_MANUAL;
  state = STATE_MANUAL_SELECTION;
  manualSelectionState = MANUAL_SELECTION_SECONDS;
  resetBarcode();
  lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print(lcdString(STRING_MANUAL));
  lcd.setCursor(0, 0);
  seconds = manualSeconds;
  lcd.print(lcdString(STRING_SEL_SEC));
  showSeconds();

                                                                                  showSecodsCursor();
  
  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_OK_CONTINUE));

}

void resetBarcode(){
  for(int j=0; j<24; j++){
    barCodeChars[j]='0';
  }
}

void startManualVoltsSelection() {
  manualSelectionState = MANUAL_SELECTION_VOLTS;
  lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print(lcdString(STRING_MANUAL));
  lcd.setCursor(0, 0);
  volts = manualVolts;
  lcd.print(lcdString(STRING_SEL_VOLT));
  showVolts();
  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_OK_CONTINUE));
}

                                                                          void increaseSeconds(){
                                                                          if (secondsCursorIndex==3){
                                                                          if (seconds <=9998) {
                                                                            seconds++;}}
                                                                          else if(secondsCursorIndex==2){
                                                                            if (seconds <=9989) {
                                                                            seconds=seconds+10;}}
                                                                          else if(secondsCursorIndex==1){
                                                                          if (seconds <=9899) {
                                                                          seconds=seconds+100;}}
                                                                          else if(secondsCursorIndex==0){
                                                                          if (seconds <=8999) {
                                                                          seconds=seconds+1000;}}
                                                                          showSeconds();
                                                                          }


                                                                          void decreaseSeconds() {
                                                                          if (secondsCursorIndex==3){
                                                                          if (seconds > 0) {
                                                                            seconds--;}}
                                                                          else if(secondsCursorIndex==2){
                                                                            if (seconds > 9) {
                                                                            seconds=seconds-10;}}
                                                                          else if(secondsCursorIndex==1){
                                                                          if (seconds > 99) {
                                                                          seconds=seconds-100;}}
                                                                          else if(secondsCursorIndex==0){
                                                                          if (seconds > 999) {
                                                                          seconds=seconds-1000;}}
                                                                          showSeconds();
                                                                          }

void showSeconds() {
  totalWeldingSeconds=seconds;
  lcd.setCursor(0, 1);
  if (seconds < 10) {
    lcd.print("000");
  } else if (seconds < 100) {
    lcd.print("00");
  }
   else if (seconds < 1000) {
    lcd.print("0");
  }
  lcd.print(seconds);
}

void increaseVolts() {
  volts++;
  if(volts>48){
    volts=48;
  }
  showVolts();
}

void decreaseVolts() {
  if (volts > 8) {
    volts--;
  }
  showVolts();
}

void showVolts() {
  lcd.setCursor(0, 1);
  if (volts < 10) {
    lcd.print("0");
  }
  lcd.print(volts);
}

void showManualReadyForWelding() {
  manualSelectionState = MANUAL_SELECTION_READY;
  manualSeconds=seconds;
  manualVolts=volts;
  lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print(lcdString(STRING_MANUAL));
  lcd.setCursor(0, 1);
  lcd.print(lcdString(STRING_OK_START));
  lcd.setCursor(0, 3);
  lcd.print(lcdString( STRING_RESET_TO_R));
}

int invertPWM(int val){
  return 255-val;
}

float calculateMainVoltage(int anVal){
  float mv= anVal*0.295+16.0;
  return mv;
}

double calculatePWMMax(int anVal){
  float pwmm=560.0-0.6*anVal;
  return pwmm;
}

  
char getLabelChar(char ch1, char ch2){
  int n1 = ch1 - '0';
  int n2 = ch2 - '0';
  if(n1>5){
    n1-=6;
  } else if(n1>2){
    n1-=3;
  }
  
  if(n1==0 && n2==1){
    return 'A';
  }
  if(n1==0 && n2==2){
    return 'B';
  }
  if(n1==0 && n2==3){
    return 'C';
  }
  if(n1==0 && n2==4){
    return 'D';
  }
  if(n1==0 && n2==5){
    return 'E';
  }
  if(n1==0 && n2==6){
    return 'F';
  }
  if(n1==0 && n2==7){
    return 'G';
  }
  if(n1==0 && n2==8){
    return 'H';
  }
  if(n1==0 && n2==9){
    return 'I';
  }
  //---------------
  if(n1==1 && n2==0){
    return 'J';
  }
  if(n1==1 && n2==1){
    return 'K';
  }
  if(n1==1 && n2==2){
    return 'L';
  }
  if(n1==1 && n2==3){
    return 'M';
  }
  if(n1==1 && n2==4){
    return 'N';
  }
  if(n1==1 && n2==5){
    return 'O';
  }
  if(n1==1 && n2==6){
    return 'P';
  }
  if(n1==1 && n2==7){
    return 'Q';
  }
  if(n1==1 && n2==8){
    return 'R';
  }
  if(n1==1 && n2==9){
    return 'S';
  }
  //-------------
  if(n1==2 && n2==0){
    return 'T';
  }
  if(n1==2 && n2==1){
    return 'U';
  }
  if(n1==2 && n2==2){
    return 'V';
  }
  if(n1==2 && n2==3){
    return 'W';
  }
  if(n1==2 && n2==4){
    return 'X';
  }
  if(n1==2 && n2==5){
    return 'Y';
  }
  if(n1==2 && n2==6){
    return 'Z';
  }
  if(n1==2 && n2==7){
    return '+';
  }
  if(n1==2 && n2==8){
    return ' ';
  }
  if(n1==2 && n2==9){
    return ' ';
  }
  //-------------
}

void initShowMainMenu(){
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_MAIN_MENU));

  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_MAIN_MENU2));
}

void showMainMenu(){
  unsigned long currentTime = millis() - lastMainMenuTimeUpdateMS;
  coolingTime=0;
  if(currentTime>1000){
    lastMainMenuTimeUpdateMS=millis();
   
    temperature=0.0;
    for(int i=0;i<10;i++){
      temperature+= analogRead(tempPin);
    }
    temperature/=18.6;
    lcd.setCursor(0,0);
    lcd.print("T=");
    lcd.print(temperature);
    lcd.print(" C   V=");
    
    int anVal=analogRead(mainVoltagePin);
    mainVoltage= calculateMainVoltage(anVal);
    lcd.print((int)(round)(mainVoltage));
    lcd.print(" V");
    
    lcd.setCursor(0, 1);
    lcd.print(lcdString(STRING_MANU_BARCODE));
    
/*
    lcd.setCursor(0, 1);
    DateTime now = RTC.now(); 
    lcd.print(now.year(), DEC);
    lcd.print('/');
    if(now.month()<10){
      lcd.print('0');
    }
    lcd.print(now.month(), DEC);
    lcd.print('/');
    if(now.day()<10){
      lcd.print('0');
    }
    lcd.print(now.day(), DEC);
    lcd.print(' ');
    if(now.hour()<10){
      lcd.print('0');
    }
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if(now.minute()<10){
      lcd.print('0');
    }
    lcd.print(now.minute(), DEC);
    */
  }
}

void initShowSettings(){
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(lcdString(STRING_OK_TIME));

  lcd.setCursor(0,1);
  lcd.print(lcdString(STRING_PREV_LANG));

  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_NEXT_MEM));

  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_R_MAIN_MENU));
}

void showSettings(){
  unsigned long currentTime = millis() - lastMainMenuTimeUpdateMS;
  if(currentTime>1000){
    lastMainMenuTimeUpdateMS=millis();
    
    lcd.setCursor(0, 1);
    DateTime now = RTC.now(); 
    lcd.print(now.year(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.day(), DEC);
    lcd.print(' ');
    lcd.print(now.hour(), DEC);
    lcd.print(':');
    if(now.minute()<10){
      lcd.print('0');
    }
    lcd.print(now.minute(), DEC);
    
  }
}

void initShowTimeSettings(){
  lcd.clear();

  DateTime now = RTC.now();
  settingsYear=now.year();
  settingsMonth=now.month();
  settingsDay=now.day();
  settingsHour=now.hour();
  settingsMinute=now.minute();

  showTimeString();
  
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_OK_SAVE));

  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_R_MAIN_MENU));
}

void showTimeString(){
  lcd.setCursor(0, 0);
  lcd.print(settingsYear, DEC);
  lcd.print('/');
  if(settingsMonth<10){
    lcd.print('0');
  }
  lcd.print(settingsMonth, DEC);
  lcd.print('/');
  if(settingsDay<10){
    lcd.print('0');
  }
  lcd.print(settingsDay, DEC);
  lcd.print(' ');
  if(settingsHour<10){
    lcd.print('0');
  }
  lcd.print(settingsHour, DEC);
  lcd.print(':');
  if(settingsMinute<10){
    lcd.print('0');
  }
  lcd.print(settingsMinute, DEC);
}

void showTimeSettings(){
  lcd.setCursor(0, 1);
  if(settingsTimePosition==0){
    lcd.print("****                ");
  } else if(settingsTimePosition==1){
    lcd.print("     **             ");
  } else if(settingsTimePosition==2){
    lcd.print("        **          ");
  } else if(settingsTimePosition==3){
    lcd.print("           **       ");
  } else if(settingsTimePosition==4){
    lcd.print("              **    ");
  }
  showTimeString();
}

                                                void showBarcodeSettings(){
                                                lcd.setCursor(0, 1);
                                                lcd.print("            ");
                                                lcd.setCursor(0, 3);
                                                lcd.print("            ");
                                                if(settingsIndexPosition<=11){
                                                lcd.setCursor(settingsIndexPosition, 1);
                                                lcd.print("*");}
                                                else {
                                                lcd.setCursor((settingsIndexPosition-12), 3);
                                                lcd.print("*");}
                                                
                                                  }

                                                void showSecodsCursor(){
                                                lcd.setCursor(0,2);
                                                lcd.print("    ");
                                                lcd.setCursor(secondsCursorIndex,2);
                                                lcd.print("*");
                                                  }

void saveTimeSettings(){
  DateTime adjustedTime = DateTime(settingsYear,settingsMonth,settingsDay,settingsHour,settingsMinute,0);
  RTC.adjust(adjustedTime);
  state = STATE_MAIN_MENU;
  initShowMainMenu();
}

void initShowMemorySettings(){
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_MEM));

  byte mc=i2c_eeprom_read_byte(MEMORY_COUNT_ADRESS);
  lcd.print(mc);
  lcd.print(" of ");
  lcd.print(MEMORY_FULL_COUNT);
  
  lcd.setCursor(0, 1);
  lcd.print(lcdString(STRING_OK_WRITE));
  
  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_NEXT_DEL));

  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_R_MAIN_MENU));
}
//----------------------
void initShowLanguageSettings(){
  
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_LANG  ));

  lcd.setCursor(0, 3);
  lcd.print(lcdString(STRING_R_MAIN_MENU));
  
}

void showLanguageSettings(){
  if(languageSettings==LANGUAGE_ENGLISH){
    lcd.setCursor(10, 0);
    lcd.print("English  ");
    
    lcd.setCursor(0, 2);
    lcd.print("NEXT - Turkce  ");
  } else if(languageSettings==LANGUAGE_TURKISH){
    lcd.setCursor(10, 0);
    lcd.print("Turkce   ");
    
    lcd.setCursor(0, 2);
    lcd.print("NEXT - Espanol  ");
  }else if(languageSettings==LANGUAGE_SPANISH){
    lcd.setCursor(10, 0);
    lcd.print("Espanol  ");
    
    lcd.setCursor(0, 2);
    lcd.print("NEXT - Frances ");
  }else {
    lcd.setCursor(10, 0);
    lcd.print("Frances ");
    
    lcd.setCursor(0, 2);
    lcd.print("NEXT - English ");
  }
}
//----------------------
//eeprom
void i2c_eeprom_write_byte(unsigned int eeaddress, byte data ) {
    int rdata = data;
    Wire.beginTransmission(EEPROM_DEVICE_ADRESS);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.write(rdata);
    Wire.endTransmission();
}

byte i2c_eeprom_read_byte(unsigned int eeaddress ) {
    byte rdata = 0xFF;
    Wire.beginTransmission(EEPROM_DEVICE_ADRESS);
    Wire.write((int)(eeaddress >> 8)); // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_DEVICE_ADRESS,1);
    if (Wire.available()) rdata = Wire.read();
    return rdata;
}


void showMemoryFull(){
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(lcdString(STRING_MEM_FULL));

  lcd.setCursor(0, 2);
  lcd.print(lcdString(STRING_MEM_SET));
}

void clearMemory(){
  //memory_count++;
  memory_count=0;
  
  i2c_eeprom_write_byte(MEMORY_COUNT_ADRESS, 0);
  delay(50);
  state = STATE_MAIN_MENU;
  initShowMainMenu();
}

void writeWeldingDataToMemory(){
  
  memory_count++;
  Serial.println("write to eeprom");
  Serial.println(memory_count);
  i2c_eeprom_write_byte(MEMORY_COUNT_ADRESS, memory_count);
  delay(10);
  for(int i=0; i<24; i++){
    unsigned int byteAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+i;
    i2c_eeprom_write_byte(byteAdr, barCodeChars[i]);
    delay(10);
  }

  DateTime now = RTC.now();
  unsigned int yearAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+25;
  byte y=now.year()-2000;
  i2c_eeprom_write_byte(yearAdr, y);
  delay(10);
  i2c_eeprom_write_byte(yearAdr+1, now.month());
  delay(10);
  i2c_eeprom_write_byte(yearAdr+2, now.day());
  delay(10);
  i2c_eeprom_write_byte(yearAdr+3, now.hour());
  delay(10);
  i2c_eeprom_write_byte(yearAdr+4, now.minute());
  delay(10);
  i2c_eeprom_write_byte(yearAdr+5, (byte)temperature);
  delay(10);
  i2c_eeprom_write_byte(yearAdr+6, (byte)(round)(mainVoltage));
  delay(10);

  unsigned int secondsFloatAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+44;
  i2c_eeprom_write_byte(secondsFloatAdr, highByte(seconds));
  delay(10);
  i2c_eeprom_write_byte(secondsFloatAdr+1, lowByte(seconds));
  delay(10);

  unsigned int voltsFloatAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+46;
  Serial.println("volts:");
  byte voltsByte=(byte)volts;
  Serial.println(voltsByte);
  i2c_eeprom_write_byte(voltsFloatAdr, voltsByte);
  delay(10);
  
}

void writeFinalDataToMemory(){
  
  FLOATUNION_t measuredOhmsFloat;
  measuredOhmsFloat.number = measuredOhms;
  unsigned int ohmAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+33;
  for(int i=0; i<4; i++){
    i2c_eeprom_write_byte(ohmAdr+i, measuredOhmsFloat.bytes[i]);
    delay(10);
  }
  FLOATUNION_t totalJoulesFloat;
  totalJoulesFloat.number = totalJoules;
  unsigned int totalJoulesFloatAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+37;
  for(int i=0; i<4; i++){
    i2c_eeprom_write_byte(totalJoulesFloatAdr+i, totalJoulesFloat.bytes[i]);
    delay(10);
  }

  unsigned int totalSecondsFloatAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+41;
  i2c_eeprom_write_byte(totalSecondsFloatAdr, highByte(currentWeldingSeconds-1));
  delay(10);
  i2c_eeprom_write_byte(totalSecondsFloatAdr+1, lowByte(currentWeldingSeconds-1));
  delay(10);

  unsigned int weldingStatusAdr=MEMORY_ADRESS_OFFSET+(memory_count-1)*MEMORY_UNIT_SIZE+43;
  i2c_eeprom_write_byte(weldingStatusAdr, weldingStatus);
  
}

//-------------------------------
void writeToUsb(){
      
  if(usb2.checkConnection(0x01)==false){
    usb2.resetALL();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdString(STRING_USB_ERROR));
    lcd.setCursor(0, 2);
    lcd.print(lcdString(STRING_CLK_OK_R));
    state = STATE_ERROR;
    usbErrorReset();
    return;
  }

  String data[1];
  usb2.createEmptyFile("LOG.TXT", "", data);
  Serial.println(data[0]);
  if(data[0]=="1"){
    Serial.println("write ok");
  } else {
    usb2.resetALL();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lcdString(STRING_USB_NOT_CON));
    lcd.setCursor(0, 2);
    lcd.print(lcdString(STRING_CLK_OK_R));
    state = STATE_ERROR;
    usbErrorReset();
    return;
  }

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(lcdString(STRING_WRITE_USB));

  usb2.resetALL();
  
  //delay(40);
  String fileContent="";
  usb2.writeFile("LOG.TXT", fileContent);
  
  memory_count=i2c_eeprom_read_byte(MEMORY_COUNT_ADRESS);
  delay(eepromDelay);
  lcd.setCursor(3, 1);
  lcd.print(" of ");
  lcd.print(memory_count);
  for(int i=0;i<memory_count;i++){
    lcd.setCursor(0, 1);
    lcd.print(i+1);
    //-------------
    String numberStr="";
    numberStr+=i+1;
    //-------------
    String barCodeStr="";
    char barCodeBytes[24];
    for(int j=0; j<24; j++){
      unsigned int byteAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+j;
      byte barcodeCh=i2c_eeprom_read_byte(byteAdr);
      delay(eepromDelay);
      barCodeBytes[j]=barcodeCh;
      barCodeStr+=barcodeCh - '0';
    }
    //--------------
    String dateStr="";
    int yearInt=2000+i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+25);
    delay(eepromDelay);
    dateStr+=yearInt;
    dateStr+="/";
    int monthInt=i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+26);
    delay(eepromDelay);
    if(monthInt<10){
      dateStr+="0";
    }
    dateStr+=monthInt;
    dateStr+="/";
    int dayInt=i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+27);
    delay(eepromDelay);
    if(dayInt<10){
      dateStr+="0";
    }
    dateStr+=dayInt;
    dateStr+="  ";
    
    int hourInt=i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+28);
    delay(eepromDelay);
    if(hourInt<10){
      dateStr+="0";
    }
    dateStr+=hourInt;
    dateStr+=":";
    
    int minutesInt=i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+29);
    delay(eepromDelay);
    if(minutesInt<10){
      dateStr+="0";
    }
    dateStr+=minutesInt;
    //--------------------
    String tempStr="";
    tempStr+=i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+30);
    delay(eepromDelay);
    tempStr+="C";
    //--------------------
    String mainVoltsStr="";
    mainVoltsStr+=i2c_eeprom_read_byte(MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+31);
    delay(eepromDelay);
    mainVoltsStr+="V";
    //--------------------
    String measuredOhms="";
    FLOATUNION_t measuredOhmsFloat;
    unsigned int ohmAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+33;
    for(int i=0; i<4; i++){
      measuredOhmsFloat.bytes[i]=i2c_eeprom_read_byte(ohmAdr+i);
      delay(eepromDelay);
    }
    measuredOhms+=measuredOhmsFloat.number;
    measuredOhms+="Ohm";
    //---------------------
    String joulesStr="";
    FLOATUNION_t joulesFloat;
    unsigned int joulesAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+37;
    for(int i=0; i<4; i++){
      joulesFloat.bytes[i]=i2c_eeprom_read_byte(joulesAdr+i);
      delay(eepromDelay);
    }
    joulesStr+=joulesFloat.number;
    joulesStr+="J";
    //---------------------
    String secondsStr="";
    unsigned int secondsFloatAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+44;
    byte seconds_hb=i2c_eeprom_read_byte(secondsFloatAdr);
    delay(eepromDelay);
    byte seconds_lb=i2c_eeprom_read_byte(secondsFloatAdr+1);
    delay(eepromDelay);
    int secondsMemory=word(seconds_hb,seconds_lb);
    secondsStr+=secondsMemory;
    secondsStr+="s";

    String totalSecondsStr="";
    unsigned int totalSecondsFloatAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+41;
    byte hb=i2c_eeprom_read_byte(totalSecondsFloatAdr);
    delay(eepromDelay);
    byte lb=i2c_eeprom_read_byte(totalSecondsFloatAdr+1);
    delay(eepromDelay);
    int totalSecondsMemory=word(hb,lb);
    totalSecondsStr+=totalSecondsMemory;
    totalSecondsStr+="s";
    //--------------------
    String weldingStr="";
    unsigned int weldingStatusAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+43;
    byte ws=i2c_eeprom_read_byte(weldingStatusAdr);
    delay(eepromDelay);
    if(ws==WELDING_OK){
      weldingStr="WELDING OK";
    } else if(ws==WELDING_NOK){
      weldingStr="ERROR";
    } else if(ws==WELDING_OHM_ERROR){
      weldingStr="OHM ERROR";
    } else if(ws==WELDING_STOPPED){
      weldingStr="STOPPED";
    }
    //---------------------
    //diam
    int diam1 = barCodeBytes[8] - '0';
    int diam2 = barCodeBytes[9] - '0';
    int diam3 = barCodeBytes[10] - '0';
    int diameter = diam1*100 + diam2 * 10 + diam3;
    String diameterStr="";
    diameterStr+=diameter;
    if(diameter<100){
      diameterStr="0"+diameterStr;
    }
    diameterStr+="mm";
    //---------------------
    //ohm
    int ohm1 = barCodeBytes[14] - '0';
    int ohm2 = barCodeBytes[15] - '0';
    int ohm3 = barCodeBytes[16] - '0';
    int ohm_comma_place=barCodeBytes[11] - '0';
    double memoryOhms;
    if(ohm_comma_place==1){
      memoryOhms = ohm1*100 + ohm2 * 10 + ohm3;
    } else if(ohm_comma_place==2 || ohm_comma_place==7){
      memoryOhms = ohm1*10 + ohm2 + ohm3 * 0.1;
    } else if(ohm_comma_place==3 || ohm_comma_place==8) {
      memoryOhms = ohm1 + ohm2 * 0.1 + ohm3 * 0.01;
    }
    String ohmsStr="";
    ohmsStr+=memoryOhms;
    ohmsStr+="Ohm";
    //---------------------
    //volts
    /*
    int volt1 = barCodeBytes[12] - '0';
    int volt2 = barCodeBytes[13] - '0';
    float voltsF = volt1 * 10 + volt2;
    */
    String voltsString="";
    unsigned int voltsStatusAdr=MEMORY_ADRESS_OFFSET+i*MEMORY_UNIT_SIZE+46;
    byte vs=i2c_eeprom_read_byte(voltsStatusAdr);
    Serial.println("eeprom volts:");
    Serial.println(vs);
    delay(eepromDelay);
    voltsString+=vs;
    voltsString+="V";
    //---------------------
    //Firm
    int firstChar=barCodeBytes[0] - '0';
    String typeStr="";
    String firmStr="";
    if(firstChar==9){
        firmStr+=getLabelChar(barCodeBytes[2],barCodeBytes[3]);
        firmStr+=getLabelChar(barCodeBytes[4],barCodeBytes[5]);
        firmStr+="  ";
        int secChar=barCodeBytes[1] - '0';
        if(secChar==0){
          typeStr="*";
        } else if(secChar==1){
          typeStr="RAYT";
        } else if(secChar==2){
          typeStr="JTDW";
        } else if(secChar==3){
          typeStr="Yred";
        } else if(secChar==4){
          typeStr="T.TE";
        } else if(secChar==5){
          typeStr="ICUP";
        } else if(secChar==6){
          typeStr="SOCK";
        } else if(secChar==7){
          typeStr="T.TE";
        } else if(secChar==8){
          typeStr="ELBW";
        } else if(secChar==9){
          typeStr="EROR";
        } 
        
      } else {
        firmStr+=getLabelChar(barCodeBytes[0],barCodeBytes[1]);
        firmStr+=getLabelChar(barCodeBytes[2],barCodeBytes[3]);
        firmStr+=getLabelChar(barCodeBytes[4],barCodeBytes[5]);
        firmStr+=getLabelChar(barCodeBytes[6],barCodeBytes[7]);
        if(firstChar>5){
          typeStr="SOCK";
        } else if(firstChar>2){
          typeStr="ICUP";
        } else {
          typeStr="T.TE";
        }
      }
      
    //--------------------
    //usb2.resetALL();
    fileContent="\n_________________________________________";
    //usb2.appendFile("LOG.TXT", fileContent);
    
    fileContent+="\nDateTime             ";
    fileContent+=dateStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    
    fileContent+="\nNO                   ";
    fileContent+=numberStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    //---------------
    
    fileContent+="\nAmbient Temperature  ";
    fileContent+=tempStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    //---------------
    
    fileContent+="\nMain Voltage         ";
    fileContent+=mainVoltsStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    //---------------
    
    fileContent+="\nService Data         ";
    fileContent+=firmStr;
    fileContent+="   ";
    fileContent+=typeStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    
    
    fileContent+="\nDiameter             ";
    fileContent+=diameterStr;
    usb2.appendFile("LOG.TXT", fileContent);
    
    fileContent="\nBarcode              ";
    fileContent+=barCodeStr;
    //usb2.appendFile("LOG.TXT", fileContent);

   //----------------
   
    fileContent+="\nAct./Nom. Seconds    ";
    fileContent+=totalSecondsStr;
    fileContent+="/";
    fileContent+=secondsStr;
    //fileContent+=sec1 * 100 + sec2 * 10 + sec3;
   
    
    //usb2.appendFile("LOG.TXT", fileContent);
    
    fileContent+="\nAct./Nom. Volts      ";
    fileContent+=voltsString;
    fileContent+="/";
    fileContent+=voltsString;
    //usb2.appendFile("LOG.TXT", fileContent);
    
    fileContent+="\nAct./Nom. Ohms       ";
    fileContent+=measuredOhms;
    fileContent+="/";
    fileContent+=ohmsStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    
    fileContent+="\nJoules               ";
    fileContent+=joulesStr;
    usb2.appendFile("LOG.TXT", fileContent);
    //----------------
    
    fileContent="\nAssessment           ";
    fileContent+=weldingStr;
    //usb2.appendFile("LOG.TXT", fileContent);
    //--------------
    
    fileContent+="\n                                         ";
    //usb2.appendFile("LOG.TXT", fileContent);
    fileContent+="\n                                         ";
    //usb2.appendFile("LOG.TXT", fileContent);
    fileContent+="\n                                         ";
    usb2.appendFile("LOG.TXT", fileContent);
    
  }
  //usb2.resetALL();
  initShowMemorySettings();
  usb2.resetALL();
  
}

void usbErrorReset(){
  digitalWrite(usbResetPin, HIGH);
  delay( 500 );
  digitalWrite(usbResetPin, LOW);
}


String lcdString(int stringCode){
  if(languageSettings==LANGUAGE_ENGLISH){
    if(stringCode==STRING_MAIN_MENU){
      return "OK - Continue";
    } else if(stringCode==STRING_MAIN_MENU2){
      return "RES - Settings";
    } else if(stringCode==STRING_WELDING){
      return "Welding";
    } else if(stringCode==STRING_MANUAL){
      return "OK - Manual";
    }else if(stringCode==STRING_SECONDS){
      return "Seconds ";
    }else if(stringCode==STRING_JOULES){
      return "Joules ";
    }else if(stringCode==SRTING_ERROR_TRY_AG){
      return "Error. Try again.";
    }else if(stringCode==STRING_SEC){
      return "sec. ";
    }else if(stringCode==STRING_AUTO){
      return "AUTO Waiting barcode";
    }else if(stringCode==STRING_CLK_OK_R){
      return "Click OK to restart";
    }else if(stringCode==STRING_S_CIRCUIT){
      return "Short circuit";
    }else if(stringCode==STRING_O_CIRCUIT){
      return "open circuit";
    }else if(stringCode==STRING_MAIN_V_ERROR){
      return "Main Voltage Error";
    }else if(stringCode==STRING_OVERHEAT_ERROR){
      return "Overheat Error";
    }else if(stringCode==STRING_WELDING_F){
      return "Welding finished";
    }else if(stringCode==STRING_EMER_STOP){
      return "Emergency stop";
    }else if(stringCode==STRING_OK_RESTART){
      return "OK - Restart";
    }else if(stringCode==STRING_COOLING_T){
      return "Cooling Time ";
    }else if(stringCode==STRING_MINS){
      return " min";
    }else if(stringCode==STRING_SEL_SEC){
      return "Select seconds";
    }else if(stringCode==STRING_OK_CONTINUE){
      return "OK - Continue";
    }else if(stringCode==STRING_SEL_VOLT){
      return "Select volts";
    }else if(stringCode==STRING_OK_START){
      return "OK - Start";
    }else if(stringCode==STRING_RESET_TO_R){
      return "RES - Restart";
    }else if(stringCode==STRING_OK_TIME){
      return "OK-Time";
    }else if(stringCode==STRING_PREV_LANG){
      return "Prev - Language";
    }else if(stringCode==STRING_NEXT_MEM){
      return "Next - Memory";
    }else if(stringCode==STRING_R_MAIN_MENU){
      return "RES - Main Menu";
    }else if(stringCode==STRING_OK_SAVE){
      return "OK-Save";
    }else if(stringCode==STRING_MEM){
      return "Memory ";
    }else if(stringCode==STRING_OK_WRITE){
      return "OK - Write to USB";
    }else if(stringCode==STRING_NEXT_DEL){
      return "NEXT - Delete";
    }else if(stringCode==STRING_LANG){
      return "Language:";
    }else if(stringCode==STRING_MEM_FULL){
      return "Memory Full";
    }else if(stringCode==STRING_MEM_SET){
      return "OK - Memory settings";
    }else if(stringCode==STRING_USB_ERROR){
      return "USB Format error";
    }else if(stringCode==STRING_USB_NOT_CON){
      return "USB not connected";
    }else if(stringCode==STRING_WRITE_USB){
      return "Writing USB";
    }else if(stringCode==STRING_ERROR){
      return "Error.";
    }else if(stringCode==STRING_ERROR_OHM){
      return "Error Ohm";
    }else if(stringCode==STRING_VOUT_ERROR){
      return "V OUT ERROR";
    }else if(stringCode==STRING_OK_START_RESET){
      return "OK-Start RES-Reset";
    }else if(stringCode==STRING_OK_AUTO){
      return "OK-Start";
    }else if(stringCode==STRING_RES){
      return "RES-Menu";
    }else if(stringCode==STRING_MANU_BARCODE){
      return "NEXT-Manual Barcode";
    }
    
  } else if(languageSettings==LANGUAGE_TURKISH){
    if(stringCode==STRING_MAIN_MENU){
      return "OK - Devam";
    } else if(stringCode==STRING_MAIN_MENU2){
      return "RES - Ayarlar";
    } else if(stringCode==STRING_WELDING){
      return "Kaynak";
    } else if(stringCode==STRING_MANUAL){
      return "OK - Manuel";
    }else if(stringCode==STRING_SECONDS){
      return "Saniye ";
    }else if(stringCode==STRING_JOULES){
      return "Joule ";
    }else if(stringCode==SRTING_ERROR_TRY_AG){
      return "Hata. Yeniden dene.";
    }else if(stringCode==STRING_SEC){
      return "San. ";
    }else if(stringCode==STRING_AUTO){
      return "Oto. barkod bekleme";
    }else if(stringCode==STRING_CLK_OK_R){
      return "OK-Yeniden Basla";
    }else if(stringCode==STRING_S_CIRCUIT){
      return "Kisa devre";
    }else if(stringCode==STRING_O_CIRCUIT){
      return "Acik devre";
    }else if(stringCode==STRING_MAIN_V_ERROR){
      return "Giris voltaj hatasi";
    }else if(stringCode==STRING_OVERHEAT_ERROR){
      return "Asiri isinma hatasi";
    }else if(stringCode==STRING_WELDING_F){
      return "Kaynak bitti";
    }else if(stringCode==STRING_EMER_STOP){
      return "Acil durdurma";
    }else if(stringCode==STRING_OK_RESTART){
      return "OK-Yeniden basla";
    }else if(stringCode==STRING_COOLING_T){
      return "Soguma zamani ";
    }else if(stringCode==STRING_MINS){
      return " Dak";
    }else if(stringCode==STRING_SEL_SEC){
      return "Saniye sec";
    }else if(stringCode==STRING_OK_CONTINUE){
      return "OK - Devam et";
    }else if(stringCode==STRING_SEL_VOLT){
      return "Voltaj sec";
    }else if(stringCode==STRING_OK_START){
      return "OK - Basla";
    }else if(stringCode==STRING_RESET_TO_R){
      return "RES - Yeniden basla";
    }else if(stringCode==STRING_OK_TIME){
      return "OK - Zaman";
    }else if(stringCode==STRING_PREV_LANG){
      return "PREV - Dil";
    }else if(stringCode==STRING_NEXT_MEM){
      return "NEXT - Hafiza";
    }else if(stringCode==STRING_R_MAIN_MENU){
      return "RES - Ana menu";
    }else if(stringCode==STRING_OK_SAVE){
      return "OK - Kaydet";
    }else if(stringCode==STRING_MEM){
      return "Hafiza ";
    }else if(stringCode==STRING_OK_WRITE){
      return "OK - USB'ye Yaz";
    }else if(stringCode==STRING_NEXT_DEL){
      return "NEXT - Sil";
    }else if(stringCode==STRING_LANG){
      return "Dil:";
    }else if(stringCode==STRING_MEM_FULL){
      return "Hafiza dolu";
    }else if(stringCode==STRING_MEM_SET){
      return "OK - Hafiza ayarlari";
    }else if(stringCode==STRING_USB_ERROR){
      return "USB Format hatasi";
    }else if(stringCode==STRING_USB_NOT_CON){
      return "USB bagli degil";
    }else if(stringCode==STRING_WRITE_USB){
      return "USB'ye yazma";
    }else if(stringCode==STRING_ERROR){
      return "Hata.";
    }else if(stringCode==STRING_ERROR_OHM){
      return "Hata Ohm";
    }else if(stringCode==STRING_VOUT_ERROR){
      return "V Cikis Hatasi";
    }else if(stringCode==STRING_OK_START_RESET){
      return "OK-Basla RES-Sifirla";
    }else if(stringCode==STRING_OK_AUTO){
      return "OK-Basla";
    }else if(stringCode==STRING_RES){
      return "RES-Menu";
    }else if(stringCode==STRING_MANU_BARCODE){
      return "NEXT-Manuel Barkod";
    }
  }else if(languageSettings==LANGUAGE_SPANISH){
    if(stringCode==STRING_MAIN_MENU){
      return "OK - Seguir";
    } else if(stringCode==STRING_MAIN_MENU2){
      return "RES - Configur";
    } else if(stringCode==STRING_WELDING){
      return "Fuente";
    } else if(stringCode==STRING_MANUAL){
      return "OK - Manual";
    }else if(stringCode==STRING_SECONDS){
      return "Segun.";
    }else if(stringCode==STRING_JOULES){
      return "Joule ";
    }else if(stringCode==SRTING_ERROR_TRY_AG){
      return "Error Inten de nuev";
    }else if(stringCode==STRING_SEC){
      return "Segundos ";
    }else if(stringCode==STRING_AUTO){
      return "Codigo barras espera";
    }else if(stringCode==STRING_CLK_OK_R){
      return "OK-Redemarrer";
    }else if(stringCode==STRING_S_CIRCUIT){
      return "Cortocircuito";
    }else if(stringCode==STRING_O_CIRCUIT){
      return "Circuito abierto";
    }else if(stringCode==STRING_MAIN_V_ERROR){
      return "Error voltaj entrada";
    }else if(stringCode==STRING_OVERHEAT_ERROR){
      return "Error Sobrecalentar";
    }else if(stringCode==STRING_WELDING_F){
      return "Soldadura terminada";
    }else if(stringCode==STRING_EMER_STOP){
      return "Parada de emergencia";
    }else if(stringCode==STRING_OK_RESTART){
      return "OK-Reiniciar";
    }else if(stringCode==STRING_COOLING_T){
      return "Tiempo enfriamiento";
    }else if(stringCode==STRING_MINS){
      return " Minutos";
    }else if(stringCode==STRING_SEL_SEC){
      return "Seleccionar segundos";
    }else if(stringCode==STRING_OK_CONTINUE){
      return "OK - Seguir";
    }else if(stringCode==STRING_SEL_VOLT){
      return "Seleccione voltios";
    }else if(stringCode==STRING_OK_START){
      return "OK - Comienzo";
    }else if(stringCode==STRING_RESET_TO_R){
      return "RES - Reiniciar";
    }else if(stringCode==STRING_OK_TIME){
      return "OK - Hora";
    }else if(stringCode==STRING_PREV_LANG){
      return "PREV - Idioma";
    }else if(stringCode==STRING_NEXT_MEM){
      return "NEXT - Memoria";
    }else if(stringCode==STRING_R_MAIN_MENU){
      return "RES - Menu principal";
    }else if(stringCode==STRING_OK_SAVE){
      return "OK - Salvar";
    }else if(stringCode==STRING_MEM){
      return "Memoria ";
    }else if(stringCode==STRING_OK_WRITE){
      return "OK - Escribir en USB";
    }else if(stringCode==STRING_NEXT_DEL){
      return "NEXT - Eliminar";
    }else if(stringCode==STRING_LANG){
      return "Idioma:";
    }else if(stringCode==STRING_MEM_FULL){
      return "Memoria llena";
    }else if(stringCode==STRING_MEM_SET){
      return "OK-Configur. memoria";
    }else if(stringCode==STRING_USB_ERROR){
      return "Error de formato USB";
    }else if(stringCode==STRING_USB_NOT_CON){
      return "USB no conectado";
    }else if(stringCode==STRING_WRITE_USB){
      return "USB de escritura";
    }else if(stringCode==STRING_ERROR){
      return "Error.";
    }else if(stringCode==STRING_ERROR_OHM){
      return "Error Ohm";
    }else if(stringCode==STRING_VOUT_ERROR){
      return "V ERROR DE SALIDA";
    }else if(stringCode==STRING_OK_START_RESET){
      return "OK-Iniciar RES-Reset";
    }else if(stringCode==STRING_OK_AUTO){
      return "OK-Start";
    }else if(stringCode==STRING_RES){
      return "RES-Menu";
    }else if(stringCode==STRING_MANU_BARCODE){
      return "NEXT-Codi Bar Manual";
    }
  }
  else if(languageSettings==LANGUAGE_FRENCH){
    if(stringCode==STRING_MAIN_MENU){
      return "OK - Continuer";
    } else if(stringCode==STRING_MAIN_MENU2){
      return "RES - Reglages";
    } else if(stringCode==STRING_WELDING){
      return "Soudage";
    } else if(stringCode==STRING_MANUAL){
      return "OK - Manuel";
    }else if(stringCode==STRING_SECONDS){
      return "Second ";
    }else if(stringCode==STRING_JOULES){
      return "Joule ";
    }else if(stringCode==SRTING_ERROR_TRY_AG){
      return "Erreur. Réessayer.";
    }else if(stringCode==STRING_SEC){
      return "Seconde.";
    }else if(stringCode==STRING_AUTO){
      return "Code barres attente";
    }else if(stringCode==STRING_CLK_OK_R){
      return "OK - Redémarrer";
    }else if(stringCode==STRING_S_CIRCUIT){
      return "Court-circuit";
    }else if(stringCode==STRING_O_CIRCUIT){
      return "Circuit ouverte";
    }else if(stringCode==STRING_MAIN_V_ERROR){
      return "Erreur tension dentr";
    }else if(stringCode==STRING_OVERHEAT_ERROR){
      return "Erreur de surchauffe";
    }else if(stringCode==STRING_WELDING_F){
      return "Soudage termine";
    }else if(stringCode==STRING_EMER_STOP){
      return "ArrEt d'urgence";
    }else if(stringCode==STRING_OK_RESTART){
      return "OK-Redemarrer";
    }else if(stringCode==STRING_COOLING_T){
      return "Temps refroidisement";
    }else if(stringCode==STRING_MINS){
      return " Minutes";
    }else if(stringCode==STRING_SEL_SEC){
      return "Selectionnez second";
    }else if(stringCode==STRING_OK_CONTINUE){
      return "OK - Continuer";
    }else if(stringCode==STRING_SEL_VOLT){
      return "Selectionnez volts";
    }else if(stringCode==STRING_OK_START){
      return "OK - Debut";
    }else if(stringCode==STRING_RESET_TO_R){
      return "RES - Redemarrer";
    }else if(stringCode==STRING_OK_TIME){
      return "OK - Temps";
    }else if(stringCode==STRING_PREV_LANG){
      return "PREV - Langue";
    }else if(stringCode==STRING_NEXT_MEM){
      return "NEXT - Memoire";
    }else if(stringCode==STRING_R_MAIN_MENU){
      return "RES - Menu principal";
    }else if(stringCode==STRING_OK_SAVE){
      return "OK - Sauvegarder";
    }else if(stringCode==STRING_MEM){
      return "Memoire";
    }else if(stringCode==STRING_OK_WRITE){
      return "OK - Ecrire sur USB";
    }else if(stringCode==STRING_NEXT_DEL){
      return "NEXT - Effacer";
    }else if(stringCode==STRING_LANG){
      return "Langue:";
    }else if(stringCode==STRING_MEM_FULL){
      return "Memoire pleine";
    }else if(stringCode==STRING_MEM_SET){
      return "OK-Reglages memoire";
    }else if(stringCode==STRING_USB_ERROR){
      return "Erreur de format USB";
    }else if(stringCode==STRING_USB_NOT_CON){
      return "USB non connecte";
    }else if(stringCode==STRING_WRITE_USB){
      return "Ecriture USB";
    }else if(stringCode==STRING_ERROR){
      return "Erreur.";
    }else if(stringCode==STRING_ERROR_OHM){
      return "Erreur Ohm";
    }else if(stringCode==STRING_VOUT_ERROR){
      return "ERREUR DE SORTIE V";
    }else if(stringCode==STRING_OK_START_RESET){
      return "OK-Debut RES-Menu";
    }else if(stringCode==STRING_OK_AUTO){
      return "OK-Debut";
    }else if(stringCode==STRING_RES){
      return "RES-Menu";
    }else if(stringCode==STRING_MANU_BARCODE){
      return "NEXT-Code Bar Manuel";
    }
  }
}
