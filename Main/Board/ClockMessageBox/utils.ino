/*----------------------------utils-----------------------------*/
/*
 * Print digits.
 * Helper for Debug output.
 */
void printDigits(int digits) 
{
  Serial.print(":");
  if (digits < 10) { Serial.print('0'); }
  Serial.print(digits);
}
