/*
  Chatty AI Meat Cooking Assistant ESP32 Code
  Code development by Nicholas Williamson
  Copyright 2025. All rights reserved.
  This code is provided as Open Source Code.
  You are free to use this code in your own projects, modify it or build on it, provided you mention my name in your credits.
*/

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_MLX90614.h>
#include <TFT_eSPI.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourceBuffer.h>

// https://github.com/earlephilhower/ESP8266Audio - Search for ESP8266Audio in the Library Manager and install it.

AudioGeneratorMP3 *mp3;
AudioFileSourceHTTPStream *file;
AudioOutputI2S *out;

WebServer server(80); // New HTTP server for receiving config

// Replace with your network credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Backend server URL
const char* serverUrl = "http://your_backend_ip:3000/temperature";

// Sensor
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Display
TFT_eSPI tft = TFT_eSPI();

// RGB LED Pins
const int redPin = 27;
const int greenPin = 32;
const int bluePin = 33;

// Buzzer Pin
const int buzzerPin = 13;

// Create global variables
String protein = "beef";
String cut = "sirloin";
int weight = 375;
String doneness = "medium";

out = new AudioOutputI2S();
out->SetPinout(26, 25, 22); // BCLK, LRC, DOUT
out->SetGain(0.5);

AudioFileSourceHTTPStream *httpStream = new AudioFileSourceHTTPStream(audioURL);
AudioFileSourceBuffer *buff = new AudioFileSourceBuffer(httpStream, 2048); // 2KB buffer

buff->begin(); // Always call this

mp3 = new AudioGeneratorMP3();
mp3->begin(buff, out);

void handleSpeak() {
  if (server.method() != HTTP_POST) {
    server.send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String body = server.arg("plain");

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  String message = doc["message"] | "";

  Serial.println("[ESP32] Speak message received:");
  Serial.println(message);

  server.send(200, "application/json", "{\"status\":\"Message received\"}");

  // Trigger playback
  delete file;
  delete mp3;

  file = new AudioFileSourceHTTPStream("http://<SERVER_IP>:3000/audio.mp3");
  mp3 = new AudioGeneratorMP3();
  mp3->begin(file, out);

  server.send(200, "application/json", "{\"status\":\"Playing\"}");

}

void setup() {
  Serial.begin(115200);

  // Initialize pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // DAC Output to PAM8403 Amplifier
  // GPIO25 - DAC1 - Left Audio Out
  // GPIO26 - DAC2 - Right Audio Out

  // MAX9814 Microphone Input
  // GPIO36 - ADC1_CH0 - Audio input from MAX9814

  // Initialize TFT display
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(0, 0);
  tft.println("Connecting to WiFi...");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  tft.println("WiFi Connected!");
  Serial.println("");
  Serial.print("Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());

  // Initialize temperature sensor
  if (!mlx.begin()) {
    Serial.println("MLX90614 not found!");
    while (1);
  }

  // Call server
  server.on("/", handleRoot);
  server.on("/config", HTTP_POST, handleConfig);
  server.on("/speak", HTTP_POST, handleSpeak); // Register /speak endpoint
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  float temp = mlx.readObjectTempC();
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.printf("Temp: %.2f C\n", temp);
  tft.println("Waiting for response...");
  Serial.printf("Temp: %.2f C\n", temp);

  sendTemperatureToServer(temp);

  server.handleClient();

  delay(10000);

}

void handleRoot() {
  server.send(200, "text/plain", "ESP32 is running.");
}

// Handle /config POST from web UI
void handleConfig() {

  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    Serial.println("Received config: " + body);

    // Parse JSON manually (lightweight)
    if (body.indexOf("protein") > -1) {
      int pStart = body.indexOf("protein") + 10;
      int pEnd = body.indexOf("\"", pStart);
      protein = body.substring(pStart, pEnd);
    }

    if (body.indexOf("cut") > -1) {
      int cStart = body.indexOf("cut") + 6;
      int cEnd = body.indexOf("\"", cStart);
      cut = body.substring(cStart, cEnd);
    }

    if (body.indexOf("weight") > -1) {
      int wStart = body.indexOf("weight") + 8;
      int wEnd = body.indexOf(",", wStart);
      weight = body.substring(wStart, wEnd).toInt();
    }

    if (body.indexOf("doneness") > -1) {
      int dStart = body.indexOf("doneness") + 11;
      int dEnd = body.indexOf("\"", dStart);
      doneness = body.substring(dStart, dEnd);
    }

    server.send(200, "application/json", "{\"status\":\"Config updated\"}");
  
  } else {
  
    server.send(400, "application/json", "{\"error\":\"No body received\"}");
  
  }

}

void sendTemperatureToServer(float temp) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String json = "{";
    json += "\"temp\":" + String(temp) + ",";
    json += "\"protein\":\"" + protein + "\",";
    json += "\"cut\":\"" + cut + "\",";
    json += "\"weight\":" + String(weight) + ",";
    json += "\"doneness\":\"" + doneness + "\"}";

    int httpResponseCode = http.POST(json);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);

      String responseLower = response;
      responseLower.toLowerCase();

      if (responseLower.indexOf("thinking") >= 0) {
        flashRGB("green", 3, 300);
      } else if (responseLower.indexOf("answering") >= 0) {
        flashRGB("blue", 3, 300);
      } else if (responseLower.indexOf("flip") >= 0) {
        flashRGB("yellow", 3, 300);
        tone(buzzerPin, 1000, 500);
        tft.setTextColor(TFT_YELLOW);
        tft.println("Flip the meat!");
      } else if (responseLower.indexOf("done") >= 0 || responseLower.indexOf("ready") >= 0 || responseLower.indexOf("cooked") >= 0) {
        flashRGB("red", 3, 300);
        tone(buzzerPin, 2000, 500);
        tft.setTextColor(TFT_RED);
        tft.println("Cooking complete!");
      } else {
        turnOffRGB();
      }
    } else {
      Serial.printf("Error sending POST: %d\n", httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

// Flash the RGB LED with specified color
void flashRGB(String color, int flashes, int delayTime) {
  for (int i = 0; i < flashes; i++) {
    setRGBColor(color);
    delay(delayTime);
    turnOffRGB();
    delay(delayTime);
  }
}

// Set specific color on RGB LED
void setRGBColor(String color) {
  if (color == "red") {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, LOW);
  } else if (color == "green") {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
  } else if (color == "blue") {
    digitalWrite(redPin, LOW);
    digitalWrite(greenPin, LOW);
    digitalWrite(bluePin, HIGH);
  } else if (color == "yellow") {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, LOW);
  }
}

// Turn off the RGB LED
void turnOffRGB() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}