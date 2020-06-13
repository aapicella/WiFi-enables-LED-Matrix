/*
    Test program B for ESP Async Web Server.
    
    Wemos D1 Mini (ESP8266)

    Based on project by Rui Santos
    https://randomnerdtutorials.com  
*/

/*----------------------------libraries-------------------------*/
//#include <EEPROM.h>                               // ESP8266 style EEPROM (using 512 bytes) - WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (3M SPIFFS)
//#include "WiFi.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
//#include "SPIFFS.h"
//#include <FS.h>                                   // file server - SPIFFS now depreciated for esp8266, now uses LittleFS
#include <LittleFS.h>
#include <MT_LightControlDefines.h>

/*----------------------------system----------------------------*/
//const char* ssid = TESTING_SSID;
//const char* password = TESTING_PASSWORD;

/*----------------------------web server------------------------*/
AsyncWebServer webServer(80);

const char* MSG_STRING = "msgString";

void handleRoot(AsyncWebServerRequest *request) { 
  request->send(200, "text/plain", "Hello world");  
}
void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void webServerGetPostSetup() 
{
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //request->send(SPIFFS, "/index.htm");
    request->send(LittleFS, "/index.html");
  });

  // Send a GET request to <ESP_IP>/get?input1=<msgString>
  webServer.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    
    // GET msgString value on <ESP_IP>/get?msgString=<inputMessage>
    if (request->hasParam(MSG_STRING)) {
      inputMessage = request->getParam(MSG_STRING)->value();
      inputParam = MSG_STRING;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });

  // Send a POST request to ESP
  webServer.on("/submitMessage", HTTP_POST, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    //if (request->hasParam(MSG_STRING, true)) {
    if (request->hasParam("msgString2", true)) {
      inputMessage = request->getParam("msgString2", true)->value();
      inputParam = "msgString2";
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP POST request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  
  webServer.on("/generate_204", handleRoot);    // Android captive portal.
  webServer.on("/fwlink", handleRoot);          // Microsoft captive portal.
  webServer.onNotFound(handleNotFound);
}

/*----------------------------main------------------------------*/
void setup() 
{
  //EEPROM.begin(512);                              // tell it we are using an EEPROM area of 512 bytes
  
  Serial.begin(115200);

  //if(!SPIFFS.begin()){
  //  Serial.println("An Error has occurred while mounting SPIFFS");
  //  return;
  //}
  //SPIFFS.begin();
  //{
  //  Dir dir = SPIFFS.openDir("/");
  //  while (dir.next()) {
  //    String fileName = dir.fileName();
  //    size_t fileSize = dir.fileSize();
  //    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
  //  }
  //  Serial.printf("\n");
  //}
  //if(!LittleFS.begin()) {
  //  Serial.println("An Error has occurred while mounting LittleFS");
  //  return;
  //}
  if(LittleFS.begin())
  {
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    }
    Serial.printf("\n");
  }
  else {
    Serial.println("An Error has occurred while mounting LittleFS");
  }

  WiFi.begin(TESTING_SSID, TESTING_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  webServerGetPostSetup();
  
  webServer.serveStatic("/", LittleFS, "/", "max-age=86400");
  //webServer.serveStatic("/", SPIFFS, "/index.htm");
  //webServer.serveStatic("/", SPIFFS, "/style.css");
  //webServer.serveStatic("/css", SPIFFS, "/css");
  //webServer.serveStatic("/images", SPIFFS, "/images");
  
  webServer.begin();
}
 
void loop() 
{
  //webServer.handleClient();
}
