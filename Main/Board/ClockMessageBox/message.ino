/*----------------------------message---------------------------*/

/*
 * Message received.
 */
void receiveMessage(String message)
{
  // Copy received message to _text
  strcpy(_text, message.c_str());
  _length=strlen(_text);

  // Set timeout for when message clears and reverts back to being a clock.
  tmElements_t timeStamp;               // Create a variable to hold the data 
  timeStamp = GetTime();                // Get the time
  _msgTimeoutNextHr = timeStamp.Hour + _msgTimeoutHr; // The hour at which to revert
  _msgTimeoutNextMin = timeStamp.Minute;            // The minute at which to revert

  _p.displayClear();
  _msgActive = true;
  _showIpActive = false;                // Just in case
  
  if (DEBUG) { Serial.println("Message active"); Serial.println(_text); }
}

/*
 * Cancel the message.
 * Called from checkMsgCancelBt and setDisplayText.
 */
void cancelMessage() 
{
  _p.displayClear();
  _msgActive = false;                 // Cancel the message
  setBtLock();
}
