#include <Adafruit_NeoMatrix.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

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

// WiFi and UTC adjust settings
char ssid[]     = "Wave G Public WiFi";
char pass[]     = "";
int hoursAdjust = -7; // UTC-7 = Pacific Time (Summer)

// Time retrieval settings
const int localPort = 2390; // Port for UDP packets
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // Buffer to hold incoming and outgoing packets

// Display / refresh interval stuff
int lastRefreshSeconds = millis() / 1000;

// Text display setup
String timeAsString       = "00:00";
int SCROLL_PLACEHOLDER    = Matrix.width();
const int SCROLL_WIDTH    = -35; // This was fit to the width of "00:00"
const uint16_t TEXT_COLOR = Matrix.Color(64,166,250); // whiteish blue color
const uint16_t BG_COLOR   = Matrix.Color(255, 0, 0); // full red

// Create a UDP instance
IPAddress timeServerIP; // time.nist.gov NTP server address
WiFiUDP udp;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  Matrix.begin();
  Matrix.setTextWrap(false);
  Matrix.setBrightness(255);
}

void loop() {
  // Place to save epoch
  unsigned long epoch;

  // Get a random NPT time server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  // Send NTP packet to time server and wait a bit to allow for a response
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  delay(1000);

  // Check to see if we have received a data packet
  int cb = udp.parsePacket();
  if (!cb) { // If not...
    Serial.println("No packet received yet");
  }
  else {
    // Debug print some info
    // Serial.print("packet received, length=");
    // Serial.println(cb);
    
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    
    // Parse out the epoch
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    
    // Convert response to epoch, and set it to the matrix printed string
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    epoch = secsSince1900 - seventyYears;
    epoch = epoch + (hoursAdjust * 3600);
  }
  printTimeForXSeconds(epoch, 60);
}

void printTimeToMatrix() {
  // Fill background, set cursor, and print
  Matrix.fillScreen(BG_COLOR);
  Matrix.setCursor(SCROLL_PLACEHOLDER, 0);
  Matrix.print(timeAsString);

  // Bump where we print from next
  if(--SCROLL_PLACEHOLDER < SCROLL_WIDTH){
    SCROLL_PLACEHOLDER = Matrix.width();
    Matrix.setTextColor(TEXT_COLOR);
  }

  // Show it
  Matrix.show();
  Serial.println("We are printing!");

  // Delay
//   delay(100);
}


String makeTimeDigitString(int timeDigit) {
    if(timeDigit < 10) {
        return "0" + String(timeDigit);
    }
    else {
        return String(timeDigit);
    }
}

String getHoursMinutesSecondsStringFromEpoch(long epoch) {
  String parsedTime = "";
  parsedTime += makeTimeDigitString((epoch  % 86400L) / 3600); // Hours
  parsedTime += ":";
  parsedTime += makeTimeDigitString((epoch  % 3600) / 60); // Minutes
  parsedTime += ":";
  parsedTime += makeTimeDigitString(epoch % 60); // Seconds
  return parsedTime;
}

void printTimeToSerial(long epoch) {
  // Get time string and save it
  timeAsString = getHoursMinutesSecondsStringFromEpoch(epoch);

  // Print it out
  Serial.println("Time: " + timeAsString);
}

void printTimeForXSeconds(long epoch, int seconds) {
  int startTime = millis();
  while(millis() < startTime + 60) {
    // Print it out to terminal every second
    int terminalPrintCounter = startTime;
    if(millis() > terminalPrintCounter + 1000) {
        printTimeToSerial(epoch);
        terminalPrintCounter = millis();
    }

    // Scroll the matrix
    int matrixScrollCounter = startTime;
    if(millis() > matrixScrollCounter + 100) {
        printTimeToMatrix();
        matrixScrollCounter = millis();
    }
  }

//   // Will print the time for ten seconds
  
//   for(int x; x<seconds; x++) {
//     printTimeToSerial(epoch);
//     epoch++;
//     delay(1000);
//   }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}