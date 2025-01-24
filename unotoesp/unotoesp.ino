#include <Servo.h>

// Define pins for devices
const int fanPin = 2;    // Digital pin for fan
const int lightPin = 3;  // Digital pin for light
Servo servo;             // Servo object

void setup() {
  Serial.begin(115200);    // Communication with ESP32
  pinMode(fanPin, OUTPUT);
  pinMode(lightPin, OUTPUT);
  servo.attach(9);       // Servo connected to pin 9

  // Turn everything off initially
  digitalWrite(fanPin, LOW);
  digitalWrite(lightPin, LOW);
  servo.write(0); // Set servo to 0 degrees

  Serial.println("Arduino Uno is ready to control devices");
}

void loop() {
  // Check for incoming data from ESP32
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any extra whitespace

    // Parse and execute the command
    if (command.startsWith("FAN:")) {
      bool state = command.substring(4).toInt();
      digitalWrite(fanPin, state);
      Serial.println("Fan set to: " + String(state));
    } else if (command.startsWith("LIGHT:")) {
      bool state = command.substring(6).toInt();
      digitalWrite(lightPin, state);
      Serial.println("Light set to: " + String(state));
    } else if (command.startsWith("SERVO:")) {
      bool state = command.substring(6).toInt();
      int angle = state ? 90 : 0; // 90 degrees if ON, 0 degrees if OFF
      servo.write(angle);
      Serial.println("Servo set to angle: " + String(angle));
    }
  }
}
