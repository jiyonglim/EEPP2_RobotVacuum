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
MeLineFollower lineFinder(PORT_7); /* Line Finder module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield. */
MeUltrasonicSensor ultraSensor(PORT_6); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */


//Constants
int LMotorSpeed = -95;
int RMotorSpeed = 85;
int LTurnTime = 700;
int RTurnTime = 700;

//Global variables
int facingDeg = 2; //0: North, 1: East, 2: South, 3: West ^><v
bool lineIRSenseState[2] = {0, 0};

bool dir = true;

void setup()
{
  Serial.begin(9600);
  moveUntilSeeLine();
  moveTurn(-1);
  moveStraight(300);
  moveTurn(-1);
  moveUntilSeeLine();
}

void loop()
{
  //bool dir == true;
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

void moveTurn(int deg) { //right is +ve
  if (deg > 0) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(-RMotorSpeed);
    delay(deg*RTurnTime);

    if ((facingDeg + deg) < 4) {
      deg = deg + facingDeg;
    }
    else {
      deg = deg + facingDeg - 4;
    }
  }

  if (deg < 0) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(RMotorSpeed);
    delay(-deg*LTurnTime);

    if ((facingDeg + deg) >= 0) {
      deg = deg + facingDeg;
    }
    else {
      deg = deg + facingDeg + 4;
    }
  }

  motorLeft.stop();
  motorRight.stop();
}

void updateLineIRSense() { //Returns array for L & R, 1 = see black
  int IROutput = lineFinder.readSensors();
  
  switch(IROutput)
  {
    case S1_IN_S2_IN: lineIRSenseState[0] = 1; lineIRSenseState[1] = 0; break;
    case S1_IN_S2_OUT: lineIRSenseState[0] = 0; lineIRSenseState[1] = 0; break;
    case S1_OUT_S2_IN: lineIRSenseState[0] = 1; lineIRSenseState[1] = 0; break;
    case S1_OUT_S2_OUT: lineIRSenseState[0] = 1; lineIRSenseState[1] = 1; break;
    default: break;
  }

  Serial.println(IROutput);
}


void avoidObject()  {
  int checkStepDIST = 0.5;    // dist before checking for object
  int objectDist = 4;         // dist  away from object

  if (ultraSensor.distanceCm() < objectDist) //distance from bottle, in cm
  {
    if (dir == 1)
    {
      moveTurn(1);
      moveStraight(checkStepDIST);
      moveTurn(-1);
      avoidObject();
    }
  }
}

void moveUntilSeeLine() {
  motorLeft.run(LMotorSpeed);
  motorRight.run(RMotorSpeed);
  while (true) {
    updateLineIRSense();
    if (lineIRSenseState[0] == 1 || lineIRSenseState[1] == 1) {
      break;
    }
    delay(50);
  }
  motorLeft.stop();
  motorRight.stop();
}