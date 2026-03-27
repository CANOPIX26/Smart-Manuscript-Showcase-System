#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

// Gas sensors
#define GAS1 A0
#define GAS2 A1

// DC-Fan
#define fanpin1 4

// Motor pins
#define ENA 6
#define ENB 5
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// Thresholds
#define GAS_THRESHOLD 90      // Gas sensor threshold
#define PRESSURE_DELTA 2000   // hPa difference to trigger pump

// I2C multiplexer address
#define TCA_ADDR 0x70

Adafruit_BMP280 bmp1;
Adafruit_BMP280 bmp2;
LiquidCrystal_I2C lcd(0x26, 16, 2);

// Normal room pressure (set at startup)
// float NORMAL_P1 = 1013;
// float NORMAL_P2 = 1013;

// Select channel on TCA9548A I2C multiplexer
void tcaSelect(uint8_t channel) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("hello");
  
  pinMode(fanpin1, OUTPUT);
  
  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, 200);
  analogWrite(ENB, 200);

  // Initialize BMP280 sensors
  tcaSelect(0);
  bmp1.begin(0x76);
  tcaSelect(1);
  bmp2.begin(0x76);

  // Initialize LCD
  tcaSelect(2);
  lcd.init();
  lcd.backlight();

  // Capture normal pressure at startup
  // tcaSelect(0);
  // P1 = bmp1.readPressure() / 100.0;
  // tcaSelect(1);
  // P2 = bmp2.readPressure() / 100.0;
}

void loop() {
  // digitalWrite(fanpin1,LOW);
  // turn on the fan

  // Read gas sensors
  int gas1 = analogRead(GAS1);
  int gas2 = analogRead(GAS2);
  float AvgGas = (gas1 + gas2) / 2.0;

  // Read pressure sensors
  tcaSelect(0);
  float p1 = bmp1.readPressure() / 100.0; // hPa
  tcaSelect(1);
  float p2 = bmp2.readPressure() / 100.0; // hPa
  float AvgPress = (p1 + p2) / 2.0;

  // Condition 1: gas above threshold
  bool gasHigh = (AvgGas > GAS_THRESHOLD);

  // Condition 2: pressure deviation above threshold
  bool pressureChange = (AvgPress > PRESSURE_DELTA || p1 > PRESSURE_DELTA || p2 > PRESSURE_DELTA);

  // Pumping if **either** condition is true
  bool pumping = (gasHigh || pressureChange);
  
  Serial.print(AvgGas);
  Serial.print(AvgPress);

  // Control motors
  if (AvgGas > GAS_THRESHOLD) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(fanpin1, HIGH);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(fanpin1, LOW);
  }

  if (AvgPress > PRESSURE_DELTA || p1 > PRESSURE_DELTA || p2 > PRESSURE_DELTA) {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    // digitalWrite(fanpin1,LOW);
  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    // digitalWrite(fanpin1,LOW);
  }

  // Display info on LCD
  tcaSelect(2);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("P:");
  lcd.print((int)AvgPress);
  lcd.print("  G:");
  lcd.print((int)AvgGas);
  
  Serial.println(AvgGas);
  Serial.println(AvgPress);

  // bool pumping = (gasHigh || pressureChange);

  // Add pumping state next to pressure if needed
  lcd.setCursor(0, 1);
  if (pumping) lcd.print(" Pump:ON");
  else lcd.print(" Pump:OFF");

  delay(1000);
}