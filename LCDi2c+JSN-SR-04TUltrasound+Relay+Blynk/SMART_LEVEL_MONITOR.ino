/*************************************************************
  Project: Smart Level Monitor (Version 2.0 - Production Ready)
  Features: LCD I2C, 3-Stage Relay/Indicator, Blynk IoT
  Safety: Includes Initial Hardware Lock (Safe State)
 *************************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_ID_HERE"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "YOUR_TOKEN_HERE"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// --- PIN Configuration ---
#define PIN_RELAY_RED    14  // Level 100% 
#define PIN_RELAY_ORANGE 13  // Level 50% - 75%
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

// --- Helper Functions ---

// Function to control all physical relays at once
void setRelays(int red, int orange, int green) {
  digitalWrite(PIN_RELAY_RED, red);
  digitalWrite(PIN_RELAY_ORANGE, orange);
  digitalWrite(PIN_RELAY_GREEN, green);
}

// Function to update LED widgets in the Blynk App
void updateBlynkLEDs(int v3, int v4, int v5) {
  Blynk.virtualWrite(V3, v3); // Red LED Widget
  Blynk.virtualWrite(V4, v4); // Orange LED Widget
  Blynk.virtualWrite(V5, v5); // Green LED Widget
}

// --- Main Logic Function ---
void updateSystem() {
  // 1. Read Ultrasonic Sensor (Standard Clean Pulse)
  digitalWrite(PIN_TRIG, LOW); // Reset signal
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); // Send sound wave
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW); // Stop sending
  
  long duration = pulseIn(PIN_ECHO, HIGH);
  int distance = duration / 58; // Convert duration to cm

  String levelStr = "";
  
  // 2. Control Logic based on Distance
  if (distance <= 100) {
    levelStr = "Level: 100%";
    setRelays(HIGH, LOW, LOW);  // Physical Red ON
    updateBlynkLEDs(255, 0, 0); // Blynk Red ON
  } 
  else if (distance <= 300) {
    levelStr = (distance <= 200) ? "Level: 75%" : "Level: 50%";
    setRelays(LOW, HIGH, LOW);  // Physical Orange ON
    updateBlynkLEDs(0, 255, 0); // Blynk Orange ON
  } 
  else {
    levelStr = (distance <= 400) ? "Level: 25%" : "Level: 0%";
    setRelays(LOW, LOW, HIGH);  // Physical Green ON
    updateBlynkLEDs(0, 0, 255); // Blynk Green ON
  }

  // 3. Update LCD Display
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print("cm      "); 
  
  lcd.setCursor(0, 1);
  lcd.print(levelStr);
  lcd.print("      ");

  // 4. Send metrics to Blynk Cloud
  Blynk.virtualWrite(V1, distance);
  Blynk.virtualWrite(V2, levelStr);
}

// --- Standard Arduino Setup ---
void setup() {
  Serial.begin(115200);

  // Initialize Pin Modes
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);
  pinMode(PIN_RELAY_ORANGE, OUTPUT);
  pinMode(PIN_RELAY_GREEN, OUTPUT);

  // --- SAFETY LOCK ---
  // Force all relays to stay OFF during the boot process
  digitalWrite(PIN_RELAY_RED, LOW);
  digitalWrite(PIN_RELAY_ORANGE, LOW);
  digitalWrite(PIN_RELAY_GREEN, LOW);
  delay(100); // Allow electrical stability
  // -------------------

  // LCD Initialization
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Booting");

  // Establish Blynk/WiFi Connection
  Blynk.begin(auth, ssid, pass);
  
  // Set Timer to run updateSystem every 500ms
  timer.setInterval(500L, updateSystem);
  
  lcd.clear();
}

// --- Main Program Loop ---
void loop() {
  Blynk.run(); // Maintains Cloud connection
  timer.run(); // Keeps the timing accurate
}
