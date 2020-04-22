/*----------------------------timeDate--------------------------*/

/* BST begins at 01:00 GMT on the last Sunday of March and 
 * ends at 01:00 GMT (02:00 BST) on the last Sunday of October.
 * https://en.wikipedia.org/wiki/British_Summer_Time
 * 
 * 2020 29 March 25 October
 * 2021 28 March 31 October
 * 2022 27 March 30 October
 * 2023 26 March 29 October
 */

void updateTimeDate() 
{  
  _timeClient.begin();                  // Initialise the NTP client
  _timeClient.setTimeOffset(0);
  _timeClient.update();

  // Create a variable to hold the data 
  DateTime MyTimestamp;

  unsigned long epochTime = _timeClient.getEpochTime();
  //Get a time structure
  /* http://www.cplusplus.com/reference/ctime/tm/
   * tm_sec: seconds after the minute;
   * tm_min: minutes after the hour;
   * tm_hour: hours since midnight;
   * tm_mday: day of the month;
   * tm_year: years since 1900;
   * tm_wday: days since Sunday;
   * tm_yday: days since January 1;
   * tm_isdst: Daylight Saving Time flag;
   */
  struct tm *ptm = gmtime ((time_t *)&epochTime); 

  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon+1;
  String currentMonthName = _months[currentMonth-1];
  int currentYear = ptm->tm_year+1900;
  
  // Load it with the date and time you want to set, for example
  //   Saturday the 3rd of October 2020 at 14:17 and 33 Seconds...
  MyTimestamp.Day    = _timeClient.getDay();
  MyTimestamp.Month  = currentMonth;
  MyTimestamp.Year   = currentYear-2000; 
  MyTimestamp.Hour   = _timeClient.getHours();
  MyTimestamp.Minute = _timeClient.getMinutes();
  MyTimestamp.Second = _timeClient.getSeconds();
  
  // Then write it to the clock
  _clock.write(MyTimestamp);

  if (DEBUG) {
    // And use it, we will read it back for example...  
    Serial.print("The time has been set to: ");
    _clock.printTo(Serial);
    Serial.println();
    
    Serial.print("Month day: ");
    Serial.println(monthDay);
    Serial.print("Month: ");
    Serial.println(currentMonth);
    Serial.print("Month name: ");
    Serial.println(currentMonthName);
    Serial.print("Year: ");
    Serial.println(currentYear);
    
    //Print complete date:
    String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
    Serial.print("Current date: ");
    Serial.println(currentDate);

    Serial.print(_daysOfTheWeek[_timeClient.getDay()]);
    Serial.print(", ");
    Serial.print(_timeClient.getHours());
    Serial.print(":");
    Serial.print(_timeClient.getMinutes());
    Serial.print(":");
    Serial.println(_timeClient.getSeconds());
  }
}

void readTime() 
{
  // Create a variable to hold the data 
  DateTime MyTimestamp;

  // Ask the clock for the data.
  MyTimestamp = _clock.read();

  String timeString = String(MyTimestamp.Hour) + ":" + String(MyTimestamp.Minute) + ":" + String(MyTimestamp.Second);
  strcpy(_text, timeString.c_str());
  _length=strlen(_text);

  if (DEBUG) { Serial.println(timeString); }
}
