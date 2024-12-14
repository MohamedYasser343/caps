#define BLYNK_TEMPLATE_ID "TMPL2YJ5IzCn9"
#define BLYNK_TEMPLATE_NAME "Cap"
#define BLYNK_AUTH_TOKEN "6sY5vCN9MABhBLXk_qS6_voBKicQS9uu"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h> 
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Cap";
char pass[] = "cap12315";
char webAppUrl[] = "https://script.google.com/macros/s/AKfycbymgGpcFidi1Z0t-GEfUgmA_Bzrq6kC2feDR3BQaR-IpeQTq3e7RQ7ytDyxkNDoxQfw/exec";

String getValue(String data, String key) {
  int startIndex = data.indexOf(key) + key.length();
  int endIndex = data.indexOf(",", startIndex);
  if (endIndex == -1) {
    endIndex = data.length();
  }
  return data.substring(startIndex, endIndex);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
}

void loop() {
  HTTPClient http;
  http.begin(webAppUrl);
  http.addHeader("Content-Type", "application/json");

  Blynk.run();

  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    String SsmokePPM = getValue(data, "SmokePPM:");
    String SheartRate = getValue(data, "HeartRate:");
    String Snoise = getValue(data, "Noise:");

    String jsonData = "{";
    jsonData += "\"smokePPM\": " + String(SsmokePPM) + ",";
    jsonData += "\"heatRate\": " + String(SheartRate) + ",";
    jsonData += "\"noise\": " + String(Snoise);
    jsonData += "}";

    int httpResponseCode = http.POST(jsonData);
    http.end();

    int smokePPM = SsmokePPM.toInt();
    int heartRate = SheartRate.toInt();
    int noise = Snoise.toInt();

    Blynk.virtualWrite(V0, smokePPM);
    Blynk.virtualWrite(V1, heartRate);
    Blynk.virtualWrite(V2, noise);

    if (smokePPM > 50) {
      Blynk.logEvent("smoke", "Smoke level exceeded 50 ppm");
    }
    if (noise > 70) {
      Blynk.logEvent("noise", "Noise level exceeded 70 dB");
    }
    if (heartRate > 120) {
      Blynk.logEvent("heart", "heart beat exceeded 120 ppm");
    }
  }
  delay(5000);
}