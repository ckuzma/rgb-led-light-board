#include <Adafruit_NeoMatrix.h>
#include <ESP8266WiFi.h>

// Change these to fit your setup
#define PIN D3
#define WIDTH 16
#define HEIGHT 8

// Change this to match the desired colors
// to display.  If using more/less than 5
// different color options, change 5 in the
// initialization for prettyColors
const int BRIGHTNESS = 255;  // 0 = off, 255 = full brightness
const int delayTime = 320; // 80 = original faster default

// Modified version of: https://www.schemecolor.com/going-up.php
const int numberOfColors = 11;
const int prettyColors[numberOfColors][3] = {
  {137, 28, 149},
  {172, 49, 75},
  {223, 229, 228},
  {32, 130, 174},
  {26, 87, 179},
  {224, 171, 86},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};


//const int prettyColors[numberOfColors][3] = {
//  {37, 37, 218}, // Bright blue
//  {17, 17, 110}, // Darker blue
//  {0, 0, 0},
//  {0, 0, 0},
//  {0, 0, 0}
//};

// Init and config an internal data array plus the NeoMatrix
int displayArray[HEIGHT][WIDTH][3] = {{0, 0, 0}};
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );
  
void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  Serial.println("Booting");
    
  // Initialize the matrix
  Matrix.begin();
  Matrix.setBrightness(BRIGHTNESS);

  // Initialize the random seed
  randomSeed(2020);
}

void displayBoard() {
  for(int x = 0; x < WIDTH; x++){
    for(int y = 0; y < HEIGHT; y++) {
      int r = displayArray[y][x][0];
      int g = displayArray[y][x][1];
      int b = displayArray[y][x][2];
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
  // Pick a random pixel to change
  int x = random(HEIGHT);
  int y = random(WIDTH);

  // Pick a color and change the pixel
  int colorNumber = random(numberOfColors);
  changePixel(x, y, prettyColors[colorNumber][0], prettyColors[colorNumber][1], prettyColors[colorNumber][2]);
}

void loop() {
  // Simple animation
  makePrettyColors();

  // Display it!
  displayBoard();

  delay(delayTime);
}
