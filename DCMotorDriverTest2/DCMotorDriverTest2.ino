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
MeBuzzer buzzer;

//Connections
MeDCMotor motorLeft(M1);
MeDCMotor motorRight(M2);
//Actual port number = port number on board + 3
MeLineFollower lineFinder(PORT_6); /* Line Finder module can only be connected to PORT_3, PORT_4, PORT_5, PORT_6 of base shield. */ 
MeUltrasonicSensor ultraSensor(PORT_7); /* Ultrasonic module can ONLY be connected to port 3, 4, 6, 7, 8 of base shield. */
const int pressureSensorPin = 9;


//Movement Constants
int LMotorSpeed = -85;
int RMotorSpeed = 70+5;
int LTurnTime = 850+100; //Time taken to turn 90deg
int RTurnTime = 880+100; //Time taken to turn 90deg
int timeToLineMiddle = 700; //timeFromFirstSeeLineToMovementCentreOverLine

//Global variables
int facingDeg = 2; //Clockwise is +ve     0: North, 1: East, 2: South, 3: West ^><v
bool lineIRSenseState[2] = {0, 0};

void setup()
{
  pinMode(pressureSensorPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //buzzer.tone(440);
  Serial.begin(115200);
  pinMode(A0, INPUT);
  //delay(1000);
  //buzzer.noTone();
  //Serial.println(digitalRead(pressureSensorPin) == 0);
  //zigzag(5);
  /*for (int m = 0; m < 8; m++) {
    moveTurn(4);
    delay(500);
    moveTurn(-4);
    delay(500);
  }*/
  //moveTurnUntilSeeLine(-1);
  //moveStraightFollowLine(5000);
  zigzag_two(3);
  returnHome();
}

void loop()
{
  delay(100);
}

void zigzag_two(int numberZigZags) { //Version that follows the line when its on the edge
//Start from top right, facing down (south), overall zigzag towards the right
  for (int i = 0; i < numberZigZags; i++) {
    moveStraightUntilSeeLineWithObstDetectAndDustAlarm(); //Will find obstacle here
    moveStraight(timeToLineMiddle);

    bool topOrBottom; //At top edge = 1, Bottom edge = 0

    if(facingDeg == 2) {
      moveTurnUntilSeeLine(-1); //Left
      topOrBottom = 0;
    }
    if(facingDeg == 0) {
      moveTurnUntilSeeLine(1);
      topOrBottom = 1; //Right
    }

    moveStraightFollowLine(1200);

    if(topOrBottom == 0) {
      moveTurn(-1); //Left
    }
    if(topOrBottom == 1) {
      moveTurn(1); //Right
    }

    moveStraight(600);
    //In the square and have completed a uturn compared to before entering previous iteration of loop
  }
}

void updateLineIRSense() { //Returns array for L & R, 1 = see black (ie see line)
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


void moveStraightUntilSeeLine() { //DEPRECATED //Will reposition to ensure it is perpendicular to the line
  motorLeft.run(LMotorSpeed);
  motorRight.run(RMotorSpeed);
  bool seenLine = 0; //Flag exists such that if robot sees line but overshoots when adjusting, it knows to exit
  bool exitLoop = 0; //To exit the perpetual loop that runs until function ends

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

    delay(5);
  }

  motorLeft.stop();
  motorRight.stop();
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

    if ((facingDeg - 1) >= 0) {
      facingDeg = facingDeg - 1;
    }
    else {
      facingDeg = facingDeg - 1 + 4;
    }
  }
}


void moveStraightUntilSeeLineWithObstDetectAndDustAlarm() { //Will reposition to ensure it is perpendicular to the line, will flag if it detects obstacle
  motorLeft.run(LMotorSpeed);
  motorRight.run(RMotorSpeed);
  bool seenLine = 0; //Flag exists such that if robot sees line but overshoots when adjusting, it knows to exit
  bool exitLoop = 0; //Flag to exit function

  while (exitLoop == 0) {  
    updateLineIRSense();
    detectDustbagAndAlarm();

    if (avoidObject()) {
      motorLeft.run(LMotorSpeed);
      motorRight.run(RMotorSpeed);
    }

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


bool avoidObject()  { //Once it sees an object, turn right, move, turn left, move, turn left, move, turn right, go straight until see line //Assumes obstscle is a cube-ish
  int checkDist = 5; // dist before checking for object
  int objectWidthMoveDist = 1200; //width of object face it sees
  int objectLengthMoveDist = 2000; //length of object face perpendicular to first face


  if (ultraSensor.distanceCm() < checkDist) { //distance from obstacle, in cm
    if (facingDeg == 0) {
      moveTurn(1);
      moveStraight(objectWidthMoveDist);
      moveTurn(-1);
      moveStraight(objectLengthMoveDist);
      moveTurn(-1);
      moveStraight(objectWidthMoveDist);
      moveTurn(1);
    }

    if (facingDeg == 2) {
      moveTurn(-1);
      moveStraight(objectWidthMoveDist);
      moveTurn(1);
      moveStraight(objectLengthMoveDist);
      moveTurn(1);
      moveStraight(objectWidthMoveDist);
      moveTurn(-1);
    }

    return true;
  }

  else {
    return false;
  }
}

bool detectDustbagAndAlarm() {
  int inputPinVal = digitalRead(pressureSensorPin);
  if(inputPinVal == 0) {
    motorLeft.stop();
    motorRight.stop();
    
    for(int i = 0; i < 5; i++) { //Lock up the robot forever
      digitalWrite(LED_BUILTIN, HIGH);
      buzzer.tone(840, 500);      
      buzzer.noTone();
      digitalWrite(LED_BUILTIN, LOW);
      delay(500);
    }

    while(true) {
      delay(1000);
    }
  }

  return false;
}


int HomingMoveStraightFollowLine() { //Returns 0 if runs out of line, returns -1 if doesnt see line at beginning, return 2 if docked
  updateLineIRSense();

  if (lineIRSenseState[0] == 0 && lineIRSenseState[1] == 0) {
    return -1;
  }

  bool atDock = 0;

  while (atDock == 0) {
    updateLineIRSense();

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
    else if(lineIRSenseState[0] == 0 && lineIRSenseState[1] == 0){
      motorLeft.stop();
      motorRight.stop();
      return 0;
    }
    if((analogRead(A0) < 973) && (ultraSensor.distanceCm() < 7)) {
      motorLeft.stop();
      motorRight.stop();
      return 2;
    }

    delay(10);
    motorLeft.stop();
    motorRight.stop();
  }
}

bool returnHome() { //978 if out of range, 971 if nearby, 970 if at the dock
  moveStraightUntilSeeLine();
  moveStraight(timeToLineMiddle);
  moveTurn(1);

  bool docked = 0;

  while(docked == 0) {
    int returnVal = HomingMoveStraightFollowLine();

    if (returnVal == 2) {
      docked = 1;
      return 1;
    }
    if (returnVal == 0) {
      moveStraight(500);
      moveTurn(1);
    }
    if (returnVal == -1) {
      moveTurnUntilSeeLine(1);
    }
  }
}
