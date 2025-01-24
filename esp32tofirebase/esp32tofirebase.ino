#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WIFI_SSID "Teacher_Room" // Wi-Fi SSID from the provided code
#define WIFI_PASSWORD "Ncit@123" // Wi-Fi password from the provided code
#define FIREBASE_URL "https://fireship-5c979-default-rtdb.firebaseio.com/.json" // Firebase URL from the provided code
#define API_KEY "AIzaSyD_mPzeWeaIvDaRwuw9TmJaaKsBv5B9XWI" // API key from the provided code

void setup() {
  Serial.begin(115200);

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

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the full URL with the API key
    String url = String(FIREBASE_URL) + "?auth=" + API_KEY;

    // Send HTTP GET request
    http.begin(url);
    int httpCode = http.GET();

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
        bool happyStatus = devices["Happy"];
        bool bulb2Status = devices["bulb"];

        // Print device states
        Serial.println("Bulb status: " + String(bulbStatus ? "ON" : "OFF"));
        Serial.println("Door status: " + String(doorStatus ? "ON" : "OFF"));
        Serial.println("Fan status: " + String(fanStatus ? "ON" : "OFF"));
        Serial.println("Happy status: " + String(happyStatus ? "ON" : "OFF"));
        Serial.println("bulb status: " + String(bulb2Status ? "ON" : "OFF"));
      } else {
        Serial.println("Devices object not found in JSON response.");
      }
    } else {
      Serial.println("HTTP request failed. Error code: " + String(httpCode));
    }

    http.end(); // Close the connection
  } else {
    Serial.println("Wi-Fi not connected.");
  }

  delay(1000); // Wait 1 second before next read
}