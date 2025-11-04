/*
  Range   : Roll  : +/- 180 deg/sec
            Pitch : +/- 180 deg/sec
            Yaw   : +/- 180 deg/sec
  Scale   : Roll  : 1 = 1 deg/sec
            Pitch : 1 = 1 deg/sec
            Yaw   : 1 = 1 deg/sec
 */

#include <IMU.h>
cIMU    IMU;
uint8_t   err_code;
uint8_t   led_tog = 0;
uint8_t   led_pin = 13;


float getRoll(){
  IMU.update();
  return (IMU.rpy[0]);
}

float getPitch(){
  IMU.update();
  return (IMU.rpy[1]);
}

float getYaw(){
  IMU.update();
  return (IMU.rpy[2]);
}

bool getSW1State(){
  // SW1 = 34
  int buttonState = digitalRead(34);
  return (buttonState);
}

bool getSW2State(){
  // SW1 = 35
  int buttonState = digitalRead(35);
  return (buttonState);
}



float getBatteryVoltage() {
  int raw = analogRead(BDPIN_BAT_PWR_ADC);
  float voltage = (float)raw / 56.01; // spanningsdeler x11
  return voltage;
}
