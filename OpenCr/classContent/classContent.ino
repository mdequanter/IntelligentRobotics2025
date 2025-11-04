#include "songs.h"
#include "sensors.h"
#include "dynamix.h"

int criticalNote = NOTE_C4;
float criticalVoltage = 11.5 ;

unsigned long lastBatteryCheck = 0;
bool dynamixelStarted =  false;

// === Nieuw: helper voor seriële commando's ===
void handleSerialCommand() {
  // Alleen iets doen als er data is
  if (!Serial.available()) return;

  // Sla witruimte/nieuwe regels over tot we een echte letter hebben
  int ch;
  do {
    ch = Serial.read();
    if (ch < 0) return; // niets bruikbaars
  } while (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');

  char cmd = (char)ch;
  if (cmd >= 'a' && cmd <= 'z') cmd = cmd - 'a' + 'A'; // lowercase → UPPERCASE

  // Lees achterliggende integers; parseInt slaat zelf spaties/CR/LF over
  long a = Serial.parseInt(); // kan ook negatief
  long b = Serial.parseInt();
  long c = Serial.parseInt();

  switch (cmd) {
    case 'V':
      // Verwacht: V <left> <right>
      setVelocity((int)a, (int)b);
      Serial.println(String("OK: setVelocity(") + a + "," + b + ")");
      break;

    case 'D':
      // Verwacht: D <left> <right> <seconds>
      drive((int)a, (int)b, (int)c);
      Serial.println(String("OK: drive(") + a + "," + b + "," + c + ")");
      break;

    case 'B': {
      float voltage = getBatteryVoltage();
      Serial.println(String("BatteryVoltage:") + voltage + " V");
      break;
    }

    default:
      Serial.println("ERROR: Unknown command. Use 'V <L> <R>' or 'D <L> <R> <t>'");
      break;
  }

  // Rest van de lijn weggooien tot en met newline, zodat we schoon starten
  while (Serial.available()) {
    int peeked = Serial.peek();
    if (peeked == '\n') { Serial.read(); break; }
    Serial.read();
  }
}
// === Einde seriële helper ===

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50); // korte timeout voor parseInt zodat we niet blokkeren
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
    // === Nieuw: verwerk seriële commando's elke iteratie ===
    handleSerialCommand();

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
