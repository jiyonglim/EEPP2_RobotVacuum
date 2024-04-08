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

// MeDCMotor motor1(PORT_1);
// MeDCMotor motor2(PORT_2);
MeDCMotor motor1(M1);
MeDCMotor motor2(M2);

uint8_t speedM1 = 254;
uint8_t speedM2 = -200;
int ECHO_PIN = 12;
int TRIG_PIN = 13;
float SPEED_OF_SOUND = 0.0345;

void setup()
{
}

void loop()
{
  motor1.run(speedM1); /* value: between -255 and 255. */
  motor2.run(speedM2); /* value: between -255 and 255. */
  delay(5000);

  // motor1.stop();
  // motor2.stop();
  // motor3.stop();
  // motor4.stop();
  // delay(100);

  // motor1.run(-motorSpeed);
  // motor2.run(-motorSpeed);
  // motor3.run(-motorSpeed);
  // motor4.run(motorSpeed);
  // delay(5000);

  // motor1.stop();
  // motor2.stop();
  // motor3.stop();
  // motor4.stop();
  // delay(100);

  // motor3.run(motorSpeed);
  // delay(100);
  // motor4.run(motorSpeed);
  // delay(100);
  // motor3.stop();
  // motor4.stop();
  // delay(50);

  // motor4.run(-motorSpeed);
  // delay(100);
  // motor3.run(-motorSpeed);
  // delay(100);
  // motor3.stop();
  // motor4.stop();
  // delay(50);

}

