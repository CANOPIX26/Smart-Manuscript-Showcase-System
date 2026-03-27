# Smart Manuscript Showcase Control System

An advanced, multi-layered environmental monitoring and preservation system designed for high-value historical manuscripts. This project utilizes a triple-circuit architecture to maintain a stable micro-climate, manage interactive lighting, and ensure atmospheric integrity.

## 🚀 System Architecture

The system is divided into three specialized control units, all programmed in **C++** via the **Arduino IDE**:

### 1. Climate Control (Thermal & Humidity)
Monitors and regulates the internal environment to prevent organic material degradation.
* **Hardware:** DHT Sensors, TEC1 Peltier Modules, Humid Argon Pump.
* **Function:** Real-time PID-style stabilization of temperature. Activates the Argon pump if humidity levels deviate from the preservation baseline.

### 2. Interactive Lighting & Telemetry (UX & ESP8266)
Manages viewer interaction while protecting the artifact from photo-oxidation.
* **Hardware:** Ultrasonic Sensor, LDR (Light Dependent Resistor), ESP8266 WiFi Module.
* **Function:** Detects viewer proximity to trigger "soft-start" LED lighting. The LDR ensures light intensity stays within safe lux limits. The ESP8266 serves as the gateway to the `canopix26.vercel.app` dashboard.

### 3. Atmospheric Integrity (Pressure & Gas)
Maintains the "Low-Pressure Vacuum Encasement" and monitors for chemical reactivity.
* **Hardware:** BMP280 Pressure Sensor, MQ-series Gas Sensors, Arduino Uno.
* **Function:** Detects pressure leaks in the vacuum seal and monitors for oxygen or pollutants. Automatically flushes the chamber with inert Argon if a breach is detected.

## 📁 Repository Structure
* `/Circuits`: Contains Fritzing wiring diagrams and schematics.
* `/Firmware`: C++ source code for Arduino and ESP8266.
* `/Docs`: Technical research on high-density micro-climate buffering.

## 🛠 Future Development
* **API Integration:** Finalizing the REST/MQTT API for real-time data streaming to the Vercel web interface.
* **Refined Encasement:** Optimizing the high-density "Micro-Climate" buffering logic.

## 📜 License
[Insert your preferred license, e.g., MIT]
