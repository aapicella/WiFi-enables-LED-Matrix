/*----------------------------wifi------------------------------*/
void setupWifiManager() 
{
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  //WiFiManager wm;
  //AsyncWiFiManager wm;
  AsyncWiFiManager wm(&_webServer,&dns);  //ESPAsyncWiFiManager
  
//  wm.resetSettings();                         // reset settings - wipe credentials for testing
  
  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  if (!DEBUG) { wm.setDebugOutput(false); }  // Debug is enabled by default. This will turn off Wifi Manager debug output
  
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect(_apName, _apPassword); // password protected ap
  
  if(!res) {
    _wifiAvailable = false;
    if (DEBUG) { Serial.println("Failed to connect to router."); }
    // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi   
    _wifiAvailable = true; 
    if (DEBUG) { Serial.println("connected to wifi router.)"); }
  }
}

void setupServer() 
{
  // Start file system
  if(LittleFS.begin())
  {
    if (DEBUG) { 
      Dir dir = LittleFS.openDir("/");
      while (dir.next()) {
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
      }
      Serial.printf("\n");
    }
  }
  else { if (DEBUG) { Serial.println("An Error has occurred while mounting LittleFS"); } }
  
  // Start TCP (HTTP) server
  webServerGetPostSetup();
  _webServer.serveStatic("/", LittleFS, "/", "max-age=86400");
  _webServer.begin();
  
  _ip = WiFi.localIP();
  String ipStr = "Connected on " + String(_ip[0]) + '.' + String(_ip[1]) + '.' + String(_ip[2]) + '.' + String(_ip[3]) + " ...";
  if (DEBUG) { Serial.print("Server started on "); Serial.println(ipStr);}
  strcpy(_text, ipStr.c_str());
  _length = strlen(_text);
}

void handleRoot(AsyncWebServerRequest *request) { 
  request->send(200, "text/plain", "Hello world");  
}
void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void webServerGetPostSetup() 
{
  _webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html");
  });

  // Send a POST request to ESP8266
  _webServer.on("/submitMessage", HTTP_POST, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;

    if (request->hasParam("msgString", true)) {
      inputMessage = request->getParam("msgString", true)->value();
      inputParam = "msgString";
      receiveMessage(inputMessage);
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    
    if (DEBUG) { Serial.println(inputMessage); }
    /*
    request->send(200, "text/html", "HTTP POST request sent to your ESP8266 on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
                                     */
    request->send(LittleFS, "/sent.html");                             
  });
  
  _webServer.on("/generate_204", handleRoot);    // Android captive portal.
  _webServer.on("/fwlink", handleRoot);          // Microsoft captive portal.
  _webServer.onNotFound(handleNotFound);
}
