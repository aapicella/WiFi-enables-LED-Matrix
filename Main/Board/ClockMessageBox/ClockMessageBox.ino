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

    WeMos D1 (R2 & mini) (ESP8266), 80 MHz, 115200 baud, 4M (3M)
    Max7219 LED matrix
    DS3231 Clock

    Original project by aapicella.
    https://github.com/aapicella/WiFi-enables-LED-Matrix
    https://www.instructables.com/id/WIFI-Enabled-LED-Matrix/
*/

/*----------------------------libraries-------------------------*/
#include <LittleFS.h>                     // ESP8266 file system
// Captive web portal @ web page @ 192.168.4.1
//#include <WiFiManager.h>                  // https://github.com/tzapu/WiFiManager - clash with ESPAsyncWebServer
//#include <AsyncWiFiManager.h>             // https://github.com/lbussy/AsyncWiFiManager - try this fork
#include <ESPAsyncWiFiManager.h>          // https://github.com/alanswx/ESPAsyncWiFiManager - or this - aagghhhh....
// ok, it works with ESP Async WIFI Manager fork.. now walk away really slowly....
#include <SPI.h>
#include <ESPAsyncWebServer.h>            // https://github.com/me-no-dev/ESPAsyncWebServer
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <MD_Parola.h>                    // https://github.com/MajicDesigns/MD_Parola 
#include <MD_MAX72xx.h>                   // https://github.com/MajicDesigns/MD_MAX72XX
#include <Time.h>
#include <TimeLib.h>                      // https://github.com/PaulStoffregen/Time
#include <DS1307RTC.h>                    // https://github.com/PaulStoffregen/DS1307RTC
#include <Timezone.h>                     // https://github.com/JChristensen/Timezone

/*----------------------------system----------------------------*/
const String _progName = "ClockMessageBox";
const String _progVers = "0.91";          // Implement Async Web Server
#define DEBUG 1                           // 0 or 1 - remove later
#define DEBUG_WIFI 0                      // 0 or 1 - remove later
#define DEBUG_DISPLAY 0                   // 0 or 1 - remove later
#define DEBUG_TIME 0                      // 0 or 1 - remove later
#define DEBUG_BT 0                        // 0 or 1 - remove later

/*----------------------------pins------------------------------*/
// Max7219 to Wemos D1 Mini (SPI - Serial) - 5V
#define DATA_PIN  13                      // DIN to D7 (MOSI - GPIO 13) orange
#define CS_PIN    12                      // CS  to D6 (MISO - GPIO 12) yellow
#define CLK_PIN   14                      // CLK to D5 (SCLK - GPIO 14) green
// DS3231 Clock to Wemos D1 Mini (I2C) - 5V
// ground = orange
// +3.3v = red
#define SCL_PIN   5                       // SCL to D1 (GPIO5) green
#define SDA_PIN   4                       // SDA to D2 (GPIO4) yellow
// button
#define BT_PIN    16                      // BT to D0 (GPIO16) with external 10K pullup / touch bt is active low

/*-----------------------------WIFI-----------------------------*/
AsyncWebServer _webServer(80);            // TCP server at port 80 will respond to HTTP requests
DNSServer dns;                            // ??? notactually used, but needed for ESP Async WIFI manager setup
IPAddress _ip;
bool _wifiAvailable = false;              // Is wifi available for use?
const char* _apName = "ClockMessageBox";
const char* _apPassword = "password";

/*-----------------------------Show message and IP--------------*/
volatile boolean _msgActive = false;      // Is the message currently being shown?
int _msgTimeoutHr = 1;                    // Message timeout in hours - Change this!
//int _msgTimeoutMin = 0;                   // Message timeout in minutes - not in use
int _msgTimeoutNextHr = 0;                // Message timeout saved hour to clear msg 
int _msgTimeoutNextMin = 0;               // Message timeout saved minute to clear msg

volatile boolean _showIpActive = false;   // Is the IP currently being shown?
unsigned long _showIpSaveTime = 0;        // Save the current time
long _showIpDisplayInterval = 10000;      // Amount of time to display the IP in milliseconds

volatile boolean _btLock = false;         // Button lock
unsigned long _btLockSaveTime = 0;        // Save the current time
long _btLockInterval = 1000;              // Amount of time to lock the button in milliseconds

/*----------------------------LED Matrix------------------------*/
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW 
#define MAX_DEVICES 5
MD_Parola _p = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

uint8_t _intensity = 1;                   // 0-15
#define SPEED_TIME  50                    // 25 - higher is slower. Change to variable int later.
#define PAUSE_TIME  1000
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

/*----------------------------daylight savings------------------*/
// UK Time Zone (London)
// British Summer Time (BST)
// https://en.wikipedia.org/wiki/British_Summer_Time
TimeChangeRule myDST = {"DST", Last, Sun, Mar, 1, 60}; // Daylight savings time = UTC minus 1 hour
TimeChangeRule myUTC = {"UTC", Last, Sun, Oct, 1, 0}; // Standard time = UTC
Timezone myTimeZone(myDST, myUTC);
TimeChangeRule *timeChangeRule;           // Pointer to the time change rule, use to get TZ abbrev

/* 
 * Get the time and check and/or convert to daylight savings.
 * Returns a tmElements_t timestamp.
 * Placed here so that it stays in scope for tmElements_t timeStamp.
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

tmElements_t timeStamp = GetTime();       // Timestamp to get the time


/*----------------------------main------------------------------*/
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
  checkShowIpBt();
  
  setDisplayText();                             // Contains a check for message cancel button
  
  displayText(_text);

  checkBtLock();
}
