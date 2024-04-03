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
uint8_t LMotorSpeed = 80;
uint8_t RMotorSpeed = 80;
int LTurnTime = 100;
int RTurnTime = 100;

void setup()
{
}

void loop()
{
  motorLeft.run(LMotorSpeed); /* value: between -255 and 255. */
  motorRight.run(-RMotorSpeed);
  delay(3000);
  motorLeft.stop();
  motorRight.stop();
  delay(100);
  motorLeft.run(-LMotorSpeed);
  motorRight.run(RMotorSpeed);
  delay(3000);
  motorLeft.stop();
  motorRight.stop();
  delay(2000);
}

/*void moveLCompensated(int dist) {
  motorLeft.run(-LMotorSpeed);
}

void moveRCompensated(int dist) {
  motorRight.run(RMotorSpeed);
}*/

void moveStraight(int dist) {
  if (dist > 0) {
    motorLeft.run(-LMotorSpeed);
    motorRight.run(RMotorSpeed);
  }
  if (dist < 0) {
    motorLeft.run(LMotorSpeed);
    motorRight.run(-RMotorSpeed);
  }
  delay(dist);
}

void turn(int deg) {
  if (deg > 0) {
    motorLeft.run(-LMotorSpeed);
    delay(((float)deg/90.0)*LTurnTime);
  }
  if (deg < 0) {
    motorRight.run(RMotorSpeed);
    delay(((float)deg/90.0)*RTurnTime);
  }
  
}