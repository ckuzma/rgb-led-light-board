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
char ssid[]     = "WIFI_SSID";
char pass[]     = "WIFI_PW";
int hoursAdjust = -7; // UTC-7 = Pacific Time (Summer)

// Time retrieval settings
const int localPort = 2390; // Port for UDP packets
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // Buffer to hold incoming and outgoing packets

// Display / refresh interval stuff
int lastRefreshSeconds = millis() / 1000;

// Text display setup
String OUTPUT_STRING      = "00:00:00";
int SCROLL_PLACEHOLDER    = Matrix.width();
const int SCROLL_WIDTH    = -45; // This fits the width of "00:00:00"
const uint16_t TEXT_COLOR = Matrix.Color(0, 0, 255); // full blue
const uint16_t BG_COLOR   = Matrix.Color(0, 0, 0); // full off

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
  
  while(WiFi.status() != WL_CONNECTED) {
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
  
  // Get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // Send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  // Read in the UDP buffer
  int cb = udp.parsePacket();

  // If we have a UDP packet, do something
  if (!cb) {
    Serial.println("No UDP packet yet");
  }
  else {
    // Print the packet length
    Serial.print("UDP packet received, length=");
    Serial.println(cb);

    // Read the received UDP packet
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    // Parse the packet and extrach the epoch
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    epoch = secsSince1900 - seventyYears;
    epoch = epoch + (hoursAdjust * 3600);
  }

  // Print the time for 60 seconds
  printTimeForXSeconds(epoch, 60);
}

void printTimeForXSeconds(long epoch, int seconds) {
  for(int x; x<seconds; x++) {
    printTime(epoch);
    for(int y=0; y<10; y++) {
      matrixTextScroll();
      delay(100);
    }
    epoch++;
  }
}

void printTime(long epoch) { // Note that this should happen every 1000 milliseconds
  // Get time string
  OUTPUT_STRING = getHoursMinutesSecondsStringFromEpoch(epoch);

  // Print it out
  Serial.println("Current time: " + OUTPUT_STRING);    
}

void matrixTextScroll() { // Note that this should happen every 100 milliseconds
  // Fill background, set cursor, and print
  Matrix.fillScreen(BG_COLOR);
  Matrix.setCursor(SCROLL_PLACEHOLDER, 0);
  Matrix.print(OUTPUT_STRING);

  // Bump where we print from next
  if(--SCROLL_PLACEHOLDER < SCROLL_WIDTH){
    SCROLL_PLACEHOLDER = Matrix.width();
    Matrix.setTextColor(TEXT_COLOR);
  }

  // Show it
  Matrix.show();
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

unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("Sending an NTP packet request...");
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}