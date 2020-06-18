#include <Adafruit_NeoMatrix.h>

// Change these to fit your setup
#define PIN D3
#define WIDTH 16
#define HEIGHT 8

// Array Setup
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );

// Text setup
char text[]               = "00:00";
int SCROLL_PLACEHOLDER    = Matrix.width();
const int SCROLL_WIDTH    = -35; // This was fit to the width of "00:00"
const uint16_t TEXT_COLOR = Matrix.Color(64,166,250); // whiteish blue color
const uint16_t BG_COLOR   = Matrix.Color(255, 0, 0); // full red

void setup() {
  Matrix.begin();
  Matrix.setTextWrap(false);
  Matrix.setBrightness(255);
}

void printText(char text[]) {
  // Fill background, set cursor, and print
  Matrix.fillScreen(BG_COLOR);
  Matrix.setCursor(SCROLL_PLACEHOLDER, 0);
  Matrix.print(text);

  // Bump where we print from next
  if(--SCROLL_PLACEHOLDER < SCROLL_WIDTH){
    SCROLL_PLACEHOLDER = Matrix.width();
    Matrix.setTextColor(TEXT_COLOR);
  }

  // Show it
  Matrix.show();

  // Delay
  delay(100);
}

void loop() {
  printText(text);
}