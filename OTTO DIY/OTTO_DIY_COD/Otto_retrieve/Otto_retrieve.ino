//Code/Otto_retrieve.ino
//----------------------------------------------------------------
//-- Otto Push
//-- With this Otto will retrieve everytime you put something in front of the Ultra sound but, Can you make Otto follow you?
//-- April 2019: Designed to work with the basic Otto but could be compatible with PLUS or Humanoid or other biped robots
/******************************************************************************************************
  Make sure to have installed all libraries: https://wikifactory.com/+OttoDIY/otto-diy
  Otto DIY invests time and resources providing open source code and hardware, 
  please support by purchasing kits from (https://www.ottodiy.com)
  BSD license, all text above must be included in any redistribution
 *******************************************************************/
//-----------------------------------------------------------------
#include <Servo.h> 
#include <Oscillator.h>
#include <US.h>
#include <Otto.h>
Otto Otto;  //This is Otto!
//---------------------------------------------------------
//-- Make sure the servos are in the right pin
/*             --------
              |  O  O  |
              |--------|
  RIGHT LEG 3 |        | LEFT LEG 2
               --------
               ||     ||
RIGHT FOOT 5 |---     ---| LEFT FOOT 4
*/
  #define PIN_LEFTLEG 2 //servo[2]
  #define PIN_RIGHTLEG 3 //servo[3]
  #define PIN_LEFTFOOT 4 //servo[4]
  #define PIN_RIGHTFOOT 5 //servo[5]
///////////////////////////////////////////////////////////////////
//-- Global Variables -------------------------------------------//
///////////////////////////////////////////////////////////////////
//-- Movement parameters
int T=1000;              //Initial duration of movement
int moveId=0;            //Number of movement
int moveSize=15;         //Asociated with the height of some movements
//---------------------------------------------------------
bool obstacleDetected = false;
///////////////////////////////////////////////////////////////////
//-- Setup ------------------------------------------------------//
///////////////////////////////////////////////////////////////////
void setup(){
  Otto.init(PIN_LEFTLEG,PIN_RIGHTLEG,PIN_LEFTFOOT,PIN_RIGHTFOOT,true); //Set the servo pins
  Otto.sing(S_connection); //Otto wake up!
  Otto.home();
  delay(50);
  Otto.sing(S_happy); // a happy Otto :)
}
///////////////////////////////////////////////////////////////////
//-- Principal Loop ---------------------------------------------//
///////////////////////////////////////////////////////////////////
void loop() {
  if(obstacleDetected){
               Otto.walk(1,1000,1);  //Otto.walk(1,1000,1) to make Otto follow you!
             obstacleDetector();
             }
         else{
            Otto.home();
            obstacleDetector();
        }
  }
///////////////////////////////////////////////////////////////////
//-- Function to read distance sensor & to actualize obstacleDetected variable
void obstacleDetector(){
   int distance = Otto.getDistance();
        if(distance<15){
          obstacleDetected = true;
        }else{
          obstacleDetected = false;
        }
}
