#include <LiquidCrystal_I2C.h>

#define PIN_RELAY_RED 14
#define PIN_RELAY_ORANGE 13
#define PIN_RELAY_GREEN 12
#define PIN_TRIG 26
#define PIN_ECHO 27
#define I2C_ADDR    0x27
#define LCD_COLUMNS 16
#define LCD_LINES    2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_LINES);

void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_RELAY_RED, OUTPUT);
  pinMode(PIN_RELAY_ORANGE, OUTPUT);
  pinMode(PIN_RELAY_GREEN, OUTPUT);

  // Initialize relays to OFF
  digitalWrite(PIN_RELAY_RED, LOW);
  digitalWrite(PIN_RELAY_ORANGE, LOW);
  digitalWrite(PIN_RELAY_GREEN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.print("Booting Up");
  delay(1000);
  lcd.clear();
}

void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  
  // Calculate distance in cm
  int distance = pulseIn(PIN_ECHO, HIGH) / 58;

  // Display Distance on Row 0
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print("cm   ");

  lcd.setCursor(0, 1);
  // --- Level Logic & Color Relays ---
  
  if (distance <= 100) {
    // LEVEL 100% - RED / BUZZER
    lcd.print("Level: 100%     ");
    digitalWrite(PIN_RELAY_RED, HIGH);
    digitalWrite(PIN_RELAY_ORANGE, LOW);
    digitalWrite(PIN_RELAY_GREEN, LOW);
  } 
  else if (distance <= 300) {
    // LEVEL 50% - 75% - ORANGE
    // (Distance from 101cm to 300cm)
    if (distance <= 200) lcd.print("Level: 75%      ");
    else                 lcd.print("Level: 50%      ");
    
    digitalWrite(PIN_RELAY_RED, LOW);
    digitalWrite(PIN_RELAY_ORANGE, HIGH);
    digitalWrite(PIN_RELAY_GREEN, LOW);
  }
  else {
    // LEVEL 0% - 25% - GREEN
    // (Distance above 300cm)
    if (distance <= 400) lcd.print("Level: 25%      ");
    else                 lcd.print("Level: 0%       ");

    digitalWrite(PIN_RELAY_RED, LOW);
    digitalWrite(PIN_RELAY_ORANGE, LOW);
    digitalWrite(PIN_RELAY_GREEN, HIGH);
  }

  delay(500);
}
