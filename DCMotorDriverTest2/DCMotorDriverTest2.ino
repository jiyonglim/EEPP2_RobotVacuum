/**
 * \par Copyright (C), 2012-2016, MakeBlock
 * @file    DCMotorDriverTest.ino
 * @author  MakeBlock
 * @version V1.0.0
 * @date    2015/09/09
 * @brief   Description: this file is sample code for Me DC motor device.
 *
 * Function List:
 *    1. void MeDCMotor::run(int16_t speed)
 *    2. void MeDCMotor::stop(void)
 *
 * \par History:
 * <pre>
 * <Author>     <Time>        <Version>      <Descr>
 * Mark Yan     2015/09/09    1.0.0          rebuild the old lib
 * </pre>
 */
#include "MeOrion.h"


MeDCMotor motorLeft(M1);
MeDCMotor motorRight(M2);
//Actual port number = port number on board + 3
MeLineFollower lineFinder(PORT_6); /* Line Finder module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield. */ 


//Constants
int LMotorSpeed = -85;
int RMotorSpeed = 70;
int LTurnTime = 800; //Time taken to turn 90deg
int RTurnTime = 830; //Time taken to turn 90deg

//Global variables
int facingDeg = 2; //Clockwise is +ve     0: North, 1: East, 2: South, 3: West ^><v
bool lineIRSenseState[2] = {0, 0};

void setup()
{
  Serial.begin(115200);
  zigzag();
  /*for (int m = 0; m < 8; m++) {
    moveTurn(4);
    delay(500);
    moveTurn(-4);
    delay(500);
  }*/
}

void loop()
{
  delay(100);
}

void zigzag() {
  int numberZigZags = 5;
  //Start from top right, move down, overall zigzag towards the right
  for (int i = 0; i < numberZigZags; i++) {
    moveUntilSeeLine();
    moveStraight(700);

    bool topOrBottom; //At top edge = 1, Bottom edge = 0

    if(facingDeg == 2) {
      moveTurn(-1);
      topOrBottom = 0;
    }
    if(facingDeg == 0) {
      moveTurn(1);
      topOrBottom = 1;
    }

    moveStraight(400);

    if(topOrBottom == 0) {
      moveTurn(-1);
    }
    if(topOrBottom == 1) {
      moveTurn(1);
    }

    moveStraight(900);
  }
}

void moveStraight(int dist) { //fwd is +ve
  if (dist > 0) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(RMotorSpeed);
    delay(dist);
  }
  if (dist < 0) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(-RMotorSpeed);
    delay(-dist);
  }
  motorLeft.stop();
  motorRight.stop();
}

void moveTurn(int deg) { //right is +ve, heading increase clockwise
  if (deg > 0) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(-RMotorSpeed);
    delay(deg*RTurnTime);

    if ((facingDeg + deg) < 4) {
      facingDeg = facingDeg + deg;
    }
    else {
      facingDeg = facingDeg + deg - 4;
    }
  }

  if (deg < 0) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(RMotorSpeed);
    delay(-deg*LTurnTime);

    if ((facingDeg + deg) >= 0) {
      facingDeg = facingDeg + deg;
    }
    else {
      facingDeg = facingDeg + deg + 4;
    }
  }
  motorLeft.stop();
  motorRight.stop();

  Serial.print("facingDeg: ");
  Serial.println(facingDeg);

}

void updateLineIRSense() { //Returns array for L & R, 1 = see black
  int IROutput = lineFinder.readSensors();
  
  switch(IROutput)
  {
    case S1_IN_S2_IN: lineIRSenseState[0] = 1; lineIRSenseState[1] = 1; break;
    case S1_IN_S2_OUT: lineIRSenseState[0] = 1; lineIRSenseState[1] = 0; break;
    case S1_OUT_S2_IN: lineIRSenseState[0] = 0; lineIRSenseState[1] = 1; break;
    case S1_OUT_S2_OUT: lineIRSenseState[0] = 0; lineIRSenseState[1] = 0; break;
    default: break;
  }

  Serial.print("IRstate: ");
  Serial.print(lineIRSenseState[0]);
  Serial.print(lineIRSenseState[1]);
}

void moveUntilSeeLine() { //Will reposition to ensure it is perpendicular to the line
  motorLeft.run(LMotorSpeed);
  motorRight.run(RMotorSpeed);
  bool seenLine = 0; //Flag exists such that if robot sees line but overshoots when adjusting, it knows to exit
  bool exitLoop = 0;
  while (exitLoop == 0) {  
    updateLineIRSense();

    if (lineIRSenseState[0] == 1 || lineIRSenseState[1] == 1) {
      seenLine = 1;

      if (lineIRSenseState[0] == 1 && lineIRSenseState[1] == 1) {
        Serial.println("Both sides in line");
        exitLoop = 1;
      }

      if (lineIRSenseState[0] == 0 && lineIRSenseState[1] == 1) { //Right is in but left is out
        //Add some on left side
        Serial.println("Only right side in line");
        motorRight.stop();
        motorLeft.run(LMotorSpeed);
        delay(60);
        motorLeft.stop();
      }
      if (lineIRSenseState[0] == 1 && lineIRSenseState[1] == 0) { //Left is in but right is not
        //Add some on right side
        Serial.println("Only left side in line");
        motorLeft.stop();
        motorRight.run(RMotorSpeed);
        delay(60);
        motorRight.stop();
      }
    }
    else if(seenLine == 1) {
      exitLoop = 1;
    }
    else {
      //delay(5);
    }

    delay(5);
  }

  motorLeft.stop();
  motorRight.stop();
}