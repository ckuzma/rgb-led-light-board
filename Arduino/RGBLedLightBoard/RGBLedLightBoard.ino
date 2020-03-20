#include <Adafruit_NeoMatrix.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PW";

#define PIN D3
#define WIDTH 16
#define HEIGHT 8

int displayArray[HEIGHT][WIDTH][3] = {{0, 0, 0}};

Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  HEIGHT, WIDTH,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );
  
void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize the matrix
  Matrix.begin();

  // Initialize the random seed
  randomSeed(2020);
}

void displayBoard() {
  for(int x = 0; x < HEIGHT; x++){
    for(int y = 0; y < WIDTH; y++) {
      int r = displayArray[x][y][0];
      int g = displayArray[x][y][1];
      int b = displayArray[x][y][2];
        Matrix.drawPixel(x, y, Matrix.Color(r, g, b));
    }
  }
  Matrix.show();
}

void changePixel(int x, int y, int r, int g, int b) {
  displayArray[x][y][0] = r;
  displayArray[x][y][1] = g;
  displayArray[x][y][2] = b;
}

void makePrettyColors() {
  // Our selected "pretty color" options
  int prettyColors[5][3] = { // [number_of_colors][r,g,b]
    {30,0,0},
    {0,20,0},
    {0,0,15},
    {30,20,0},
    {0,30,20}
  };

  // Pick a random pixel to change
  int x = random(HEIGHT);
  int y = random(WIDTH);

  // Pick a color and change the pixel
  int colorNumber = random(5);
  changePixel(x, y, prettyColors[colorNumber][0], prettyColors[colorNumber][1], prettyColors[colorNumber][2]);
}

void loop() {
  // Simple animation
  makePrettyColors();

  // Display it!
  displayBoard();

  delay(80);
}