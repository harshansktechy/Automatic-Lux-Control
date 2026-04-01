// =========================================================
// Constant-Lux PID + Cumulative Energy + ON/OFF Status
// ESP32 Arduino Core 3.0+
// =========================================================

// --- 1. BLYNK CREDENTIALS ---
#define BLYNK_TEMPLATE_ID "TMPL33JSD07UW" // Paste your new Template ID here!
#define BLYNK_TEMPLATE_NAME "LUX MONITOR"
#define BLYNK_AUTH_TOKEN "PglOISQs7oG5xKuQecAH58IJ1fnLE_3o"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// --- 2. WIFI CREDENTIALS ---
char ssid[] = "SK F41";
char pass[] = "16171819";

// --- 3. HARDWARE & PID VARIABLES ---
const int ldrPin = 34;
const int mosfetPin = 15;
const int pwmFreq = 1000;
const int pwmResolution = 8;

int targetLight = 2800;
int currentLight = 0;

// Cumulative Energy Trackers
double cumulativeMaxEnergy = 0;
double cumulativeSavedEnergy = 0;

float Kp = 0.25;
float Ki = 0.0;
float Kd = 0.0;
float integral = 0;
float previousError = 0;
int lastPWM = 0;
int maxStep = 5;

BlynkTimer timer;

// --- 4. BLYNK APP CONTROL (Slider on V0) ---
BLYNK_WRITE(V0) {
  targetLight = param.asInt();
  Serial.print("Target Updated via Blynk: ");
  Serial.println(targetLight);
}

// --- 5. SEND DATA TO CLOUD (Runs every 1 second) ---
void sendSensorData() {
  // 1. Calculate Cumulative Energy
  cumulativeMaxEnergy += 255.0;
  cumulativeSavedEnergy += (255.0 - lastPWM);
  float totalEnergySavedPercent = (cumulativeSavedEnergy / cumulativeMaxEnergy) * 100.0;

  // 2. Determine LED ON/OFF Status
  String ledStatus = "OFF";
  if (lastPWM > 0) {
    ledStatus = "ON";
  }

  // 3. Send all data to Virtual Pins
  Blynk.virtualWrite(V1, currentLight);
  Blynk.virtualWrite(V2, totalEnergySavedPercent);
  Blynk.virtualWrite(V0, targetLight);
  Blynk.virtualWrite(V3, ledStatus); // Sends "ON" or "OFF" text
}

void setup() {
  Serial.begin(115200);
  
  analogSetPinAttenuation(ldrPin, ADC_11db);
  ledcAttach(mosfetPin, pwmFreq, pwmResolution);
  
  Serial.println("Connecting to WiFi and Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  timer.setInterval(1000L, sendSensorData);
  Serial.println("System Online!");
}

void loop() {
  Blynk.run();
  timer.run();

  // -------- 1. Sensor Averaging --------
  int sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(ldrPin);
  }
  currentLight = sum / 10;

  // -------- 2. Error Calculation --------
  float error = currentLight - targetLight;

  // -------- 3. Proper Deadband Freeze --------
  if (abs(error) < 20) {
    ledcWrite(mosfetPin, lastPWM);
    delay(80);
    return;
  }

  // -------- 4. PID --------
  integral += error;
  float derivative = error - previousError;
  float output = (Kp * error) + (Ki * integral) + (Kd * derivative);
  previousError = error;

  // -------- 5. Clamp Output --------
  if (output > 255) output = 255;
  if (output < 0) output = 0;
  
  int pwmOutput = (int)output;

  // -------- 6. Rate Limiting --------
  if (pwmOutput > lastPWM + maxStep) {
    pwmOutput = lastPWM + maxStep;
  } else if (pwmOutput < lastPWM - maxStep) {
    pwmOutput = lastPWM - maxStep;
  }
  
  lastPWM = pwmOutput;

  // -------- 7. Apply PWM --------
  ledcWrite(mosfetPin, pwmOutput);
  delay(80);
}
