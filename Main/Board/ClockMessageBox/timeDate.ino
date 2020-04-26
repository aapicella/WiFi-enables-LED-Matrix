/*----------------------------timeDate--------------------------*/

void updateTimeDate() 
{  
  _timeClient.begin();                  // Initialise the NTP client
  _timeClient.setTimeOffset(0);
  _timeClient.update();

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
  int yr = currentYear-2000;
  
  saveTime(_timeClient.getDay(), currentMonth, yr, _timeClient.getHours(), _timeClient.getMinutes(), _timeClient.getSeconds());
  
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

void saveTime(int d, int mo, int yr, int hr, int m, int s)
{
  // Create a variable to hold the data 
  DateTime MyTimestamp;

  // Load it with the date and time you want to set, for example
  //   Saturday the 3rd of October 2020 at 14:17 and 33 Seconds...
  MyTimestamp.Day    = d;
  MyTimestamp.Month  = mo;
  MyTimestamp.Year   = yr; 
  MyTimestamp.Hour   = hr;
  MyTimestamp.Minute = m;
  MyTimestamp.Second = s;
  
  // Then write it to the clock
  _clock.write(MyTimestamp);
}

void readTime() 
{
  // Create a variable to hold the data 
  DateTime MyTimestamp;

  // Ask the clock for the data.
  MyTimestamp = _clock.read();

//  checkDst(MyTimestamp);

  int hr = MyTimestamp.Hour;
  if (daylightSavings == 1) {
    hr = hr + 1;
  }

  String timeString = String(hr) + ":" + String(MyTimestamp.Minute) + ":" + String(MyTimestamp.Second);
  strcpy(_text, timeString.c_str());
  _length=strlen(_text);

  if (DEBUG) { Serial.println(timeString); }
}
