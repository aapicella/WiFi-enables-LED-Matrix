/*----------------------------display---------------------------*/

/* This function is called in loop but 
 *  only does stuff when animimation delay
 *  is met. 
 *  
 *  This will allow loop to do other thing instead
 *  of waiting for a delay to happen.
 *  
 *  Delay=bad programming.
 */
void displayText ( const char *theText)
{
  uint8_t charWidth;
  uint8_t cBuf[8];  // this should be ok for all built-in fonts
  _mx.clear();
  
//  if ( _myTime + _animDelay < millis()) 
//  {
//    _myTime=millis();
    // Draw the text to the current position
    //drawString(theText, _length, x, 0);
    // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background... 
    // Toggle display of the new framebuffer
    //lmd.display();
    // Advance to next coordinate
    //if( --x < _length * -8 )
    //{
    //  x = LEDMATRIX_WIDTH;
    //  lmd.clear();
    //}
    while (*theText != '\0')
    {
      charWidth = _mx.getChar(*theText++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
    
      for (uint8_t i=0; i<=charWidth; i++)  // allow space between characters
      {
        _mx.transform(MD_MAX72XX::TSL);
        if (i < charWidth)
          _mx.setColumn(0, cBuf[i]);
        delay(_animDelay);
      }
    }
//  }
}
