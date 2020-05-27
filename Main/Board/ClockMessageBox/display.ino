/*----------------------------display---------------------------*/
void setupDisplay() 
{
  _p.begin();                             // Initialise the display
  _p.setIntensity(_intensity);                      
  _p.setInvert(false);
  _p.displaySuspend(false);
  //_p.setSpeed(_frameDelay);
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
      _p.setTextAlignment(_textAlign[1]); // align left
    } else {
      _p.setTextEffect(_effect[0], PA_NO_EFFECT);     // print
      _p.setTextAlignment(_textAlign[0]); // align center
    }
    
    _p.displayReset();
  }
}
