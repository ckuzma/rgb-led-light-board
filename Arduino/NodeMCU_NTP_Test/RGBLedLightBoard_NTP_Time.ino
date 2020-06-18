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
char ssid[]     = "Festnetz";
char pass[]     = "reutlingen";
int hoursAdjust = -7; // UTC-7 = Pacific Time (Summer)

// Time retrieval settings
const int localPort = 2390; // Port for UDP packets
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; // Buffer to hold incoming and outgoing packets

// Display / refresh interval stuff
int lastRefreshSeconds = millis() / 1000;

// Text display setup
char text[]               = "00:00";
int SCROLL_PLACEHOLDER    = Matrix.width();
const int SCROLL_WIDTH    = -35; // This was fit to the width of "00:00"
const uint16_t TEXT_COLOR = Matrix.Color(64,166,250); // whiteish blue color
const uint16_t BG_COLOR   = Matrix.Color(255, 0, 0); // full red

// Create a UDP instance
IPAddress timeServerIP; // time.nist.gov NTP server address
WiFiUDP udp;

void setup()
{
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

void loop()
{
  // Place to save epoch
  unsigned long epoch;
  
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = " );
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    //unsigned long epoch = secsSince1900 - seventyYears;
    epoch = secsSince1900 - seventyYears;
    epoch = epoch + (hoursAdjust * 3600);
    // print Unix time:
    //Serial.println(epoch);

    // Print it
    //printTime(epoch);

  }
  // wait ten seconds before asking for the time again
  //delay(10000);
  printTimeForXSeconds(epoch, 60);
}

void printText() {
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

void printTime(long epoch) {
  // Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
  int hours = (epoch  % 86400L) / 3600;

  // Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
  int minutes = (epoch  % 3600) / 60;

  // Serial.println(epoch % 60); // print the second
  int seconds = epoch % 60;

  Serial.println("Hours: ");
  Serial.println(hours);
  Serial.println("Minutes: ");
  Serial.println(minutes);
  Serial.println("Seconds: ");
  Serial.println(seconds);
    
}

void printTimeForXSeconds(long epoch, int seconds) {
  // Will print the time for ten seconds
  
  for(int x; x<seconds; x++) {
    printTime(epoch);
    epoch++;
    delay(1000);
  }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
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