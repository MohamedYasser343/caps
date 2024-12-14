#define BLYNK_TEMPLATE_ID "TMPL2ypdK00mv"
#define BLYNK_TEMPLATE_NAME "ACap"
#define BLYNK_AUTH_TOKEN "w4JaSk4R4lTqafHNowYfURtUw9FQD8Jd"

#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>

#define MQ135PIN 35
#define MQ2PIN 34
#define BUZZERPIN 32
#define LEDPIN 33
const float MQR = 10.0;
#define DHTPIN 13
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

char ssid[] = "capstone";
char pass[] = "cap12345";
char webAppUrl[] = "https://script.google.com/macros/s/AKfycbzOswrfE5VXyH4mtHpjUq9IyciHdcrV07tfj1xIbCo5Nbh8Z2G0KbkTh_D17V6YPyc/exec";

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to Wifi and Blynk cloud");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  Serial.println("Connected to Wifi and Blynk cloud");

  pinMode(MQ135PIN, INPUT);
  pinMode(MQ2PIN, INPUT);
  pinMode(BUZZERPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  dht.begin();
}


void loop() {
  HTTPClient http;
  http.begin(webAppUrl);
  http.addHeader("Content-Type", "application/json");
  Blynk.run();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int mq135Value = analogRead(MQ135PIN);
  int mq135 = map(mq135Value, 0, 1023, 0, 500);
  int mq2Value = analogRead(MQ2PIN);
  int mq2 = map(mq2Value, 0, 1023, 200, 3000);

  String jsonData = "{";
  jsonData += "\"temp\": " + String(t) + ",";
  jsonData += "\"hum\": " + String(h) + ",";
  jsonData += "\"ammonia\": " + String(mq135) + ",";
  jsonData += "\"smoke\": " + String(mq2);
  jsonData += "}";

  int httpResponseCode = http.POST(jsonData);
  http.end();

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, mq135);
  Blynk.virtualWrite(V3, mq2);

  Serial.printf("Humidity: %.2f %%\n", h);
  Serial.printf("Temperature: %.2f °C\n", t);
  Serial.printf("ammonia gas: %d ppm\n", mq135);
  Serial.printf("smoke: %d ppm\n", mq2);

  if (t > 30) {
    Blynk.logEvent("temperature", "Warning: Temperature is higher than 30");
  }

  if (h > 50) {
    Blynk.logEvent("humidity", "Warning: Humidity is higher than 50");
  }

  if (mq135 > 300) {
    digitalWrite(BUZZERPIN, HIGH);
    Blynk.logEvent("ammonia", "Warning: ammonia gas concentration is higher than 300");
    delay(1000);
    digitalWrite(BUZZERPIN, LOW);
  }

  if (mq2 > 1500) {
    digitalWrite(LEDPIN, HIGH);
    Blynk.logEvent("smoke", "Warning: smoke concentration is higher than 1500");
    delay(1000);
    digitalWrite(LEDPIN, LOW);
  }

  delay(1000);
}
