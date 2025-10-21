#include <DynamixelWorkbench.h>

#if defined(__OPENCM904__)
  #define DEVICE_NAME "3" // Dynamixel on Serial3(USART3) <- OpenCM 485EXP
#elif defined(__OPENCR__)
  #define DEVICE_NAME ""
#endif

#define BAUDRATE     1000000
#define DXL_ID_1     1      // wiel links/rechts (wheel mode)
#define DXL_ID_2     2
#define DXL_ID_Z     14     // hoogte (joint/position mode)
#define DXL_ID_GRIP  15     // grijper (joint/position mode)

#define PROFILE_VELOCITY     50
#define PROFILE_ACCELERATION 10

// Je gewenste grenzen:
#define Z_MIN_DEG     0.0f
#define Z_MAX_DEG     160.0f
#define GRIP_MIN_DEG  0.0f
#define GRIP_MAX_DEG  120.0f

DynamixelWorkbench dxl_wb;

// ---------- Helpers wheel control ----------
bool setWheelMode(uint8_t id) {
  const char* log;
  bool ok = dxl_wb.wheelMode(id, 0, &log);
  if (!ok) {
    Serial.print("Failed to set wheel mode on ID ");
    Serial.print(id);
    Serial.print(": ");
    Serial.println(log);
  }
  return ok;
}

bool setVelocity(uint8_t id, int32_t vel) {
  const char* log;
  bool ok = dxl_wb.goalVelocity(id, vel, &log);
  if (!ok) {
    Serial.print("Failed to set velocity on ID ");
    Serial.print(id);
    Serial.print(": ");
    Serial.println(log);
  }
  return ok;
}

// ---------- Helpers joint/position control ----------
bool setJointMode(uint8_t id, uint32_t prof_vel = PROFILE_VELOCITY, uint32_t prof_acc = PROFILE_ACCELERATION) {
  const char* log;
  bool ok = dxl_wb.jointMode(id, prof_vel, prof_acc, &log);
  if (!ok) {
    Serial.print("Failed to set joint mode on ID ");
    Serial.print(id);
    Serial.print(": ");
    Serial.println(log);
  }
  return ok;
}

// Nieuw: setter die zelf clamp toepast met min/max grenzen
bool setPositionDeg(uint8_t id, float deg, float min_deg = 0.0f, float max_deg = 300.0f) {
  if (deg < min_deg) deg = min_deg;
  if (deg > max_deg) deg = max_deg;

  float rad = deg * (PI / 180.0f);
  int32_t pos_val = dxl_wb.convertRadian2Value(id, rad);

  const char* log;
  bool ok = dxl_wb.goalPosition(id, pos_val, &log);
  if (!ok) {
    Serial.print("Failed to set position on ID ");
    Serial.print(id);
    Serial.print(": ");
    Serial.println(log);
  }
  return ok;
}

// ---------- Parsing ----------
static bool isNumeric(const String &s) {
  if (s.length() == 0) return false;
  int i = 0;
  if (s[0] == '+' || s[0] == '-') i = 1;
  for (; i < s.length(); i++) if (!isDigit(s[i])) return false;
  return true;
}

void splitCSV(const String &line, String parts[], int &count, int maxParts) {
  count = 0;
  int start = 0;
  while (count < maxParts-1) {
    int idx = line.indexOf(',', start);
    if (idx < 0) break;
    parts[count++] = line.substring(start, idx);
    start = idx + 1;
  }
  parts[count++] = line.substring(start);
  for (int i = 0; i < count; i++) parts[i].trim();
}

bool parseTwoSpeeds(const String &line, int &v1, int &v2) {
  String parts[2]; int n=0;
  splitCSV(line, parts, n, 2);
  if (n != 2) return false;
  if (!isNumeric(parts[0]) || !isNumeric(parts[1])) return false;
  v1 = parts[0].toInt();
  v2 = parts[1].toInt();
  v1 = v1 < -100 ? -100 : (v1 > 100 ? 100 : v1);
  v2 = v2 < -100 ? -100 : (v2 > 100 ? 100 : v2);
  return true;
}

