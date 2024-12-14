#include <Arduino.h>
#include <SoftwareSerial.h>

#define MQPIN A0  // MQ sensor pin


#define HWPIN A1  // Heart rate sensor pin

#define MAXPIN A2  // Microphone (MAX9814) pin
const float vRef = 5.0;  // Voltage reference for analogRead

#define RXPIN 2  // ESP8266 RX pin
#define TXPIN 3  // ESP8266 TX pin
SoftwareSerial espSerial(RXPIN, TXPIN);

const int sampleWindow = 50;  // Sample window for dB calculation (ms)
unsigned int sample;



void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  espSerial.begin(9600);

  pinMode(MQPIN, INPUT);
  pinMode(HWPIN, INPUT);
  pinMode(MAXPIN, INPUT);

  Serial.println("Arduino Decibel Meter and Sensor Node Initialized");
}

void loop() {
  // *Step 1: Sound Level (dB) Calculation*
  unsigned long startMillis = millis();
  float peakToPeak = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  while (millis() - startMillis < sampleWindow) {
    sample = analogRead(MAXPIN);
    if (sample < 1024) {
      if (sample > signalMax) {
        signalMax = sample;
      } else if (sample < signalMin) {
        signalMin = sample;
      }
    }
  }

  peakToPeak = signalMax - signalMin;
  float dB = map(peakToPeak, 20, 900, 49.5, 90);

  // *Step 2: Smoke Sensor Calculation*
  int mqValue = analogRead(MQPIN);
  int m = map(mqValue,0,1023,0,300);

  // *Step 3: Heart Rate Sensor*
  int heartRate = analogRead(HWPIN);
  int h = map(heartRate,0,1023,0,160);

  // *Step 4: Data Transmission*
  String data = "SmokePPM:" + String(m) + 
                ", HeartRate:" + String(h) + 
                ", Noise:" + String(dB, 1) + "\n";
  espSerial.print(data);
  Serial.println(data);

  delay(5000);  // Delay to control update frequency
}
