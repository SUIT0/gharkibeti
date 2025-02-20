#include <Servo.h>

// Define pins for devices
const int bulbPin = 2;    // Digital pin for bulb (connected to relay)
const int fanPin = 4;     // Digital pin for fan
const int lockPin = 5;    // Digital pin for lock (e.g., solenoid lock)
Servo doorServo;          // Servo object for door control

// Variables to store previous device states
int prevBulbState = HIGH; // Start with bulb OFF (HIGH for reverse logic)
int prevDoorState = 0;    // Servo angle for door
int prevFanState = LOW;
int prevLockState = LOW;

void setup() {
  Serial.begin(115200);    // Communication with ESP32

  // Set pin modes
  pinMode(bulbPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(lockPin, OUTPUT);
  doorServo.attach(9);     // Servo connected to pin 9

  // Turn everything off initially
  digitalWrite(bulbPin, HIGH); // Bulb OFF (reverse logic)
  digitalWrite(fanPin, LOW);
  digitalWrite(lockPin, LOW);
  doorServo.write(0);      // Set servo to 0 degrees (closed door)

  Serial.println("Arduino Uno is ready to control devices");
}

void printDeviceStates() {
  Serial.println("Current Device States:");
  Serial.println("Bulb: " + String(digitalRead(bulbPin) ? "OFF" : "ON")); // Inverted logic
  Serial.println("Door: " + String(doorServo.read() == 90 ? "OPEN" : "CLOSED"));
  Serial.println("Fan: " + String(digitalRead(fanPin) ? "ON" : "OFF"));
  Serial.println("Lock: " + String(digitalRead(lockPin) ? "LOCKED" : "UNLOCKED"));
  Serial.println();
}

void loop() {
  // Check for incoming data from ESP32
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Remove any extra whitespace

    // Parse and execute the command
    if (command.startsWith("BULB:")) {
      bool state = command.substring(5).toInt();
      digitalWrite(bulbPin, !state); // Invert the state for reverse logic
      Serial.println("Bulb set to: " + String(state ? "ON" : "OFF"));
    } else if (command.startsWith("DOOR:")) {
      bool state = command.substring(5).toInt();
      int angle = state ? 90 : 0; // 90 degrees if OPEN, 0 degrees if CLOSED
      doorServo.write(angle);
      Serial.println("Door set to angle: " + String(angle));
    } else if (command.startsWith("FAN:")) {
      bool state = command.substring(4).toInt();
      digitalWrite(fanPin, state);
      Serial.println("Fan set to: " + String(state));
    } else if (command.startsWith("LOCK:")) {
      bool state = command.substring(5).toInt();
      digitalWrite(lockPin, state);
      Serial.println("Lock set to: " + String(state));
    }
  }

  // Check for changes in device states
  int currentBulbState = digitalRead(bulbPin);
  int currentDoorState = doorServo.read();
  int currentFanState = digitalRead(fanPin);
  int currentLockState = digitalRead(lockPin);

  // If any state has changed, print the updated status
  if (currentBulbState != prevBulbState || currentDoorState != prevDoorState ||
      currentFanState != prevFanState || currentLockState != prevLockState) {
    printDeviceStates();

    // Update previous states
    prevBulbState = currentBulbState;
    prevDoorState = currentDoorState;
    prevFanState = currentFanState;
    prevLockState = currentLockState;
  }
}