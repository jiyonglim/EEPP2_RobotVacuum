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
MeUltrasonicSensor ultraSensor(PORT_7); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */


//Constants
int LMotorSpeed = -85;
int RMotorSpeed = 70;
int LTurnTime = 900; //Time taken to turn 90deg
int RTurnTime = 930; //Time taken to turn 90deg

//Global variables
int facingDeg = 2; //Clockwise is +ve     0: North, 1: East, 2: South, 3: West ^><v
bool lineIRSenseState[2] = {0, 0};

void setup()
{
  //Serial.begin(115200);
  //zigzag(5);
  /*for (int m = 0; m < 8; m++) {
    moveTurn(4);
    delay(500);
    moveTurn(-4);
    delay(500);
  }*/
  //moveTurnUntilSeeLine(-1);
  //moveStraightFollowLine(5000);
  zigzag_two(5);
}

void loop()
{
  delay(100);
}

void zigzag_two(int numberZigZags) { //Version that follows the line when its on the edge
//Start from top right, move down, overall zigzag towards the right
  for (int i = 0; i < numberZigZags; i++) {
    moveStraightUntilSeeLine();
    moveStraight(700);

    bool topOrBottom; //At top edge = 1, Bottom edge = 0

    if(facingDeg == 2) {
      moveTurnUntilSeeLine(-1);
      topOrBottom = 0;
    }
    if(facingDeg == 0) {
      moveTurnUntilSeeLine(1);
      topOrBottom = 1;
    }

    moveStraightFollowLine(400);

    if(topOrBottom == 0) {
      moveTurn(-1);
    }
    if(topOrBottom == 1) {
      moveTurn(1);
    }

    moveStraight(600);
  }
}

void zigzag(int numberZigZags) {
  //Start from top right, move down, overall zigzag towards the right
  for (int i = 0; i < numberZigZags; i++) {
    moveStraightUntilSeeLine();
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

    moveStraight(600);
  }
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


void moveTurnUntilSeeLine(int input) { //Will turn left or right until it sees a black line, should turn ~90deg only 
  if (input == 1) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(-RMotorSpeed);

    bool exitLoop = 0;
    while (exitLoop == 0) {
      updateLineIRSense();
      if (lineIRSenseState[0] == 1) {
        exitLoop = 1;
        motorLeft.stop();
        motorRight.stop();
      }
    }
    //delay(deg*RTurnTime);

    if ((facingDeg + 1) < 4) {
      facingDeg = facingDeg + 1;
    }
    else {
      facingDeg = facingDeg + 1 - 4;
    }

  }


  else if (input == -1) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(RMotorSpeed);
    
    bool exitLoop = 0;
    while (exitLoop == 0) {
      updateLineIRSense();
      if (lineIRSenseState[1] == 1) {
        exitLoop = 1;
        motorLeft.stop();
        motorRight.stop();
      }
    }


    //delay(-deg*LTurnTime);

    if ((facingDeg - 1) >= 0) {
      facingDeg = facingDeg - 1;
    }
    else {
      facingDeg = facingDeg - 1 + 4;
    }
  }
}


void moveStraightUntilSeeLine() { //Will reposition to ensure it is perpendicular to the line
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


int moveStraightFollowLine(double distance) { //Returns 1 if finishes following line for duration successfully, returns 0 if runs out of line, returns -1 if doesnt see line at beginning
  updateLineIRSense();
  if (lineIRSenseState[0] == 0 && lineIRSenseState[1] == 0) {
    return -1;
  }
  
  long startTime = millis();
  long endTime = millis();

  while ((endTime - startTime) < distance) {
    updateLineIRSense();
    endTime = millis();

    if(lineIRSenseState[0] == 1 || lineIRSenseState[1] == 1) {
      if(lineIRSenseState[0] == 0 && lineIRSenseState[1] == 1) { //No line on left --> turn right
        motorLeft.run(LMotorSpeed);
        motorRight.run(-RMotorSpeed);
      }
      if(lineIRSenseState[0] == 1 && lineIRSenseState[1] == 0) { //No line on right --> turn left
        motorLeft.run(-LMotorSpeed);
        motorRight.run(RMotorSpeed);
      }
      if(lineIRSenseState[0] == 1 && lineIRSenseState[1] == 1) { //Line on both sides
        motorLeft.run(LMotorSpeed);
        motorRight.run(RMotorSpeed);
      }
    }
    else if((endTime - startTime) >= distance){
      return 1;
    }
    else if(lineIRSenseState[0] == 0 && lineIRSenseState[1] == 0){
      return 0;
    }

    delay(10);
    motorLeft.stop();
    motorRight.stop();
  }

  return 1;
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


void avoidObject()  {
  int checkStepDIST = 0.5;    // dist before checking for object
  int objectDist = 4;         // dist  away from object

  if (ultraSensor.distanceCm() < objectDist) //distance from bottle, in cm
  {
    if (facingDeg == 0)
    {
      moveTurn(1);
      moveStraight(checkStepDIST);
      moveTurn(-1);
      avoidObject();
    }
  }
}
