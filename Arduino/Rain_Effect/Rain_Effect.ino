#include <Adafruit_NeoMatrix.h>
#include <ESP8266WiFi.h>

// Change these to fit your setup
#define PIN D3
#define WIDTH 16
#define HEIGHT 8

// Changes behavior of the drops
const int maxDrops = 6; // Maximum possible drops on-display
const int frameDelay = 100; // Time between visualization "frame" updates
const int dropProbability = 20; // Percentage likelihood of a drop happening
const bool moveHorizontal = true; // Move drops horizontally or not

// Change this to match the desired colors and brightness
// to display.  The number of colors and ordering here
// corresponds to colorProgressionCount and colorProgression
// below in the rain code.
const int BRIGHTNESS = 255;  // 0 = off, 255 = full brightness

// Soft-white rain
//const int colorMap[5][3] = {
//  {0, 0, 0},
//  {70, 30, 30},
//  {102, 92, 82},
//  {228, 218, 208},
//  {255, 245, 235}
//};

// Blue rain
//const int colorMap[5][3] = {
//  {0, 0, 0},
//  {27, 42, 102},
//  {48, 74, 178},
//  {62, 95, 228},
//  {71, 108, 255}
//};

// Red rain
const int colorMap[5][3] = {
  {0, 0, 0},
  {102, 5, 5},
  {178, 15, 15},
  {228, 20, 20},
  {255, 40, 40}
};

// Create our "fictional" board for Serial printing and ease-of-coding
int fictionalBoard[HEIGHT][WIDTH]; // By not declaring a value, all cells have a value of 0

// Init and config an internal data array plus the NeoMatrix
Adafruit_NeoMatrix Matrix = Adafruit_NeoMatrix(
  WIDTH, HEIGHT,
  PIN,
  NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS   + NEO_MATRIX_ZIGZAG,
  NEO_GRB           + NEO_KHZ800
  );

// ------------------------------------ //
// --- Below is code for the board ---- //
// ------------------------------------ //

void displayOnBoard() {
  for(int x = 0; x < WIDTH; x++){
    for(int y = 0; y < HEIGHT; y++) {
      Matrix.drawPixel(x, y,Matrix.Color(colorMap[fictionalBoard[y][x]][0], colorMap[fictionalBoard[y][x]][1], colorMap[fictionalBoard[y][x]][2]));
    }
  }
  Matrix.show();
}

// ------------------------------------ //
// --- Below is rain-specific code ---- //
// ------------------------------------ //

// Set the color number progressions
const int colorProgressionCount = 5;
const int colorProgression[colorProgressionCount] = {4, 3, 2, 1, 0};

int pickRandomColumn() {
  if(moveHorizontal == false) {
    return random(0, WIDTH);
  }
  else { // aka moveHorizontal == true
    return random(0, HEIGHT);
  }
}

bool pickRandomDo() {
  if(random(0, 100) < dropProbability) {
    return true;
  }
  return false;
}

int countOccupiedColumns() {
  int occupiedCount = 0;
  for(int x=0; x<WIDTH; x++) {
    bool occupied = false;
    for(int y=0; y<HEIGHT; y++) {
      if(fictionalBoard[y][x] != 0) {
        occupied = true;
      }
    }
    if(occupied == true) {
      occupiedCount++;
    }
  }
  return occupiedCount;
}

int getNextColor(int previousColor) {
  if(previousColor != 0) {
    for(int x=0; x<colorProgressionCount; x++) {
      if(previousColor == colorProgression[x]) {
        return colorProgression[x+1];
      }
    }
    return -1;
  }
  else {
    return 0;
  }
}

void dropDrops() {
  for(int x=WIDTH-1; x>=0; x--) {
    for(int y=HEIGHT-1; y>=0; y--) {
      if(moveHorizontal == false) {
        if(fictionalBoard[y][x] == colorProgression[0] && y != HEIGHT-1) {
          fictionalBoard[y+1][x] = colorProgression[0];
        }
        fictionalBoard[y][x] = getNextColor(fictionalBoard[y][x]);
      }
      else { // aka moveHorizontal == true
        if(fictionalBoard[y][x] == colorProgression[0] && x != WIDTH-1) {
          fictionalBoard[y][x+1] = colorProgression[0];
        }
        fictionalBoard[y][x] = getNextColor(fictionalBoard[y][x]);
      }
    }
  }
}

void serialPrintBoard() {
  Serial.println("--------------------------------");
  for(int y=0; y<HEIGHT; y++) {
    for(int x=0; x<WIDTH; x++) {
      Serial.print(fictionalBoard[y][x]);
      Serial.print(" ");
    }
    Serial.println("");
  }
}

void iterateDrops() {
  dropDrops();
  if(countOccupiedColumns() < maxDrops) {
    if(pickRandomDo()) {
      if(moveHorizontal == false) {
        fictionalBoard[0][pickRandomColumn()] = colorProgression[0];
      }
      else { // aka moveHorizontal == true
        fictionalBoard[pickRandomColumn()][0] = colorProgression[0];
      }
    }
  }
}


// ------------------------------------ //
// --- Below is standard Arduino   ---- //
// ------------------------------------ //

void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);

  // Initialize the matrix
  Matrix.begin();
  Matrix.setBrightness(BRIGHTNESS);
}

void loop() {
  // serialPrintBoard();
  displayOnBoard();
  iterateDrops();
  delay(frameDelay);
}
