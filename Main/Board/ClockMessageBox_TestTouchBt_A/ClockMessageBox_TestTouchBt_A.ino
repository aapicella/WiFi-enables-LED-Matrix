/*
    Test program A for TTP223 touch button.
    
    Wemos D1 Mini Pro (ESP8266)
*/

/*----------------------------system----------------------------*/
const String _progName = "ClockMessageBox";

/*----------------------------pins------------------------------*/
#define BT_PIN 16                         // BT to D0 (GPIO16) with external 10K pullup

void setup() 
{
  Serial.begin(115200);
  
  Serial.println();
  Serial.println(_progName);
  Serial.print("..");
  Serial.println();

  pinMode(BT_PIN, INPUT);                 // Set button pin as input (with external 10K pullup)

  delay(1);
}

void loop() 
{
  int val = digitalRead(BT_PIN);          // Read the input button pin
  
  if (val == 1) {
    Serial.print("Touch button on pin ");
    Serial.print((int)BT_PIN);
    Serial.println(" is High.");
  } else {
    Serial.print("Touch button on pin ");
    Serial.print((int)BT_PIN);
    Serial.println(" is Low.");
  }
}
