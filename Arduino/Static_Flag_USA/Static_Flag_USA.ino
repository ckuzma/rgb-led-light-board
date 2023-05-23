// Arduino OTA stuff
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Get configuration values
#include "config.h"
const char* ssid = STASSID;
const char* password = STAPSK;
const char* host = "OTA-RGB-LED-LightBoard";

// NeoMatrix stuff
#include <Adafruit_NeoMatrix.h>

// Change these to fit your setup
#define PIN D3
#define WIDTH 16
#define HEIGHT 8
const int BRIGHTNESS = 255;  // 0 = off, 255 = full brightness

// Init an internal place to store what should be displayed
/* USA Flag colors
  {255, 50, 50},   // Red
  {180, 180, 180}, // White
  {50, 50, 255},    // Blue
*/
int displayArray[HEIGHT][WIDTH][3] = {
  {{50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}},
  {{180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}},
  {{50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}},
  {{180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {50, 50, 255}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}},
  {{255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}},
  {{180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}},
  {{255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}, {255, 50, 50}},
  {{180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}, {180, 180, 180}}
};

// Initialize the matrix
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );
  
void setup(){
  // Get WiFi up and going for OTA purposes
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("Retrying connection...");
  }
  ArduinoOTA.setHostname(host);
  ArduinoOTA.begin();

  // Initialize the matrix
  Matrix.begin();
  Matrix.setBrightness(BRIGHTNESS);

  // Draw all of it to the Matrix
  for(int y=0; y<HEIGHT; y++){
    for(int x=0; x<WIDTH; x++){
      Matrix.drawPixel(x, y, Matrix.Color(
        displayArray[y][x][0], // Red
        displayArray[y][x][1], // Green
        displayArray[y][x][2]  // Blue
      ));
    }
  }

  // Show it
  Matrix.show();
}



void loop(){
  ArduinoOTA.handle();
  // Nothing here
}
