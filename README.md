# Clock Message Box
LED Matrix and ESP8266 project.

### Equipment
#### Hardware
- WeMos D1 (R2 & mini) (ESP8266), 80 MHz, 115200 baud, 4M (3M)
- Max7219 LED matrix
- DS3231 Clock

#### Software
- https://github.com/esp8266/Arduino
- https://github.com/MajicDesigns/MD_MAX72XX
- https://github.com/MajicDesigns/MD_Parola
- https://github.com/tzapu/WiFiManager
- https://github.com/lbussy/AsyncWiFiManager
- https://github.com/alanswx/ESPAsyncWiFiManager
- https://github.com/PaulStoffregen/Time
- https://github.com/JChristensen/Timezone
- https://github.com/PaulStoffregen/DS1307RTC
- https://github.com/arduino-libraries/NTPClient
- https://github.com/me-no-dev/AsyncTCP
- https://github.com/me-no-dev/ESPAsyncWebServer
- https://github.com/esp8266/Arduino/tree/master/libraries/LittleFS

### Licence
- Written by MTS Standish (Thurstan|mattKsp) 2020
- Released under GNU GPLv3 (see LICENCE file)

This software is provided on an "AS-IS-BASIS"

Originally based on project by aapicella (mostly replaced now).
- https://github.com/aapicella/WiFi-enables-LED-Matrix
- https://www.instructables.com/id/WIFI-Enabled-LED-Matrix/

### Instructions to setup device 
- Power on.  
- Connect your phone/tablet/computer to Wifi device "ClockMessageBox".  
- Connection password is "password".  
- When successfull open browser and goto 192.168.4.1   
- Select the Wifi device you want it connected to (probably your home router).  
- Enter the password for that device.  
- Read the instructions on the page to tell if it was successfull or not.   
- If not repeat.   
- When successfull the Clock Message Box will save the connection and (attempt to) automatically connect to this every time it powers on.   
- On power-up the Clock Message Box will display the IP it has been assigned.   
- Open browser and goto that IP to send the box messages.  

- ( To display the IP again touch the front button. )  
- ( To clear a message touch the front button. )  
- ( Message will timeout and revert to clock (currently) after 1 hour. )   

### Reference
https://www.w3schools.com/tags/ref_httpmethods.asp <br>
https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/ <br> 
https://randomnerdtutorials.com/esp8266-web-server-spiffs-nodemcu/ <br> 
https://randomnerdtutorials.com/esp32-esp8266-input-data-html-form/ <br> 
https://forum.arduino.cc/index.php?topic=590802.0 <br> 



