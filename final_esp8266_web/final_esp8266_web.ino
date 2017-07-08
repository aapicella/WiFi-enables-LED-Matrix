/*
 * 
 * Serial print messages to arduino.
 * Start web server on port 80
 * First send IP untill the web page is access
 * then send the information posted in web page.
 * 
 * Have fun!!
 */
#include <ESP8266WiFi.h>

const char* ssid = "Put your SSID here";
const char* password = "Put your wifi password here";
bool sentText=false;
long timer1=millis();
int interval=10000;
String answer;


// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

void setup(void)
{  
  Serial.begin(115200);
  
  // Connect to WiFi network
  WiFi.begin(ssid, password);

  Serial.println("STARTING WIFI");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("CONNECTED");
  delay(500);    
  // Start TCP (HTTP) server
  server.begin();  
  Serial.println(WiFi.localIP());
}

void loop(void)
{
    // This block is to resend the information.
    // every interval ms.  If the sentText is false send the IP again
    // This protects against the arduino being reset.
     if(timer1 + interval < millis()) {
        timer1=millis();
        if(sentText){
          Serial.println(answer);
        } else {
          Serial.println(WiFi.localIP());
        }
     }  
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    // Wait for data from client to become available
    while(client.connected() && !client.available()){
      delay(1);
      
      // This block is to resend the information.
      // every interval ms.  If the sentText is false send the IP again
      // This protects against the arduino being reset.
      if(timer1 + interval < millis()) {
        timer1=millis();
        if(sentText){
          Serial.println(answer);
        } else {
          //Serial.println("nothing entered yet");
          Serial.println(WiFi.localIP());
        }
      }
    }
  
  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');
  
  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  client.flush();
  String s;
  //if the web page has a ? in it then its a post with the text.
  if (req.indexOf('?')>0) {
    s="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>";
    s+="<br>Sending...";
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
    s+=answer;
    s+=" to the 7219 Led matrix!";
    s+"</body></html>\r\n\r\n";
    client.print(s);
    Serial.println(answer);
    //set the sentText as true if the user entered wed information.
    sentText=true;
    return;
  // Accept any requests as valid and give the form to enter text.
  // unless there is a ? in the url.  
  } else {
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s+="<!doctype html> <style> \#header{ min-height: 20px; background-color: \#666699; } \#menu{ min-height: 20px; margin-top: 1%; background-color: \#999999; } \#body{ min-height: 200px; margin-top: 1%; } \#footer{ min-height: 20px; margin-top: 1%; background-color: \#666699; } \#header, \#menu, \#body, \#footer{ margin-left: 10%; margin-right: 10%; box-shadow: 3px 5px 7px \#666666; border: 1px solid black; } @viewport{ zoom: 1.0; width: extend-to-zoom; } @-ms-viewport{ width: extend-to-zoom; zoom: 1.0; } </style> <html lang='en'> <head> <meta name='viewport' content='width=device-width, initial-scale=1'> <title>ESP8266 Web Server</title> </head> <body> <div id='header'><center><h1>Welcome to the ESP8266 Web Server</H1></center></div> <div id='menu'><center><H2>Enter text which I will sent to the arduino and will be scrolled on a 4- 8x8 led matrix</h2></center></div> <div id='body'><center><div><div><br> <form action='esp8266'> <br>Enter Text to display on LED Matrix:<br><input type='text' maxlength='52' size='52' name='max' value=''><br><br> <input type='submit' value='Submit'></form> </div></div> </center> </div> <div id='footer'> </div> </body>";
    s += "</html>\r\n\r\n";
    client.print(s);
    return;
  }
}

