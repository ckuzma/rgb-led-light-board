#include <ESP8266WiFi.h>

// Change these to fit your setup
#define WIDTH 16
#define HEIGHT 8

// Create our working array
int fictionalBoard[HEIGHT][WIDTH]; // By not declaring a value, all cells have a value of 0

// Set the color number progressions
const int colorProgressionCount = 5;
const int colorProgression[colorProgressionCount] = {4, 3, 2, 1, 0};
const int maxDrops = 2;

void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
}

int pickRandomColumn() {
  return random(0, WIDTH);  
}

bool pickRandomDo() {
  if(random(0, 100) < 10) {
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
      if(fictionalBoard[y][x] == colorProgression[0] && y != HEIGHT-1) {
        fictionalBoard[y+1][x] = colorProgression[0];
      }
      fictionalBoard[y][x] = getNextColor(fictionalBoard[y][x]);
    }
  }
}

void prettyPrintBoard() {
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
      fictionalBoard[0][pickRandomColumn()] = colorProgression[0];
    }
  }
}

void loop() {
  prettyPrintBoard();
  iterateDrops();
  delay(500);
}