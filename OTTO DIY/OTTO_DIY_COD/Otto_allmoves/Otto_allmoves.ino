//Code/Otto_allmoves.ino
//----------------------------------------------------------------
//-- Otto All moves test
//-- This code will make Otto make all functions, you can reorganize moves, gestures or uncoment sings in the principal loop
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
#include <Otto.h>
Otto Otto;  //This is Otto!
//----------------------------------------------------------------------
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
//-- Setup ------------------------------------------------------//
///////////////////////////////////////////////////////////////////
void setup(){
  Otto.init(PIN_LEFTLEG,PIN_RIGHTLEG,PIN_LEFTFOOT,PIN_RIGHTFOOT,true); //Set the servo pins
  Otto.sing(S_connection); //Otto wake up!
  Otto.home();
  delay(50);
}
///////////////////////////////////////////////////////////////////
//-- Principal Loop ---------------------------------------------//
///////////////////////////////////////////////////////////////////
void loop() {
          Otto.walk(2,1000,1); //2 steps, "TIME". IF HIGHER THE VALUE THEN SLOWER (from 600 to 1400), 1 FORWARD
          Otto.walk(2,1000,-1); //2 steps, T, -1 BACKWARD
          Otto.turn(2,1000,1);//3 steps turning LEFT
          Otto.home();
          delay(100);
          Otto.turn(2,1000,-1);//3 steps turning RIGHT
          Otto.bend (1,500,1); //usually steps =1, T=2000
          Otto.bend (1,2000,-1);
          Otto.shakeLeg (1,1500, 1);
          Otto.home();
          delay(100);
          Otto.shakeLeg (1,2000,-1);
          Otto.moonwalker(3, 1000, 25,1); //LEFT
          Otto.moonwalker(3, 1000, 25,-1); //RIGHT
          Otto.crusaito(2, 1000, 20,1);
          Otto.crusaito(2, 1000, 20,-1);
          delay(100);
          Otto.flapping(2, 1000, 20,1);
          Otto.flapping(2, 1000, 20,-1);
          delay(100);
          Otto.swing(2, 1000, 20);
          Otto.tiptoeSwing(2, 1000, 20);
          Otto.jitter(2, 1000, 20); //(small T)
          Otto.updown(2, 1500, 20);  // 20 = H "HEIGHT of movement"T
          Otto.ascendingTurn(2, 1000, 50);
          Otto.jump(1,2000);
          delay(50);
          //Otto.sing(S_cuddly);
          //Otto.sing(S_OhOoh);
          //Otto.sing(S_OhOoh2);
          //Otto.sing(S_surprise);
          //Otto.sing(S_buttonPushed);
          //Otto.sing(S_mode1);
          //Otto.sing(S_mode2); 
          //Otto.sing(S_mode3);
          //Otto.sing(S_sleeping);
          //Otto.sing(S_fart1);
          //Otto.sing(S_fart2);
          //Otto.sing(S_fart3);
          //Otto.sing(S_happy);
          //Otto.sing(S_happy_short);
          //Otto.sing(S_superHappy);
          //Otto.sing(S_sad);
          //Otto.sing(S_confused);
          //Otto.sing(S_disconnection);
          Otto.home();
          Otto.playGesture(OttoHappy);
          Otto.playGesture(OttoSuperHappy);
          Otto.playGesture(OttoSad);
          Otto.playGesture(OttoVictory); 
          Otto.playGesture(OttoAngry);
          Otto.playGesture(OttoSleeping);
          Otto.playGesture(OttoFretful);
          Otto.playGesture(OttoLove);
          Otto.playGesture(OttoConfused);
          Otto.playGesture(OttoFart);
          Otto.playGesture(OttoWave);
          Otto.playGesture(OttoMagic);
          Otto.playGesture(OttoFail);
          Otto.home();
}
