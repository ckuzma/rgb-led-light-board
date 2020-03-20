#include <Adafruit_NeoMatrix.h>

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