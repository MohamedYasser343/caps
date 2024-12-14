#include <SoftwareSerial.h>

// Define the RX and TX pins for communication with NodeMCU
SoftwareSerial NodeMCUSerial(1, 0);  // RX, TX (change pins based on your setup)

// MQ-135 Sensor
const int mq135Pin = A1;  // MQ-135 sensor connected to A1 pin
const float R0 = 10.0; // Base resistance of MQ-135 in clean air (adjust based on calibration)

// MAX9814 Microphone Sensor
const int micPin = A0;  // MAX9814 OUT pin connected to A0
float dB_offset = 50;  // Adding a fixed offset to shift the values

// Function to calculate resistance for MQ-135
float calculateResistance(int raw_adc) {
  float RL = 1.0;  // Load resistance on the board, 1kΩ for MQ-135
  return (1023.0 / (float)raw_adc - 1.0) * RL;
}

void setup() {
  // Start communication with NodeMCU at 9600 baud
  NodeMCUSerial.begin(9600);
  
  // Start the serial communication with the computer for debugging
  Serial.begin(9600);
}

void loop() {
  // --- MQ-135 Sensor Reading ---
  // Read the analog value from the MQ-135 sensor
  int mq135Value = analogRead(mq135Pin);
  
  // Convert analog value to voltage (assuming 5V reference for Arduino)
  float mq135Voltage = mq135Value * (5.0 / 1023.0);
  
  // Calculate gas concentrations in PPM (simplified approximation)
  float resistance = calculateResistance(mq135Value);

  // Using arbitrary calibration constants (you can adjust these)
  float smoke_ppm = (resistance / R0) * 200;  // Smoke concentration
  float alcohol_ppm = (resistance / R0) * 300; // Alcohol concentration
  float benzene_ppm = (resistance / R0) * 400; // Benzene concentration

  // --- MAX9814 Microphone Reading ---
  // Read the analog value from the microphone
  int micValue = analogRead(micPin);

  // Convert analog reading to voltage (5V reference for Arduino)
  float micVoltage = (micValue * 5.0) / 1023.0;
  
  // Estimate dB from voltage using reference voltage 5V
  float dB = (micVoltage > 0) ? 20 * log10(micVoltage / 5.0) + dB_offset : -100;

  // --- Print to Serial Monitor (for debugging) ---
  // MQ-135 Data
  Serial.print(" Air Quality Voltage: ");
  Serial.print(mq135Voltage);
  Serial.println(" V"); 

  Serial.print("  Smoke: ");
  Serial.print(smoke_ppm);
  Serial.println(" ppm");

  Serial.print("  Alcohol: ");
  Serial.print(alcohol_ppm);
  Serial.println(" ppm");

  Serial.print("  Benzene: ");
  Serial.print(benzene_ppm);
  Serial.println(" ppm");

  // MAX9814 Data on a Single Line
  Serial.print("Noise: ");
  Serial.print(dB);
  Serial.print(" dB, Voltage: ");
  Serial.print(micVoltage);
  Serial.println(" V");
  
  // --- Send Data to NodeMCU ---
  // MQ-135 Data
  NodeMCUSerial.print("  Air Quality Voltage: ");
  NodeMCUSerial.print(mq135Voltage);
  NodeMCUSerial.println(" V");

  NodeMCUSerial.print("Smoke: ");
  NodeMCUSerial.print(smoke_ppm);
  NodeMCUSerial.println(" ppm");

  NodeMCUSerial.print("Alcohol: ");
  NodeMCUSerial.print(alcohol_ppm);
  NodeMCUSerial.println(" ppm");

  NodeMCUSerial.print("Benzene: ");
  NodeMCUSerial.print(benzene_ppm);
  NodeMCUSerial.println(" ppm");

  // MAX9814 Data on a Single Line
  NodeMCUSerial.print("  Noise: ");
  NodeMCUSerial.print(dB);
  NodeMCUSerial.print(" dB, Voltage: ");
  NodeMCUSerial.print(micVoltage);
  NodeMCUSerial.println(" V");

  // Delay before taking the next reading
  delay(2000);
}
