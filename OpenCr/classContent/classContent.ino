
#include "songs.h"
#include "sensors.h"
#include "dynamix.h"

int criticalNote = NOTE_C4;
float criticalVoltage = 11.5 ;

unsigned long lastBatteryCheck = 0;
bool dynamixelStarted =  false;


void setup() {
  Serial.begin(115200);
  Serial.println("Launching IMU");
  IMU.begin();
  Serial.println("Playing start melody");
  playSequence(HappyBirthday,noteDurations_HappyBirthday);
  pinMode( led_pin, OUTPUT );
  
  
  dynamixelStarted = checkDynamixel();

  if (dynamixelStarted == false) {
    Serial.println("Dynamixels not initiated.");
  }

}

bool checkCriticalBattery(){
  float voltage = getBatteryVoltage();
  Serial.println("Executed: checkCriticalBattery ");

  if (voltage < criticalVoltage) {
    tone(BDPIN_BUZZER, criticalNote, 100);
    return true;
  }

  return false;
}

void loop() {

    float roll = getRoll();
    float yaw = getYaw();
    float pitch = getPitch();

    bool criticalVoltage = false;

    float voltage = getBatteryVoltage();
    
    // Controleer of er 60 seconden voorbij zijn sinds de laatste batterijcheck
    if (millis() - lastBatteryCheck >= 60000) {
        lastBatteryCheck = millis();           // Reset timer
        criticalVoltage = checkCriticalBattery();  // Controleer opnieuw
    }

    if (dynamixelStarted == false) {
      Serial.println("Problems with dynamixels");
      delay(1000);
    }
    
    Serial.print("Roll:");
    Serial.print(roll);
    Serial.print(",Pitch:");
    Serial.print(pitch);
    Serial.print(",Yaw:");
    Serial.print(yaw);
    Serial.print(",Voltage:");
    Serial.println(voltage);
    delay(100);  
}
