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
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <Time.h>
#include <TimeLib.h>                      // https://github.com/PaulStoffregen/Time
#include <DS1307RTC.h>                    // https://github.com/PaulStoffregen/DS1307RTC
#include <Timezone.h>                     // https://github.com/JChristensen/Timezone

// UK Time Zone (London)
// British Summer Time (BST)
// https://en.wikipedia.org/wiki/British_Summer_Time
TimeChangeRule myDST = {"DST", Last, Sun, Mar, 1, 0}; // Daylight savings time = UTC minus 1 hour
TimeChangeRule myUTC = {"UTC", Last, Sun, Oct, 1, 0}; // Standard time = UTC
Timezone myTimeZone(myDST, myUTC);
TimeChangeRule *timeChangeRule;           // Pointer to the time change rule, use to get TZ abbrev

/*----------------------------system----------------------------*/
const String _progName = "ClockMessageBox";
const String _progVers = "0.83";          // fixed tmElements_t timeStamp does not name type - added Time.h include AND moved GetTime so stays in scope.
#define DEBUG 1                           // 0 or 1 - remove later
#define DEBUG_TIME 0                      // 0 or 1 - remove later

/*----------------------------pins------------------------------*/
// Max7219 to Wemos D1 Mini Pro (SPI - Serial) - 5V
#define DATA_PIN  13                      // DIN to D7 (MOSI - GPIO 13) orange
#define CS_PIN    12                      // CS  to D6 (MISO - GPIO 12) yellow
#define CLK_PIN   14                      // CLK to D5 (SCLK - GPIO 14) green
// DS3231 Clock to Wemos D1 Mini Pro (I2C) - 5V
// ground = orange
// +3.3v = red
#define SCL_PIN   5                       // SCL to D1 (GPIO5) green
#define SDA_PIN   4                       // SDA to D2 (GPIO4) yellow
// button
// D3 and D4 have internal 10k pullups. Connect straight to ground. Set INPUT_PULLUP ???
//#define BT_PIN    0                       // BT  to D3 (GPIO0) - damn, if pulled low at start then boot will fail !!!
// probably D0 (GPIO16) with an external pullup 10k?
#define BT_PIN    16                      // BT to D0 (GPIO16) with external 10K pullup / touch bt is active low
// D4 (GPIO2)
// can use D0 for WS2812B LED if needed

/*-----------------------------WIFI-----------------------------*/
WiFiServer _server(80);                   // TCP server at port 80 will respond to HTTP requests
IPAddress _ip;
bool _wifiAvailable = false;              // Is wifi available for use?

/*-----------------------------WIFI Manager---------------------*/
const char* _apName = "ClockMessageBox";
const char* _apPassword = "password";
volatile boolean _msgActive = false;      // Is the message currently being shown?
int _msgTimeoutHr = 1;                    // Message timeout in hours - Change this!
//int _msgTimeoutMin = 0;                   // Message timeout in minutes - not in use
int _msgTimeoutNextHr = 0;                // Message timeout saved hour to clear msg 
int _msgTimeoutNextMin = 0;               // Message timeout saved minute to clear msg

/*----------------------------LED Matrix------------------------*/
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW 
#define MAX_DEVICES 5
MD_Parola _p = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7 (4 8x8 blocks)
const int LEDMATRIX_WIDTH = 31;  
//tmElements_t timeStamp = GetTime();                  // Get the time
int x = LEDMATRIX_WIDTH, y=0;             // start top left

//Parola
uint8_t _intensity = 1;                   // 0-15
#define SPEED_TIME  25
#define PAUSE_TIME  1000
uint8_t _frameDelay = 25;                  // default frame delay value
textEffect_t _effect[] = { PA_PRINT, PA_SCROLL_LEFT, };
textPosition_t _textAlign[] = { PA_CENTER, PA_LEFT, }; 

#define BUF_SIZE  512
char _text[BUF_SIZE] = " Hello ";         // Marquee text
int _length = strlen(_text); 
const int _animDelay = 100; //75;         // frameDelay ???

/*----------------------------NTP-------------------------------*/
WiFiUDP _ntpUDP;
NTPClient _timeClient(_ntpUDP, "europe.pool.ntp.org");  // specifically picking europe region
char _daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String _months[12]={"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
int daylightSavings = 1;  // DST flag   ..TEMP 1 - should be 0

/* 
 * Get the time and check and/or convert to daylight savings.
 * Returns a tmElements_t timestamp.
 */
tmElements_t GetTime() 
{
  time_t utc = now();                     // Create a variable to hold the data
  time_t local = myTimeZone.toLocal(utc, &timeChangeRule); // Get the time and check/convert daylight savings
  tmElements_t timeStamp;                 // Create a variable to hold the data 
  // timeStamp.Year
  // timeStamp.Month
  // timeStamp.Day
  // timeStamp.Hour
  // timeStamp.Minute
  // timeStamp.Second
  
  //local = makeTime(timeStamp);            // Convert the tmElements_t to a time_t variable with function makeTime
  breakTime(local, timeStamp);            // Convert back to a tmElements_t with function breakTime
  
  if (DEBUG_TIME) { 
    Serial.print("The time is now: ");
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.println();
  }
  
  return timeStamp;
}
tmElements_t timeStamp = GetTime();                  // Get the time

  
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

  pinMode(BT_PIN, INPUT);                 // Set button pin as input (with external 10K pullup)
  //pinMode(BT_PIN, INPUT_PULLUP);          // Set button pin as input with pullup
  
  setSyncProvider(RTC.get);               // the function to get the time from the RTC
  if (DEBUG) {
    if(timeStatus()!= timeSet) { Serial.println("Unable to sync with the RTC"); }
    else { Serial.println("RTC has set the system time"); }
  }

  setupDisplay();
  setupWifiManager();

  if (_wifiAvailable) 
  {
    updateTimeDate();
    setupServer();
  }
  delay(1);
}

void loop() 
{
  //_msgActive = false;                     // TEMP - remove when button attached
  readTime();                             // contains a check for message cancel button
  
  displayText(_text);

  if (_wifiAvailable) {
    webMessage();
  }
}
