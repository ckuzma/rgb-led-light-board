// Originally found here:
// https://www.youtube.com/watch?v=TfzFJkDgg7s

#include <ESP8266WiFi.h>
#include "espneotext.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN D3
#define WIDTH 16
#define HEIGHT 8

const char* ssid = "WIFI_2.4GHZ_SSID";
const char* password = "WIFI_PASSWORD";
String value;
//char charBuf[60];
int z=0;
int pixelsInText;
// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);



// Array initialization
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );

const uint16_t colors[] = {
  Matrix.Color(255, 255, 255),
  Matrix.Color(0, 255, 0),
  Matrix.Color(0, 0, 255) };


void setup() {
    Matrix.begin();
  Matrix.setTextWrap(false);
  Matrix.setBrightness(255);
  Matrix.setTextColor(colors[0]);
  Serial.begin(115200);
  delay(10);

    // Connect to WiFi network
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
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

}

int x    = Matrix.width();
int pass = 0;

void loop() {

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
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
pixelsInText = (value.length() * 7)+8;
  Serial.println(value);
//value.toCharArray(charBuf, 70) ;
}
else {  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += file1;  
  client.flush();

  // Send the response to the client
while(s.length()>2000)
{
  String dummy = s.substring(0,2000);
  client.print(dummy);
  s.replace(dummy," ");
}
  
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is destroyed
}
if(z){
  while(x+17 > (Matrix.width() - pixelsInText)){
 Matrix.fillScreen(0);
  Matrix.setCursor(--x, 0);
  Matrix.print(value);
  Matrix.show();
  delay(100);
  }
  x    = Matrix.width();
}
}