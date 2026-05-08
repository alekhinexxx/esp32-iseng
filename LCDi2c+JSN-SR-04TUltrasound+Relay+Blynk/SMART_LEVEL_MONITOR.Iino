/*************************************************************
  Project: Smart Level Monitor
  Features: LCD I2C, 3-Stage Relay/Indicator to LED, Blynk IoT
 *************************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_ID_HERE"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "YOUR_TOKEN_HERE"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// --- PIN Configuration ---
#define PIN_RELAY_RED    14  // Level 100% (Alert/Buzzer)
#define PIN_RELAY_YELLOW 13  // Level 50% - 75%
#define PIN_RELAY_GREEN  12  // Level 0% - 25%
#define PIN_TRIG         26
#define PIN_ECHO         27

// --- LCD Configuration ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- WiFi Configuration ---
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "YOUR_WIFI_NAME";
char pass[] = "YOUR_WIFI_PASS";

BlynkTimer timer;

// Function to read sensor and update system status
void updateSystem() {
  // 1. Read Ultrasonic Sensor (Standard Clean Pulse)
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  long duration = pulseIn(PIN_ECHO, HIGH);
  int distance = duration / 58; // Convert to cm

  // 2. Variable to hold the display string
  String levelStr = "";
  
  // 3. Main Logic (Level, Relays, & Blynk LEDs)
  if (distance <= 100) {
    levelStr = "Level: 100%";
    setRelays(HIGH, LOW, LOW);  // RED Relay ON
    updateBlynkLEDs(255, 0, 0); // Blynk Red Widget ON
  } 
  else if (distance <= 300) {
    levelStr = (distance <= 200) ? "Level: 75%" : "Level: 50%";
    setRelays(LOW, HIGH, LOW);  // YELLOW Relay ON
    updateBlynkLEDs(0, 255, 0); // Blynk Yellow Widget ON
  } 
  else {
    levelStr = (distance <= 400) ? "Level: 25%" : "Level: 0%";
    setRelays(LOW, LOW, HIGH);  // GREEN Relay ON
    updateBlynkLEDs(0, 0, 255); // Blynk Green Widget ON
  }

  // 4. Update LCD Display
  // Row 0: Real-time Distance
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print("cm      "); 
  
  // Row 1: Percentage Status
  lcd.setCursor(0, 1);
  lcd.print(levelStr);
  lcd.print("      ");

  // 5. Send Data to Blynk Cloud
  Blynk.virtualWrite(V1, distance);
  Blynk.virtualWrite(V2, levelStr);
}

// Helper: Simplifies physical relay control
void setRelays(int red, int yellow, int green) {
  digitalWrite(PIN_RELAY_RED, red);
  digitalWrite(PIN_RELAY_YELLOW, yellow);
  digitalWrite(PIN_RELAY_GREEN, green);
}

// Helper: Simplifies Blynk App LED updates
void updateBlynkLEDs(int v3, int v4, int v5) {
  Blynk.virtualWrite(V3, v3); // Virtual Pin for Red LED
  Blynk.virtualWrite(V4, v4); // Virtual Pin for Yellow LED
  Blynk.virtualWrite(V5, v5); // Virtual Pin for Green LED
}

void setup() {
  Serial.begin(115200);

  // Pin Mode Setup
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);
  pinMode(PIN_RELAY_YELLOW, OUTPUT);
  pinMode(PIN_RELAY_GREEN, OUTPUT);

  // LCD Initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Booting");

  // Blynk Connection
  Blynk.begin(auth, ssid, pass);
  
  // Set Timer Interval (500ms)
  timer.setInterval(500L, updateSystem);
  
  lcd.clear();
}

void loop() {
  Blynk.run(); // Keeps Blynk connection alive
  timer.run(); // Checks the 500ms schedule
}
