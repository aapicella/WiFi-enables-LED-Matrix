/*
    'ClockMessageBox' by Thurstan. Wifi connected box with LED Matrix showing clock and messages.
    Copyright (C) 2020  MTS Standish (Thurstan|mattKsp)
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be usefull,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    https://github.com/mattThurstan/

    Wemos D1 Mini Pro (ESP8266)
    Max7219 LED matrix
    DS3231 Clock

    Original project by aapicella.
    https://github.com/aapicella/WiFi-enables-LED-Matrix
    https://www.instructables.com/id/WIFI-Enabled-LED-Matrix/
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
#include <NTPClient.h>
#include <WiFiUdp.h>

/*----------------------------system----------------------------*/
const String _progName = "ClockMessageBox";
const String _progVers = "0.7";           // Implement cancel message button
#define DEBUG 1                           // 0 or 1 - remove later

/*----------------------------pins------------------------------*/
// Max7219 to Wemos D1 Mini Pro (SPI - Serial) - 5V
#define DATA_PIN  13                      // DIN to D7 (MOSI - GPIO 13)
#define CS_PIN    12                      // CS  to D6 (MISO - GPIO 12)
#define CLK_PIN   14                      // CLK to D5 (SCLK - GPIO 14)
// DS3231 Clock to Wemos D1 Mini Pro (I2C) - 5V
#define SCL_PIN   5                       // SCL to D1 (GPIO5)
#define SDA_PIN   4                       // SDA to D2 (GPIO4)
// button
// D3 and D4 have internal 10k pullups. Connect straight to ground. Set INPUT_PULLUP ???
#define BT_PIN    0                       // BT  to D3 (GPIO0)
// D4 (GPIO2)
// can use D0 for WS2812B LED if needed

/*-----------------------------WIFI-----------------------------*/
WiFiServer _server(80);                   // TCP server at port 80 will respond to HTTP requests
IPAddress _ip;
bool _wifiAvailable = false;              // Is wifi available for use?

/*-----------------------------WIFI Manager---------------------*/
const char* _apName = "ClockMessageBox";
const char* _apPassword = "password";
volatile boolean msgActive = false;        // Is the message currently being shown?

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
 DS3231_Simple _clock;
 
/*----------------------------NTP-------------------------------*/
WiFiUDP _ntpUDP;
//NTPClient _timeClient(_ntpUDP);
NTPClient _timeClient(_ntpUDP, "europe.pool.ntp.org");  // specifically picking europe region
//NTPClient _timeClient(_ntpUDP, "pool.ntp.org"); // automatically picks sub-region
//NTPClient _timeClient(_ntpUDP, "time.nist.gov");
char _daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String _months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
int daylightSavings = 1;  // DST flag   ..TEMP 1 - should be 0

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
    Serial.print(_progName);
    Serial.print(" v.");
    Serial.print(_progVers);
    Serial.println();
    Serial.print("..");
    Serial.println();
  }

  pinMode(BT_PIN, INPUT);                 // Set button pin as input
  //pinMode(BT_PIN, INPUT_PULLUP);                 // Set button pin as input with pullup
  
  _clock.begin();                         // Initialise the DS3231 clock
  _mx.begin();                            // Initialise the display
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

  if (_wifiAvailable) 
  {
    updateTimeDate();
    setupServer();
  }
}

void loop() 
{
  if (msgActive == true) {
    checkMsgCancelBt();
  } else {
    readTime();         // can i do this every loop ?
  }
  
  displayText(_text);

  if (DEBUG) {
    _clock.printDateTo_YMD(Serial);
    Serial.print(' ');
    _clock.printTimeTo_HMS(Serial);
    Serial.println();
  }

  if (_wifiAvailable) {
    webMessage();
  }
}
