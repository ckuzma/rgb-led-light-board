#include <Adafruit_NeoMatrix.h>

// Change these to fit your setup
#define PIN D3
#define WIDTH 16
#define HEIGHT 8

// Array initialization
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );

// Text setup
const char text[] = "happy hour";
const int scroll_width = -65;
const int color_count = 5;
const uint16_t colors[] = {
  Matrix.Color(255,186,21),
  Matrix.Color(249,251,79),
  Matrix.Color(19,249,246),
  Matrix.Color(64,166,250),
  Matrix.Color(240,61,248)
 };

// Some initial variable instantiations
int x = Matrix.width();
int pass = 0;

void setup() {
  Matrix.begin();
  Matrix.setTextWrap(false);
  Matrix.setBrightness(255);
  Matrix.setTextColor(colors[0]);
}

void loop() {
  Matrix.fillScreen(0);
  Matrix.setCursor(x, 0);
  Matrix.print(text);
  if(--x < scroll_width){
    x = Matrix.width();
    if(++pass >= color_count){
      pass = 0;
    }
    Matrix.setTextColor(colors[pass]);
  }
  Matrix.show();
  delay(100);
}