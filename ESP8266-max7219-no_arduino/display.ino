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
void displayText ( char * theText)
{
  if ( myTime + ANIM_DELAY < millis()) 
  {
    myTime=millis();
    // Draw the text to the current position
    drawString(theText, len, x, 0);
    // In case you wonder why we don't have to call lmd.clear() in every loop: The font has a opaque (black) background... 
    // Toggle display of the new framebuffer
    lmd.display();
    // Advance to next coordinate
    if( --x < len * -8 )
    {
      x = LEDMATRIX_WIDTH;
      lmd.clear();
    }
  }
}

/**
 * This function draws a string of the given length to the given position.
 */
void drawString(char* text, int len, int x, int y )
{
  for( int idx = 0; idx < len; idx ++ )
  {
    int c = text[idx] - 32;

    // stop if char is outside visible area
    if( x + idx * 8  > LEDMATRIX_WIDTH ) 
    {
      return;
    }

    // only draw if char is visible
    if( 8 + x + idx * 8 > 0 ) 
    {
      drawSprite( font[c], x + idx * 8, y, 8, 8 );
    }
  }
}

/**
 * This draws a sprite to the given position using the width and height supplied (usually 8x8)
 */
void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  // The mask is used to get the column bit from the sprite row
  byte mask = B10000000;

  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      //Yes my font is backwards so I swap it around.
      //lmd.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));
      lmd.setPixel(x + (width - ix), y + iy, (bool)(sprite[iy] & mask ));

      // shift the mask by one pixel to the right
      mask = mask >> 1;
    }

    // reset column mask
    mask = B10000000;
  }
}
