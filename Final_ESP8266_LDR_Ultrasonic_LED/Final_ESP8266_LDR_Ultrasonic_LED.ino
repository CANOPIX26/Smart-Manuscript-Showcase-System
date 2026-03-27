#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Credentials - Network Configuration
const char* ssid = "Sara's Galaxy A34 5G";      // Target WiFi SSID
const char* password = "sara292005";            // WiFi Password

// Pin Definitions for Sensors
const int trigPin = D0;                         // Ultrasonic Trigger
const int echoPin = D1;                         // Ultrasonic Echo
const int ldrPin = A0;                          // Light Dependent Resistor (Analog)

// L298N Motor/LED Driver Pins
const int enA = D2, in1 = D3, in2 = D4;         // Channel A Control
const int enB = D7, in3 = D5, in4 = D6;         // Channel B Control

// Operational Thresholds
const int DARK = 800;                           // High resistance = Dark environment
const int BRIGHT = 200;                         // Low resistance = Bright environment
const int PERSON_DIST = 50;                     // Detection range in cm (50cm)

// Web server instance on standard HTTP port 80
ESP8266WebServer server(80);

// Global variables for sensor telemetry
int ldrValue = 0;
int distance = 0;
bool personDetected = false;
int ledBrightness = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Initialize L298N Output Pins using a loop for efficiency
  int pins[] = {enA, in1, in2, enB, in3, in4};
  for (int i = 0; i < 6; i++) pinMode(pins[i], OUTPUT);
  
  // Set default forward direction for the driver channels
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
  
  // Begin WiFi Connection Sequence
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  // Connection timeout logic (approx 30 seconds)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Open browser and go to: http://" + WiFi.localIP().toString());
  } else {
    Serial.println("\nFailed to connect to WiFi!");
    Serial.println("Check your credentials or try again.");
  }
  
  // Define Web Server Routing
  server.on("/", handleRoot);      // Serves the main Dashboard UI
  server.on("/data", handleData);  // Serves real-time JSON data for the UI
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // 1. Data Acquisition
  ldrValue = analogRead(ldrPin);
  distance = getDistance();
  personDetected = (distance > 0 && distance < PERSON_DIST);
  
  // 2. Control Logic: Adjust LED brightness based on proximity and ambient light
  if (personDetected) {
    // Map LDR values to PWM range (0-255). Constrain ensures we stay in bounds.
    ledBrightness = constrain(map(ldrValue, BRIGHT, DARK, 0, 255), 0, 255);
    analogWrite(enA, ledBrightness);
    analogWrite(enB, ledBrightness);
  } else {
    // Turn off LEDs if no one is detected
    ledBrightness = 0;
    analogWrite(enA, 0);
    analogWrite(enB, 0);
  }
  
  // 3. Serial Debugging Output
  Serial.print("LDR:" + String(ldrValue) + " Dist:" + String(distance) + "cm Person:" + String(personDetected ? "YES" : "NO"));
  Serial.println(" LEDs:" + String(ledBrightness));
  
  // 4. Handle incoming HTTP client requests
  server.handleClient();
  
  delay(300); // Small delay to prevent sensor spamming
}

// Function to calculate distance using the HC-SR04 Ultrasonic sensor
int getDistance() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Pulse timeout set to 30ms (approx 5 meters max)
  long duration = pulseIn(echoPin, HIGH, 30000);
  return (duration > 0) ? duration * 0.034 / 2 : -1;
}

// Main HTML Dashboard Interface
void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body{font-family:Arial;text-align:center;margin:50px;background:#f0f0f0;}";
  html += ".container{background:white;padding:20px;border-radius:10px;box-shadow:0 0 10px gray;}";
  html += "h1{color:#333;}";
  html += ".sensor{margin:20px;padding:15px;border-radius:5px;}";
  html += ".ldr{background:#e3f2fd;}";
  html += ".dist{background:#f1f8e9;}";
  html += ".person{background:#fff3e0;}";
  html += ".led{background:#fce4ec;}";
  html += ".value{font-size:24px;font-weight:bold;color:#1976d2;}";
  html += "</style>";
  
  // AJAX Script for real-time dashboard updates without refreshing page
  html += "<script>";
  html += "function updateData(){";
  html += "fetch('/data').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('ldr').innerHTML = d.ldr;";
  html += "document.getElementById('dist').innerHTML = d.distance + ' cm';";
  html += "document.getElementById('person').innerHTML = d.person ? 'YES' : 'NO';";
  html += "document.getElementById('led').innerHTML = d.brightness + ' / 255';";
  html += "document.getElementById('percent').innerHTML = Math.round(d.brightness/255*100) + '%';";
  html += "document.getElementById('lightCond').innerHTML = getLightCond(d.ldr);";
  html += "});}";
  html += "function getLightCond(v){";
  html += "if(v > " + String(DARK) + ") return 'Dark';";
  html += "if(v < " + String(BRIGHT) + ") return 'Bright';";
  html += "return 'Medium';}";
  html += "setInterval(updateData, 100);"; // Fetch updates every 100ms
  html += "</script>";
  
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h1>ESP8266 Manuscript Telemetry</h1>";
  html += "<p><small>SSID: " + String(ssid) + "</small></p>";
  
  // UI Panels for individual sensors
  html += "<div class='sensor ldr'><h2>Light Level</h2>";
  html += "<div class='value' id='ldr'>" + String(ldrValue) + "</div>";
  html += "<div id='lightCond'>" + getLightCondition(ldrValue) + "</div></div>";
  
  html += "<div class='sensor dist'><h2>Visitor Proximity</h2>";
  html += "<div class='value' id='dist'>" + String(distance) + " cm</div></div>";
  
  html += "<div class='sensor person'><h2>Presence Detected</h2>";
  html += "<div class='value' id='person'>" + String(personDetected ? "YES" : "NO") + "</div></div>";
  
  html += "<div class='sensor led'><h2>LED Output Status</h2>";
  html += "<div class='value' id='led'>" + String(ledBrightness) + " / 255</div>";
  html += "<div id='percent'>" + String(map(ledBrightness, 0, 255, 0, 100)) + "%</div></div>";
  
  html += "</div></body></html>";
  
  server.send(200, "text/html", html);
}

// JSON Endpoint for data fetching (API-style)
void handleData() {
  String json = "{";
  json += "\"ldr\":" + String(ldrValue) + ",";
  json += "\"distance\":" + String(distance) + ",";
  json += "\"person\":" + String(personDetected ? "true" : "false") + ",";
  json += "\"brightness\":" + String(ledBrightness);
  json += "}";
  
  server.send(200, "application/json", json);
}

// Helper function to categorize light levels
String getLightCondition(int value) {
  if (value > DARK) return "Dark";
  if (value < BRIGHT) return "Bright";
  return "Medium";
}
