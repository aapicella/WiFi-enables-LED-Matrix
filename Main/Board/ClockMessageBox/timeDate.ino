/*----------------------------timeDate--------------------------*/

/*
 * Update time and date.
 * Spins up an NTP client, connects to world clock and sets the current time.
 * Called during setup if WIFI is available.
 */
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
  
  setTime(_timeClient.getHours(), _timeClient.getMinutes(), _timeClient.getSeconds(), _timeClient.getDay(), currentMonth, yr);
  
  if (DEBUG) {
    Serial.print("The time has been set to: ");
    Serial.print(hour());
    printDigits(minute());
    printDigits(second());
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

/*
 * Save time.
 * Takes in the time and saves to RTC.
 */
void saveTime(int d, int mo, int yr, int hr, int m, int s)
{  
  tmElements_t timeStamp;                 // Create a variable to hold the data 

  // Load it with the date and time you want to set
  timeStamp.Day    = d;
  timeStamp.Month  = mo;
  timeStamp.Year   = yr; 
  timeStamp.Hour   = hr;
  timeStamp.Minute = m;
  timeStamp.Second = s;
    
  RTC.write(timeStamp);                   // Then write it to the clock
}
