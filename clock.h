#include "RTClib.h"

RTC_DS1307 rtc;

char monthsOfTheYr[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JLY", "AUG", "SPT", "OCT", "NOV", "DEC"};
char daysOfTheWeek[7][12] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

void displayClock() {
  char txtBuffer[12];
  DateTime now = rtc.now();
  
  sprintf(txtBuffer, "%02d:%02d", now.hour(), now.minute());
  displayText(txtBuffer, 2, 3, 9, LED_WHITE_HIGH);
  sprintf(txtBuffer, "%02d %s %04d", now.day(), monthsOfTheYr[(now.month()-1)], now.year()-100);
  displayText(txtBuffer, 1, 0, 25, LED_GREEN_HIGH);
}
