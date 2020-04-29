/*----------------------------display---------------------------*/

void displayText ( const char *theText)
{
  // If message is active then display message, else show the clock.
  if (_p.displayAnimate())              // animates and returns true when an animation is completed
  {
    _p.setTextBuffer(theText);
    
    if (_msgActive) {
      _p.setTextEffect(_effect[1], PA_NO_EFFECT);     // scroll left
      _p.setTextAlignment(_textAlign[1]); // align left
    } else {
      _p.setTextEffect(_effect[0], PA_NO_EFFECT);     // print
      _p.setTextAlignment(_textAlign[0]); // align center
    }
    
    _p.displayReset();
  }
}
