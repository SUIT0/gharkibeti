#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h" // Provides the token generation process info
#include "addons/RTDBHelper.h"  // Provides the RTDB payload printing info

// Replace with your network credentials
const char* ssid = "Sudip wifi";
const char* password = "9840255425Abc";

// Replace with your Firebase project details
const char* apiKey = "AIzaSyBoEyPvpcPOpVOKMNSSvLBJlQe4pxHVl5c";
const char* databaseURL = "https://arduinotofirebase-93989-default-rtdb.firebaseio.com/";

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth; // FirebaseAuth object is still required
FirebaseConfig config;

void setup() {
  Serial.begin(115200); // Start serial communication with Arduino Uno

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to Wi-Fi");

  // Initialize Firebase
  config.api_key = apiKey;
  config.database_url = databaseURL;

  // Initialize Firebase (both config and auth are required)
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase initialized");
}

void loop() {
  // Check Wi-Fi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000); // Wait for reconnection
  }

  // Check if data is received from the Arduino Uno
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n'); // Read data from Arduino
    data.trim(); // Remove any extra whitespace
    Serial.println("Received data from Arduino: " + data); // Debug statement

    // Send data to Firebase
    if (Firebase.ready()) {
      if (Firebase.RTDB.setString(&fbdo, "/sensorData", data)) {
        Serial.println("Data sent to Firebase: " + data);
      } else {
        Serial.println("Failed to send data to Firebase: " + fbdo.errorReason());
      }
    }
  }
}