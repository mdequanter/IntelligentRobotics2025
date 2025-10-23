
#include "songs.h"
#include "sensors.h"

void setup() {
  Serial.begin(115200);
  Serial.println("Launching IMU");
  IMU.begin();
  Serial.println("Playing start melody");
  playSequence(HappyBirthday,noteDurations_HappyBirthday);
  pinMode( led_pin, OUTPUT );
}

void loop() {

    float roll = getRoll();
    float yaw = getYaw();
    float pitch = getPitch();

    float voltage = getBatteryVoltage();
    
    Serial.println(voltage);

    Serial.print("Roll:");
    Serial.print(roll);
    Serial.print(",Pitch:");
    Serial.print(pitch);
    Serial.print(",Yaw:");
    Serial.println(yaw);
    delay(100);  


}
