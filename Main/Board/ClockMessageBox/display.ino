/*----------------------------display---------------------------*/
void setupDisplay() 
{
  _p.begin();                             // Initialise the display
  _p.setIntensity(_intensity);                      
  _p.setInvert(false);
  _p.displaySuspend(false);
  //_p.setSpeed();
  _p.displayClear();

  _p.displayText(_text, _textAlign[0], SPEED_TIME, PAUSE_TIME, _effect[0], PA_NO_EFFECT); // center, print
}

void displayText( const char *theText)
{
  // If message is active then display message, else show the clock.
  if (_p.displayAnimate())              // animates and returns true when an animation is completed
  {
    _p.setTextBuffer(theText);
    
    if (_msgActive || _showIpActive) {
      _p.setTextEffect(_effect[1], PA_NO_EFFECT);     // scroll left
      _p.setTextAlignment(_textAlign[1]);             // align left
    } else {
      _p.setTextEffect(_effect[0], PA_NO_EFFECT);     // print
      _p.setTextAlignment(_textAlign[0]);             // align center
    }
    
    _p.displayReset();
  }
}

/*
 * Set the display text (excluding message).
 * Checks if message is active or not, 
 * if not checks and sets text to IP or current time.
 */
void setDisplayText() 
{
  tmElements_t timeStamp;                 // Create a variable to hold the data 
  timeStamp = GetTime();                  // Get the time
  
  if (_msgActive == true) { 
    if (timeStamp.Hour >= _msgTimeoutNextHr && timeStamp.Minute >= _msgTimeoutNextMin) {
      cancelMessage();
      if (DEBUG_DISPLAY) { Serial.println("Message canceled by timeout"); }
    } else {
      checkMsgCancelBt();
    }
  } else if (_showIpActive == true) {
    String ipStr = " _ " + String(_ip[0]) + '.' + String(_ip[1]) + '.' + String(_ip[2]) + '.' + String(_ip[3]);
    if (DEBUG_DISPLAY) { Serial.print("IP displayed - "); Serial.println(ipStr);}
    strcpy(_text, ipStr.c_str());
    _length=strlen(_text);
  } else {
    String tHr = String(timeStamp.Hour);
    String tMin = String(timeStamp.Minute);
    if (timeStamp.Hour < 10) { tHr = "0" + String(timeStamp.Hour); }
    if (timeStamp.Minute < 10) { tMin = "0" + String(timeStamp.Minute); }
    //String timeString = String(timeStamp.Hour) + ":" + String(timeStamp.Minute) + ":" + String(timeStamp.Second);
    String timeString = tHr + ":" + tMin;
    strcpy(_text, timeString.c_str());
    _length=strlen(_text);
  }

}
