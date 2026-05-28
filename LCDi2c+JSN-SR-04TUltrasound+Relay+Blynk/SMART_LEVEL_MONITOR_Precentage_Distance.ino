/*************************************************************
  Project: Smart Level Monitor (Version 3.1 - Hybrid Data)
  Features: LCD I2C, 3-Stage Relay/Indicator, Blynk IoT
  Sensor: JSN-SR04T (Range: 20cm - 600cm)
  Data: Displays BOTH Distance (cm) and Percentage (%)
 *************************************************************/

#define BLYNK_TEMPLATE_ID "YOUR_ID_HERE"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME_HERE"
#define BLYNK_AUTH_TOKEN "YOUR_TOKEN_HERE"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>

// --- PIN Configuration ---
#define PIN_RELAY_RED    14  // High Alert (Full)
#define PIN_RELAY_ORANGE 13  // Medium Level
#define PIN_RELAY_GREEN  12  // Low Level (Empty)
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
  // 1. Read JSN-SR04T Ultrasonic Sensor (Standard Clean Pulse)
  digitalWrite(PIN_TRIG, LOW); // Reset signal
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); // Send sound wave
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW); // Stop sending
  
  long duration = pulseIn(PIN_ECHO, HIGH);
  int distance = duration / 58; // Convert duration to cm

  // Jeda pengaman untuk sensor JSN-SR04T (20cm - 600cm)
  int constrainedDistance = constrain(distance, 20, 600);

  // 2. Convert Distance to Percentage
  // 600cm = 0% (Empty), 20cm = 100% (Full)
  int percentage = map(constrainedDistance, 600, 20, 0, 100);
  
  // 3. Control Logic based on Percentage Numerical Value
  if (percentage >= 80) {
    // Level 80% - 100% -> RED Relay Active
    setRelays(HIGH, LOW, LOW);  
    updateBlynkLEDs(255, 0, 0); 
  } 
  else if (percentage >= 30) {
    // Level 30% - 79% -> ORANGE Relay Active
    setRelays(LOW, HIGH, LOW);  
    updateBlynkLEDs(0, 255, 0); 
  } 
  else {
    // Level 0% - 29% -> GREEN Relay Active
    setRelays(LOW, LOW, HIGH);  
    updateBlynkLEDs(0, 0, 255); 
  }

  // 4. Update LCD Display (Menampilkan Keduanya)
  // Baris 0: Jarak ORI dalam cm
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance); // Menggunakan nilai ori sebelum di-constrain biar ketahuan klo diluar limit
  lcd.print("cm      "); 
  
  // Baris 1: Persentase Air %
  lcd.setCursor(0, 1);
  lcd.print("Water: ");
  lcd.print(percentage);
  lcd.print("%        ");

  // 5. Send metrics to Blynk Cloud
  Blynk.virtualWrite(V1, distance);   // V1 = Jarak Asli (cm)
  Blynk.virtualWrite(V2, percentage); // V2 = Hasil Persen (%)
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
  digitalWrite(PIN_RELAY_RED, LOW);
  digitalWrite(PIN_RELAY_ORANGE, LOW);
  digitalWrite(PIN_RELAY_GREEN, LOW);
  delay(100); 
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
  Blynk.run(); 
  timer.run(); 
}
