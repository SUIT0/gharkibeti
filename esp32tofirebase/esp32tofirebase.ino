#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "Sudip wifi"
#define WIFI_PASSWORD "9840255425Abc"
#define FIREBASE_URL "https://fireship-5c979-default-rtdb.firebaseio.com/.json"
#define API_KEY "AIzaSyD_mPzeWeaIvDaRwuw9TmJaaKsBv5B9XWI"

#define TX_PIN 17 // Define TX pin for Serial1
#define RX_PIN 16 // Define RX pin for Serial1

void setup() {
  Serial.begin(115200); // For debugging
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN); // Initialize Serial1 for communication with Arduino

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Reconnected to Wi-Fi");
  }
}

void loop() {
  reconnectWiFi(); // Ensure Wi-Fi is connected

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the full URL with the API key
    String url = String(FIREBASE_URL) + "?auth=" + API_KEY;

    // Send HTTP GET request
    http.begin(url);
    http.setTimeout(5000); // Set timeout to 5 seconds

    int retryCount = 3; // Number of retries
    int httpCode = -1;

    for (int i = 0; i < retryCount; i++) {
      httpCode = http.GET();
      if (httpCode == HTTP_CODE_OK) {
        break; // Exit the loop if the request succeeds
      }
      Serial.println("HTTP request failed. Retrying... (" + String(i + 1) + "/" + String(retryCount) + ")");
      delay(1000); // Wait 1 second before retrying
    }

    // Check the response
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Response: " + payload);

      // Parse the JSON response
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      // Extract device states from the "devices" object
      JsonObject devices = doc["devices"];
      if (!devices.isNull()) {
        bool bulbStatus = devices["Bulb"];
        bool doorStatus = devices["Door"];
        bool fanStatus = devices["Fan"];
        bool lockStatus = devices["Lock"];

        // Send commands to Arduino Uno
        Serial1.println("BULB:" + String(bulbStatus));
        Serial1.println("DOOR:" + String(doorStatus));
        Serial1.println("FAN:" + String(fanStatus));
        Serial1.println("LOCK:" + String(lockStatus));

        // Print device states for debugging
        Serial.println("Bulb status: " + String(bulbStatus ? "ON" : "OFF"));
        Serial.println("Door status: " + String(doorStatus ? "ON" : "OFF"));
        Serial.println("Fan status: " + String(fanStatus ? "ON" : "OFF"));
        Serial.println("Lock status: " + String(lockStatus ? "ON" : "OFF"));
      } else {
        Serial.println("Devices object not found in JSON response.");
      }
    } else {
      Serial.println("HTTP request failed. Error code: " + String(httpCode));
      Serial.println("Error details: " + http.errorToString(httpCode));
    }

    http.end(); // Close the connection
  } else {
    Serial.println("Wi-Fi not connected.");
  }

  delay(1000); // Wait 1 second before next read
}