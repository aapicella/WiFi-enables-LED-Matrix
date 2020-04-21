/*
 * Original project by aapicella.
 * https://github.com/aapicella/WiFi-enables-LED-Matrix
 * https://www.instructables.com/id/WIFI-Enabled-LED-Matrix/
 */

//#include <LEDMatrixDriver.hpp>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

/*----------------------------system----------------------------*/
const String _progName = "";
const String _progVers = "0.2"; // Converting to MD_MAX72XX library

/*-----------------------------WIFI-----------------------------*/
const char* ssid = "";
const char* password = "";
WiFiServer server(80);  // TCP server at port 80 will respond to HTTP requests
IPAddress ip;

/*
 * Max7219 to Wemos D1 Mini Pro
 * DIN to D7 (GPIO 13)
 * CS to D6 (GPIO 12)
 * CLK to D5 (GPIO 14)
 */

//const uint8_t LEDMATRIX_CS_PIN = 12;

// Define LED Matrix dimensions (0-n) - eg: 32x8 = 31x7 (4 8x8 blocks)
const int LEDMATRIX_WIDTH = 31;  
//const int LEDMATRIX_HEIGHT = 7;
//const int LEDMATRIX_SEGMENTS = 4;

// The LEDMatrixDriver class instance
//LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

//#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
#define MAX_DEVICES 2

#define DATA_PIN  13 // DIN to D7 (GPIO 13)
#define CS_PIN    12 // CS to D6 (GPIO 12)
#define CLK_PIN   14 // CLK to D5 (GPIO 14)

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

int x = LEDMATRIX_WIDTH, y=0;   // start top left

// Marquee text
char text[75] = " Hello ";
int len = strlen(text);
// Marquee speed
const int ANIM_DELAY = 100; //75;
unsigned long myTime=millis();
//unsigned long intro=millis();

// We always wait a bit between updates of the display
//#define  DELAYTIME  100  // in milliseconds
  
void setup() 
{
  //Initialize the display
  mx.begin();
  
  sprintf(text,"Connecting to %s",ssid);
  len=strlen(text);
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    displayText(text);
    delay(ANIM_DELAY);
  }
  // Start TCP (HTTP) server
  server.begin();
  ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  strcpy(text, ipStr.c_str());
  len=strlen(text);
}

void loop() 
{
  displayText(text);
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }

  // Wait for data from client to become available
  while(client.connected() && !client.available()) 
  {
    displayText(text);
    delay(1);
  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');
  
  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) 
  {
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  client.flush();
  
  String s;
  String answer;
  int pos;
  if (req.indexOf('?')>0) 
  {
    //Change url tags to text
    answer=req.substring(req.indexOf('=')+1);
    answer.replace('+',' ');
    //Conver HTML URL Encode to Text:
    //https://www.w3schools.com/tags/ref_urlencode.asp
    answer.replace("%21","\!");
    answer.replace("%22","\"");
    answer.replace("%23","\#");
    answer.replace("%24","\$");
    answer.replace("%25","%");
    answer.replace("%26","&");
    answer.replace("%27","\'");
    answer.replace("%28","\(");
    answer.replace("%29","\)");
    answer.replace("%2B","\+");
    answer.replace("%2C","\,");
    answer.replace("%2D","\-");
    answer.replace("%2E","\.");
    answer.replace("%2F","\/");
    answer.replace("%3A","\:");
    answer.replace("%3B","\;");
    answer.replace("%3C","\<");
    answer.replace("%3D","\=");
    answer.replace("%3E","\>");
    answer.replace("%3F","\?");
    answer.replace("%40","\@");
    answer.replace("%5B","\[");
    answer.replace("%5C","\\");
    answer.replace("%5D","\]");
    answer.replace("%5E","\^");
    answer.replace("%7B","\{");
    answer.replace("%7C","\|");
    answer.replace("%7D","\}");

    strcpy(text, answer.c_str());
    len=strlen(text);
    x=LEDMATRIX_WIDTH;
    //lmd.clear();
    mx.clear();

    //Serial.println(req.substring(req.indexOf('=')+1));
    s="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML><style> #header{ min-height: 20px; background-color: #666699; } #menu{ min-height: 20px; margin-top: 1%; background-color: #999999; } #body{ min-height: 200px; margin-top: 1%; } #footer{ min-height: 20px; margin-top: 1%; background-color: #666699; } #header, #menu, #body, #footer{ margin-left: 10%; margin-right: 10%; box-shadow: 3px 5px 7px #666666; border: 1px solid black; } @viewport{ zoom: 1.0; width: extend-to-zoom; } @-ms-viewport{ width: extend-to-zoom; zoom: 1.0; } </style> <html lang='en'> <head> <meta name='viewport' content='width=device-width, initial-scale=1'> <title>ESP8266 Web Server</title> </head> <body> <div id='header'><center><h1>Welcome to the ESP8266 Web Server</H1></center></div>";
    s+=" <div id='menu'><center><H2>Printing to the 4 - 8x8 Matrix</h2></center></div> ";
    s+="<div id='body'><center><div><div><br/><H3>";
    s+=answer;
    s+="<p><a href='./'>Back</a></p>";
    s+="</H3></div></div> </center> </div> <div id='footer'> </div> </body></html> ";
    //s+=req.substring(req.indexOf('=')+1);
    s+"</body></html>\r\n\r\n";
    client.print(s);
    return;
  }
  else
  {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s+="<!doctype html> <style> \#header{ min-height: 20px; background-color: \#666699; } \#menu{ min-height: 20px; margin-top: 1%; background-color: \#999999; } \#body{ min-height: 200px; margin-top: 1%; } \#footer{ min-height: 20px; margin-top: 1%; background-color: \#666699; } \#header, \#menu, \#body, \#footer{ margin-left: 10%; margin-right: 10%; box-shadow: 3px 5px 7px \#666666; border: 1px solid black; } @viewport{ zoom: 1.0; width: extend-to-zoom; } @-ms-viewport{ width: extend-to-zoom; zoom: 1.0; } </style> <html lang='en'> <head> <meta name='viewport' content='width=device-width, initial-scale=1'> <title>ESP8266 Web Server</title> </head> <body> <div id='header'><center><h1>Welcome to the ESP8266 Web Server</H1></center></div> <div id='menu'><center><H2>Enter text which will be displayed on a 4 - 8x8 led matrix</h2></center></div> <div id='body'><center><div><div><br> <form action='esp8266'> <br>Enter Text to display on LED Matrix:<br><input type='text' maxlength='70' name='max' value=''><br><br> <input type='submit' value='Submit'></form> </div></div> </center> </div> <div id='footer'> </div> </body>";
    s += "</html>\r\n\r\n";
    client.print(s);
    return;
  }
}
