/*************************************************************
  Project: Smart Level Monitor (Red Alarm Special Interval)
  Logic: RED Alarm runs for 5 mins every 1 hour if distance <= 100
 *************************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_ID_HERE"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "YOUR_TOKEN_HERE"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// --- PIN Configuration ---
#define PIN_RELAY_RED    14
#define PIN_RELAY_ORANGE 13
#define PIN_RELAY_GREEN  12
#define PIN_TRIG         26
#define PIN_ECHO         27

LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASS";

BlynkTimer timer;

// --- Special Alarm Variables ---
bool alarmReady = true;   // Ready to trigger 5-min alarm
bool alarmActive = false;  // Currently in the 5-min ON period

// Callback to turn OFF the alarm after 5 minutes
void stopAlarm() {
  alarmActive = false;
  Serial.println("5-minute alarm finished. Cooling down for 1 hour...");
}

// Callback to make the alarm ready again after 1 hour
void resetAlarmReady() {
  alarmReady = true;
  Serial.println("1 hour cooldown finished. Alarm is ready again.");
}

void setRelays(int red, int orange, int green) {
  digitalWrite(PIN_RELAY_RED, red);
  digitalWrite(PIN_RELAY_ORANGE, orange);
  digitalWrite(PIN_RELAY_GREEN, green);
}

void updateBlynkLEDs(int v3, int v4, int v5) {
  Blynk.virtualWrite(V3, v3);
  Blynk.virtualWrite(V4, v4);
  Blynk.virtualWrite(V5, v5);
}

void updateSystem() {
  // 1. Read Sensor
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  long duration = pulseIn(PIN_ECHO, HIGH);
  int distance = duration / 58;

  String levelStr = "";

  // 2. Logic Control
  if (distance <= 100) {
    levelStr = "Level: 100% ALERT";

    // SPECIAL RED LOGIC
    if (alarmReady) {
      alarmActive = true;
      alarmReady = false; // Lock it!
      
      // Schedule: Stop alarm in 5 minutes (300,000 ms)
      timer.setTimeout(300000L, stopAlarm);
      
      // Schedule: Ready again in 1 hour (3,600,000 ms)
      timer.setTimeout(3600000L, resetAlarmReady);
    }

    // Execute Red based on alarmActive status
    if (alarmActive) {
      setRelays(HIGH, LOW, LOW); 
      updateBlynkLEDs(255, 0, 0);
    } else {
      setRelays(LOW, LOW, LOW); // Silent period within the 1 hour
      updateBlynkLEDs(0, 0, 0);
    }
  } 
  else {
    // If distance is safe (> 100), we reset the alarm so it's ready for next time
    alarmReady = true; 
    alarmActive = false;

    if (distance <= 300) {
      levelStr = (distance <= 200) ? "Level: 75%" : "Level: 50%";
      setRelays(LOW, HIGH, LOW);  // Orange ON (Normal/Infinite)
      updateBlynkLEDs(0, 255, 0);
    } 
    else {
      levelStr = (distance <= 400) ? "Level: 25%" : "Level: 0%";
      setRelays(LOW, LOW, HIGH);  // Green ON (Normal/Infinite)
      updateBlynkLEDs(0, 0, 255);
    }
  }

  // 3. Update Display
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print("cm      "); 
  
  lcd.setCursor(0, 1);
  lcd.print(levelStr);
  lcd.print("      ");

  Blynk.virtualWrite(V1, distance);
  Blynk.virtualWrite(V2, levelStr);
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);
  pinMode(PIN_RELAY_ORANGE, OUTPUT);
  pinMode(PIN_RELAY_GREEN, OUTPUT);

  // Safety Lock
  digitalWrite(PIN_RELAY_RED, LOW);
  digitalWrite(PIN_RELAY_ORANGE, LOW);
  digitalWrite(PIN_RELAY_GREEN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.print("System Booting");

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(500L, updateSystem);
  lcd.clear();
}

void loop() {
  Blynk.run();
  timer.run();
}
