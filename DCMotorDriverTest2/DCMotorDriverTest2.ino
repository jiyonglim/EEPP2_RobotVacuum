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


//Constants
uint8_t LMotorSpeed = 95;
uint8_t RMotorSpeed = 85;
int LTurnTime = 1100;
int RTurnTime = 1100;

void setup()
{
}

void loop()
{
  moveStraight(3000);
  delay(1000);
  moveStraight(-3000);
  delay(1000);
  turn(90);
  delay(1000);
  turn(-90);
}

/*void moveLCompensated(int dist) {
  motorLeft.run(-LMotorSpeed);
}

void moveRCompensated(int dist) {
  motorRight.run(RMotorSpeed);
}*/

void moveStraight(int dist) { //fwd is +ve
  if (dist > 0) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(RMotorSpeed);
    delay(dist);
  }
  if (dist < 0) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(-RMotorSpeed);
    delay(-dist);
  }
  motorLeft.stop();
  motorRight.stop();
}

void turn(int deg) { //right is +ve
  if (deg > 0) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(-RMotorSpeed);
    delay(((float)deg/90.0)*RTurnTime);
  }
  if (deg < 0) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(RMotorSpeed);
    delay(((float)-deg/90.0)*LTurnTime);
  }
  motorLeft.stop();
  motorRight.stop();
}