/*----------------------------daylight savings------------------*/

/* 
 *  BST begins at 01:00 GMT on the last Sunday of March and 
 * ends at 01:00 GMT (02:00 BST) on the last Sunday of October.
 * https://en.wikipedia.org/wiki/British_Summer_Time
 * 
 * 2020 29 March 25 October
 * 2021 28 March 31 October
 * 2022 27 March 30 October
 * 2023 26 March 29 October
 */

/* 
 * https://www.instructables.com/id/The-Arduino-and-Daylight-Saving-Time-Europe/
 * 
 * dow = day of week (as in 1-7)
 * mo = month
 * d = day (as in 1-31)
 * h = hour
 */

/* 
 * https://forum.arduino.cc/index.php?topic=443112.0
 * Calculate day of week in proleptic Gregorian calendar. Sunday == 0. 
 */
int dayOfWeek(int yr, int m, int d)
{
  int adjustment, mm, yy;
  //if (yr<2000) yr+=2000;
  adjustment = (14 - m) / 12;
  mm = m + 12 * adjustment - 2;
  yy = yr - adjustment;
  return (d + (13 * mm - 1) / 5 + yy + yy / 4 - yy / 100 + yy / 400) % 7;
}

void checkDst(DateTime MyTimestamp) 
{
  int mo = MyTimestamp.Month; 
  int d = MyTimestamp.Day; 
  int h = MyTimestamp.Hour;
  int dow = dayOfWeek(MyTimestamp.Year, mo, d);
  
  // check start of daylight savings (last sunday in march, goes forward 1 hr at 1 am)
  if (dow == 7 && mo == 3 && d >= 25 && h == 1 && daylightSavings == 0)
  {
    // set clock to 2 am (forward 1 hr)
    daylightSavings = 1;
  }
  
  // check end of daylight savings (last sunday in october, goes back 1 hr at 2 am) 
  if (dow == 7 && mo == 10 && d >= 25 && h == 2 && daylightSavings == 1)
  {
    // set clock to 1 am (back 1 hr)
    daylightSavings = 0;
  }

}
