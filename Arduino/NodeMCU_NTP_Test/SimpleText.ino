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
char text[] = "18:55";
const int scroll_width = -65;
const int color_count = 1;
const uint16_t colors[] = {
  Matrix.Color(64,166,250)
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

void printText(char text[]) {
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

void loop() {
  printText(text);
}