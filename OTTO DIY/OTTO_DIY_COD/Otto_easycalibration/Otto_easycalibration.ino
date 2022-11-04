//Code/Otto_easycalibration.ino
//----------------------------------------------------------------
//-- Otto Easy calibration
//-- With this code you can calibrate the servo motors butit will require to adjust the TRIM values
//-- April 2019: Designed to work with the basic Otto but could be compatible with PLUS or Humanoid or other biped robots
/******************************************************************************************************
  Make sure to have installed all libraries: https://wikifactory.com/+OttoDIY/otto-diy
  Otto DIY invests time and resources providing open source code and hardware,
  please support by purchasing kits from (https://www.ottodiy.com)
  BSD license, all text above must be included in any redistribution
 *******************************************************************/
//-- VERY IMPORTANT only calibrate ONE TIME per board!to avoid damage EEPROM memory
//-----------------------------------------------------------------
#include <Otto.h>
#include <Servo.h>
#include <Oscillator.h>
#include <EEPROM.h>
Otto Otto;
void setup()
{
  Otto.init(2, 3, 4, 5, false);
  Otto.setTrims(-7,-4,-4,7); //change Trim "offset values" gradually until Otto is completely straight (90º)
 // Otto.saveTrimsOnEEPROM(); //use only after completely straight(90º), delete this line after for further programming
}
int posiciones[] = {90, 90, 90, 90};
void loop()  //test comparing before & after function
{
  Otto.home();
 // delay(4000);
 // Otto.updown(6, 500, BIG);
 // delay(2000);
 //
 // Otto.walk(4,1800);
 //delay(2000);
}
