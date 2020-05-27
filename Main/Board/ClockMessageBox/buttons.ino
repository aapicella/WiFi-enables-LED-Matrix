/*----------------------------buttons---------------------------*/
/*
 * Check message cancel button.
 * Touch button.
 */
void checkMsgCancelBt() 
{
  int val = digitalRead(BT_PIN);          // Read the input button pin / touch bt is active low
  if (val == 0) {
    cancelMessage();
    if (DEBUG) { Serial.println("Message canceled by button"); }
  }
}

/*
 * Check show IP button.
 * Uses same touch button as message cancel.
 */
void checkShowIpBt() 
{
  unsigned long currentTime = millis();     // Get the current time
  
  // Only check if we are not currently diplaying a message and Show IP is not active.
  if (_msgActive == false && _showIpActive == false && _btLock == false) {
    int val = digitalRead(BT_PIN);          // Read the input button pin / touch bt is active low
    if (val == 0) {
      _p.displayClear();
      _showIpActive = true;                 // Show the IP for X seconds
      _showIpSaveTime = currentTime;        // Save the current time
      if (DEBUG) { Serial.println("IP displayed by button"); }
    }
  } else if (_showIpActive == true) {
    if (currentTime - _showIpSaveTime > _showIpDisplayInterval) {
      _p.displayClear();
      _showIpActive = false;                // Cancel show the IP
      if (DEBUG) { Serial.println("IP display canceled"); }
    }
  }
}

/*
 * Issue: When button is pressed to cancel the button stays low for a bit so the Show IP triggers straight away.
 * Resolve: Put in a button lock for X time.
 */
/*
 * Set button lock.
 * Called from checkMsgCancelBt and readTime.
 */
void setBtLock() 
{
    unsigned long currentTime = millis(); // Get the current time
    _btLockSaveTime = currentTime;        // Save the current time
    _btLock = true;                       // Lock the button
    if (DEBUG) { Serial.println("Button lock set"); }
}

/*
 * Check button lock.
 * Called from end of main loop.
 */
void checkBtLock() 
{
  if (_btLock) {
    unsigned long currentTime = millis(); // Get the current time
    if (currentTime - _btLockSaveTime > _btLockInterval) {
      _btLock = false;                    // Cancel button lock
      if (DEBUG) { Serial.println("Button lock canceled"); }
    }
  }
}
