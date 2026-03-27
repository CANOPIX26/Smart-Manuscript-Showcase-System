#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// I2C LCD addresses (Check if they conflict with your TCA9548A if used here)
LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd1(0x25, 16, 2);

#define DHTTYPE DHT11

// Using Pins 6 and 7 for dual DHT redundancy
DHT dht1(6, DHTTYPE);
DHT dht2(7, DHTTYPE);

// Output Peripheral Pins
int buzzer1 = 3;
int buzzer2 = 4;
int humidar = 2;   // Relay for Argon Air Pump (Active-LOW)
int alertLED = 22; // Relay for Red Alert LED (Active-LOW)

// H-Bridge (L298N/BTS7960) for (2) TEC1 Peltier Modules
int ENA2 = 10;   // PWM Speed control for Channel A
int ENB2 = 11;   // PWM Speed control for Channel B
int IN1_2 = 53;  // Direction Control 1
int IN2_2 = 51;  // Direction Control 2
int IN3_2 = 49;  // Direction Control 3
int IN4_2 = 47;  // Direction Control 4

void setup() {
  // Initialize standard outputs
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);
  pinMode(humidar, OUTPUT);
  pinMode(alertLED, OUTPUT);

  // Initialize H-Bridge pins for Peltier Cooling/Heating
  pinMode(ENA2, OUTPUT);
  pinMode(ENB2, OUTPUT);
  pinMode(IN1_2, OUTPUT);
  pinMode(IN2_2, OUTPUT);
  pinMode(IN3_2, OUTPUT);
  pinMode(IN4_2, OUTPUT);

  // Set Peltier modules to full power (255)
  analogWrite(ENA2, 255);
  analogWrite(ENB2, 255);

  // Initialize both LCD screens
  lcd.init(); 
  lcd.backlight();
  lcd1.init(); 
  lcd1.backlight();

  Serial.begin(9600); // Standard Baud Rate for Mega

  dht1.begin();
  dht2.begin();
}

void loop() {
  // Data Acquisition: Reading Temperature and Humidity from both sensors
  float t1 = dht1.readTemperature();
  float h1 = dht1.readHumidity();
  float t2 = dht2.readTemperature();
  float h2 = dht2.readHumidity();

  // Calculating averages for display and logic stabilization
  float avgTemp = (t1 + t2) / 2.0;
  float avgHumd = (h1 + h2) / 2.0;

  bool alert = false;

  // CRITICAL SAFETY LOGIC:
  // Trigger alert if temp < 16°C or > 24°C (Safe preservation range)
  if (t1 < 16 || t1 > 24 || t2 < 16 || t2 > 24) alert = true;
  // Trigger alert if humidity is outside 30-70% range
  if (h1 < 30 || h1 > 70 || h2 < 30 || h2 > 70) alert = true;
  // Trigger alert if there is a 5°C discrepancy between sensors (Sensor failure check)
  if (abs(t1 - t2) > 5) alert = true;

  lcd.clear();
  lcd1.clear();
  
  if (alert) {
    // ALERT STATE: Visual and Audio Warnings
    lcd.setCursor(0, 0); lcd.print("Temperature:"); lcd.print(avgTemp);
    lcd.setCursor(0, 1); lcd.print("ERROR");

    lcd1.setCursor(0, 0); lcd1.print("Humidity:"); lcd1.print(avgHumd);
    lcd1.setCursor(0, 1); lcd1.print("ERROR");

    digitalWrite(alertLED, LOW); // Relay ON (Low Trigger)
    digitalWrite(buzzer1, HIGH);
    digitalWrite(buzzer2, HIGH);
    digitalWrite(humidar, HIGH); // Pump OFF

    // Stop all Peltier operations during critical failure
    digitalWrite(IN1_2, LOW); digitalWrite(IN2_2, LOW);
    digitalWrite(IN3_2, LOW); digitalWrite(IN4_2, LOW);

    delay(300); // Beep/Flash interval

    digitalWrite(alertLED, HIGH); // Relay OFF
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);

    delay(300); 
  } else {
    // NORMAL OPERATION STATE
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(alertLED, HIGH);

    // THERMAL REGULATION LOGIC
    if (avgTemp > 18.5) {
      // COOLING MODE: Active if temp exceeds 18.5°C
      lcd.setCursor(0, 0); lcd.print("Temp:"); lcd.print(avgTemp);
      lcd.setCursor(0, 1); lcd.print("STATUS: COOLING");

      digitalWrite(IN1_2, HIGH); digitalWrite(IN2_2, LOW);
      digitalWrite(IN3_2, LOW);  digitalWrite(IN4_2, LOW);
    } else {
      // HEATING MODE: Active if temp is below 18.5°C
      lcd.setCursor(0, 0); lcd.print("Temp:"); lcd.print(avgTemp);
      lcd.setCursor(0, 1); lcd.print("STATUS: HEATING");

      digitalWrite(IN1_2, LOW);  digitalWrite(IN2_2, LOW);
      digitalWrite(IN3_2, HIGH); digitalWrite(IN4_2, LOW);
    }

    // HUMIDITY REGULATION LOGIC
    if (avgHumd < 30) {
      // PUMPING MODE: Inject humid Argon if air is too dry
      lcd1.setCursor(0, 0); lcd1.print("Humid:"); lcd1.print(avgHumd);
      lcd1.setCursor(0, 1); lcd1.print("STATUS: PUMPING");
      digitalWrite(humidar, LOW); // Relay ON
    } else {
      // STABLE MODE: Target humidity reached
      lcd1.setCursor(0, 0); lcd1.print("Humid:"); lcd1.print(avgHumd);
      lcd1.setCursor(0, 1); lcd1.print("STATUS: STABLE");
      digitalWrite(humidar, HIGH); // Relay OFF
    }
    delay(500); 
  }
}