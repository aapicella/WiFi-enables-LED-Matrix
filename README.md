# WiFi-enables-LED-Matrix
Arduino, LED Matrix and ESP8266 project.

Load arduino with file MD_MAX7219_Scroll_Message.ino. 
Connect 7219 display.  
With serial monitor send text and make sure it apprears on display.

Next load board esp8266-12
load file final_esp8266_web.ino
Be sure to change the ssid and password.
Connect via serial monitor to see if it prints connected then IP address.
It will resend every 15 seconds to serial incase arduino is reset.