bool parseTwoSpeedsTwoAngles(const String &line, int &v1, int &v2, float &h_deg, float &g_deg) {
  String parts[4]; int n=0;
  splitCSV(line, parts, n, 4);
  if (n != 4) return false;
  if (!isNumeric(parts[0]) || !isNumeric(parts[1])) return false;
  v1 = parts[0].toInt();
  v2 = parts[1].toInt();
  h_deg = parts[2].toFloat();
  g_deg = parts[3].toFloat();
  v1 = v1 < -100 ? -100 : (v1 > 100 ? 100 : v1);
  v2 = v2 < -100 ? -100 : (v2 > 100 ? 100 : v2);
  return true;
}

// ---------- Setup & loop ----------
void setup() {
  Serial.begin(57600);
  while(!Serial);

  const char *log;
  bool ok = dxl_wb.init(DEVICE_NAME, BAUDRATE, &log);
  if (!ok) {
    Serial.println(log);
    Serial.println("Failed to init");
  } else {
    Serial.print("Succeeded to init : ");
    Serial.println(BAUDRATE);
  }

  uint16_t model;
  uint8_t ids[] = {DXL_ID_1, DXL_ID_2, DXL_ID_Z, DXL_ID_GRIP};
  for (uint8_t i = 0; i < sizeof(ids)/sizeof(ids[0]); i++) {
    if (!dxl_wb.ping(ids[i], &model, &log)) {
      Serial.print("Ping failed for ID "); Serial.print(ids[i]); Serial.print(": "); Serial.println(log);
    } else {
      Serial.print("ID "); Serial.print(ids[i]); Serial.print(" model_number: "); Serial.println(model);
    }
  }

  setWheelMode(DXL_ID_1);
  setWheelMode(DXL_ID_2);

  setJointMode(DXL_ID_Z);
  setJointMode(DXL_ID_GRIP);

  setVelocity(DXL_ID_1, 0);
  setVelocity(DXL_ID_2, 0);
  setPositionDeg(DXL_ID_Z, 0.0f, Z_MIN_DEG, Z_MAX_DEG);
  setPositionDeg(DXL_ID_GRIP, 0.0f, GRIP_MIN_DEG, GRIP_MAX_DEG);

  Serial.println();
  Serial.println("Klaar.");
  Serial.println("Wielen: v1,v2          (bv. 40,-20)");
  Serial.println("Wielen + Z + Grip: v1,v2,h_deg,g_deg   (bv. 30,-30,120,45)");
  Serial.println("Bereiken: v∈[-100..100], Z∈[0..160]°, Grip∈[0..120]°.");
  Serial.println();
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    int v1, v2;
    float h_deg, g_deg;

    if (parseTwoSpeedsTwoAngles(input, v1, v2, h_deg, g_deg)) {
      // Zet wielen
      bool ok1 = setVelocity(DXL_ID_1, (int32_t)v1);
      bool ok2 = setVelocity(DXL_ID_2, (int32_t)v2);

      // Zet posities met limieten
      bool okZ = setPositionDeg(DXL_ID_Z,   h_deg, Z_MIN_DEG,   Z_MAX_DEG);
      bool okG = setPositionDeg(DXL_ID_GRIP, g_deg, GRIP_MIN_DEG, GRIP_MAX_DEG);

      if (ok1 && ok2 && okZ && okG) {
        Serial.print("OK -> ID1: "); Serial.print(v1);
        Serial.print(" | ID2: "); Serial.print(v2);
        Serial.print(" | Hoogte(deg): "); Serial.print(h_deg, 1);
        Serial.print(" (clamp "); Serial.print(Z_MIN_DEG); Serial.print(".."); Serial.print(Z_MAX_DEG); Serial.print(")");
        Serial.print(" | Grijper(deg): "); Serial.print(g_deg, 1);
        Serial.print(" (clamp "); Serial.print(GRIP_MIN_DEG); Serial.print(".."); Serial.print(GRIP_MAX_DEG); Serial.println(")");
      }
    }
    else if (parseTwoSpeeds(input, v1, v2)) {
      bool ok1 = setVelocity(DXL_ID_1, (int32_t)v1);
      bool ok2 = setVelocity(DXL_ID_2, (int32_t)v2);
      if (ok1 && ok2) {
        Serial.print("Snelheden gezet -> ID1: ");
        Serial.print(v1);
        Serial.print(" | ID2: ");
        Serial.println(v2);
      }
    }
    else {
      Serial.println("Ongeldige invoer.");
      Serial.println("Gebruik: v1,v2  of  v1,v2,h_deg,g_deg   (bv. 30,-30,120,45)");
    }
  }
}
