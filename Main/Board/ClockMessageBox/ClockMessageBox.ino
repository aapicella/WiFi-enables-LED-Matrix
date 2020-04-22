/*
 * ClockMessageBox
 * 
 * Wemos D1 Mini Pro (ESP8266)
 * Max7219 LED matrix
 * DS3231 Clock
 * 
 * Original project by aapicella.
 * https://github.com/aapicella/WiFi-enables-LED-Matrix
 * https://www.instructables.com/id/WIFI-Enabled-LED-Matrix/
 */
 
#include <WiFiManager.h>                  // https://github.com/tzapu/WiFiManager - web page @ 192.168.4.1
#include <SPI.h>
#include <MD_MAX72xx.h>
/* 
 *  _BV(bit) is a macro defined in  avr/io.h file in AVR libraries.
 *  Added the following to DS3231_Simple.h for temporary fix.
 *  
 * #define _BV(bit) \
 * (1 << (bit))
 */
#include <DS3231_Simple.h>

/*----------------------------system----------------------------*/
const String _progName = "ClockMessageBox";
const String _progVers = "0.4";           // Added DS3231 Clock
#define DEBUG 1                           // 0 or 1 - remove later

/*----------------------------pins------------------------------*/
// Max7219 to Wemos D1 Mini Pro (SPI - Serial) - 5V
#define DATA_PIN  13                      // DIN to D7 (MOSI - GPIO 13)
#define CS_PIN    12                      // CS  to D6 (MISO - GPIO 12)
#define CLK_PIN   14                      // CLK to D5 (SCLK - GPIO 14)
// DS3231 Clock to Wemos D1 Mini Pro (I2C) - 5V
#define SCL_PIN   5                       // SCL to D1 (GPIO5)
#define SDA_PIN   4                       // SDA to D2 (GPIO4)

/*-----------------------------WIFI-----------------------------*/
WiFiServer _server(80);                   // TCP server at port 80 will respond to HTTP requests
IPAddress _ip;
bool _wifiAvailable = false;              // Is wifi available for use?

/*-----------------------------WIFI Manager---------------------*/
const char* _apName = "ClockMessageBox";
const char* _apPassword = "password";

/*----------------------------LED Matrix------------------------*/
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW  // PAROLA_HW
#define MAX_DEVICES 2
MD_MAX72XX _mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7 (4 8x8 blocks)
const int LEDMATRIX_WIDTH = 31;  
//const int LEDMATRIX_HEIGHT = 7;
//const int LEDMATRIX_SEGMENTS = 4;
int x = LEDMATRIX_WIDTH, y=0;             // start top left

/*----------------------------DS3231----------------------------*/
 DS3231_Simple Clock;

/*----------------------------values----------------------------*/
char _text[75] = " Hello ";               // Marquee text
int _length = strlen(_text);
const int _animDelay = 100; //75;
//unsigned long _myTime=millis();

  
void setup() 
{
  if (DEBUG) {
    Serial.begin(115200);
    Serial.println();
  }
  
  Clock.begin();
  _mx.begin();                            // Initialize the display
  displayText(_text);
  
  setupWifiManager();
  
  //sprintf(_text,"Connecting to %s",ssid);
  //_length=strlen(_text);
  // Connect to WiFi network
  //WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
  //  displayText(_text);
  //  delay(_animDelay);
  //}

  if (_wifiAvailable) {
    setupServer();
  }
}

void loop() 
{
  displayText(_text);

  if (DEBUG) {
    Clock.printDateTo_YMD(Serial);
    Serial.print(' ');
    Clock.printTimeTo_HMS(Serial);
    Serial.println();
  }

  if (_wifiAvailable) {
    webMessage();
  }
}
