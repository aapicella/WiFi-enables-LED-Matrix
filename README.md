# Clock Message Box
LED Matrix and ESP8266 project.

Hardware used:
- Wemos D1 Mini Pro (ESP8266)
- Max7219 LED matrix
- DS3231 Clock

Libraries used:
- https://github.com/esp8266/Arduino
- https://github.com/MajicDesigns/MD_MAX72XX
- https://github.com/MajicDesigns/MD_Parola
- https://github.com/tzapu/WiFiManager
- https://github.com/PaulStoffregen/Time
- https://github.com/JChristensen/Timezone
- https://github.com/PaulStoffregen/DS1307RTC
- https://github.com/arduino-libraries/NTPClient

Originally based on project by aapicella (mostly replaced now).
- https://github.com/aapicella/WiFi-enables-LED-Matrix
- https://www.instructables.com/id/WIFI-Enabled-LED-Matrix/

Instructions to setup device:  
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

