// Originally found here:
// https://www.youtube.com/watch?v=TfzFJkDgg7s

#include <ESP8266WiFi.h>
#include <Adafruit_NeoMatrix.h>
#include "TextPageHTML.h"

// Setup for our particular display
#define PIN D3
#define WIDTH 16
#define HEIGHT 8

// WiFi settings and text color
const char* ssid     = "WIFI_2.4GHZ_SSID";
const char* password = "WIFI_PASSWORD";
const int red   = 255;
const int green = 255;
const int blue  = 255;

// Place to store the string
String value;

// Used later for calculating scroll lengths
int pixelsInText;
int z=0;

// Start the server and listen on port 80
WiFiServer server(80);

// Setup the array
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT, PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
);

// Not sure
int x = Matrix.width();

void setup() {
  Serial.begin(115200);

  // Initialize the matrix
  Matrix.begin();
  Matrix.setTextWrap(false);
  Matrix.setBrightness(255);
  Matrix.setTextColor(Matrix.Color(red, green, blue));

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start server
  server.begin();
  Serial.println("Server started");

  // Print serverIP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("New HTTP client connection");
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String command1 = client.readStringUntil('/');
  String command = client.readStringUntil('/');
  Serial.println(command);

  if (command == "text") {
    value = client.readStringUntil('/');
    z=1;
    value.replace("%20", " ");
    pixelsInText = (value.length() * 7) + 8;
    Serial.println(value);
  }
  else { // Prepare the response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += page_html;  
    client.flush();

    // Send the response to the client
    while(s.length() > 2000) {
      String dummy = s.substring(0, 2000);
      client.print(dummy);
      s.replace(dummy, " ");
    }
  
    client.print(s);
    delay(1);
    Serial.println("HTTP client disconnected");

    // The client will actually be disconnected 
    // when the function returns and 'client' object is destroyed
  }
  if(z) {
    while(x + 17 > (Matrix.width() - pixelsInText)) {
      Matrix.fillScreen(0);
      Matrix.setCursor(--x, 0);
      Matrix.print(value);
      Matrix.show();
      delay(100);
    }
    x = Matrix.width();
  }
}