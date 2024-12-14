void setup() {
  // Start the serial communication with Arduino at 9600 baud
  Serial.begin(9600);

  // Start serial communication with the computer for debugging
  Serial.setTimeout(2000);  // Adjust this timeout if necessary
}

void loop() {
  // If data is available from Arduino, read and print it on the Serial Monitor
  if (Serial.available()) {
    String messageFromArduino = Serial.readString();
    Serial.print("Message from Arduino: ");
    Serial.println(messageFromArduino);
  }

  // Send a message to Arduino (optional)
  Serial.println("Received Data from Arduino");
  delay(2000);
}
