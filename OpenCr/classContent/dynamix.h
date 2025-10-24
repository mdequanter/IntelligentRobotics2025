/*******************************************************************************
* Copyright 2016 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Authors: Taehun Lim (Darby) */

#include <DynamixelWorkbench.h>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" //Dynamixel on Serial3(USART3)  <-OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif          

#define BAUDRATE  1000000

DynamixelWorkbench dxl_wb;

int maxSpeed = 100;

bool checkDynamixel() 
{
  Serial.begin(57600);
  //while(!Serial); // Wait for Opening Serial Monitor

  const char *log = NULL;
  bool result = false;

  uint8_t scanned_id[16];
  uint8_t dxl_cnt = 0;
  uint8_t range = 100;

  result = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to init");
  }
  else
  {
    Serial.print("Succeeded to init : ");
    Serial.println(BAUDRATE);
  }

  Serial.println("Wait for scan...");
  result = dxl_wb.scan(scanned_id, &dxl_cnt, range, &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("Failed to scan");
    return false;
  }
  else
  {
    Serial.print("Find ");
    Serial.print(dxl_cnt);
    Serial.println(" Dynamixels");
    int counter = 0;
    for (int cnt = 0; cnt < dxl_cnt; cnt++)
    {
      counter++;
      Serial.print("id : ");
      Serial.print(scanned_id[cnt]);
      Serial.print(" model name : ");
      Serial.println(dxl_wb.getModelName(scanned_id[cnt]));
    }
    if (counter >=1) {
      return true;
    } else {
      return false;
    }
  }  
}

bool setMode(int dxl_id, int mode){
  const char *log;
  dxl_wb.wheelMode(dxl_id, mode, &log);
  return true;
}

bool setVelocity(int vel1, int vel2){
  if (abs(vel1) > maxSpeed) {
    vel1 = 0;
    return false;
  }
  if (abs(vel2) > maxSpeed) {
    vel2 = 0;
    return false;
  }

  dxl_wb.goalVelocity(1, vel1);
  dxl_wb.goalVelocity(2, vel2);
  return true;
}

bool drive(int vel1, int vel2, int duration) {
  unsigned long startTime = millis();   // Starttijd vastleggen
  setVelocity(vel1, vel2);
  while (millis() - startTime <= (unsigned long)duration * 1000) {
    delay(20);
  }
  setVelocity(0, 0);
  return true;
}


bool turn(float degrees = 0.0) {

  float currentYaw = getYaw();
  float initialYaw = 0;

  float goalYaw = initialYaw + degrees;

  // turn CW
  if (goalYaw > 0) {
    setVelocity(50,-50);  
    Serial.println("turn CW");
    while (currentYaw <= goalYaw) {
      currentYaw = getYaw();
      delay(20);
    }
    setVelocity(0,0);
    return true;
  }
  //turn CCW 
  if (goalYaw < 0) {
    setVelocity(-50,-50);  
    Serial.println("turn CCW");
    while (currentYaw >= goalYaw) {
      currentYaw = getYaw();
      delay(20);
    }
    setVelocity(0,0);
    return true;
  }
}


