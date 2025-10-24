
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
  pinMode( led_pin, OUTPUT );
  
  
  dynamixelStarted = checkDynamixel();

  if (dynamixelStarted == false) {
    Serial.println("Dynamixels not initiated.");
  }

  if (dynamixelStarted == true) {
    setMode(1,0);
    setMode(2,0);
    setVelocity(0,0);
  }

  Serial.println("Playing start melody");
  playSequence(HappyBirthday,noteDurations_HappyBirthday);


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


void printIMU(){
    float roll = getRoll();
    float yaw = getYaw();
    float pitch = getPitch();
    Serial.println("Roll:" + String(roll) + ",Pitch:" + String(pitch) + ",Yaw:" + String(yaw));
}


void loop() {

    bool criticalVoltage = false;
    int velocity = true;

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

    //velocity = drive(50,-50,1);
    //velocity = drive(0,0,1);
    //velocity = drive(-50,50,1);

    //Serial.println("Turn clockwise");
    //turn(90);

    bool SW1State = getSW1State();
    bool SW2State = getSW2State();

    //Serial.println("States SW1/SW2:" + String(SW1State) + "/" + String(SW2State));

    if (SW1State == 1) {
      drive(50,-50,1);
    }
    if (SW2State == 1) {
      drive(-50,50,1);
    }

    delay(100);  

}
