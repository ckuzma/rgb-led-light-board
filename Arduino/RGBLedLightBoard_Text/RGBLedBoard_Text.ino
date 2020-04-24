// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif
// Change these to fit your setup
#define PIN D3
#define WIDTH 16
#define HEIGHT 8


// Init and config an internal data array plus the NeoMatrix
int displayArray[HEIGHT][WIDTH][3] = {{0, 0, 0}};
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );

const uint16_t colors[] = {
  Matrix.Color(0, 255, 0)
 };

void setup() {
  Matrix.begin();
  Matrix.setTextWrap(false);
  Matrix.setBrightness(255);
  Matrix.setTextColor(colors[0]);
}

int x    = Matrix.width();
int pass = 0;

void loop() {
  Matrix.fillScreen(0);
  Matrix.setCursor(x, 0);
  Matrix.print(F("hello"));
  if(--x < -36) {
    x = Matrix.width();
    if(++pass >= 1) pass = 0;
    Matrix.setTextColor(colors[pass]);
  }
  Matrix.show();
  delay(100);
}
