
//
// Wifi Clock
// Adapted from HackerBox 0065 Clock Demo for 64x32 LED Array and SmartMatrix example file
//
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

#include "RTClib.h"
#include "colors.h"
#include "matrix_config.h"
#include "wifi_config.h"
#include "temperature_sensor.h"
#include "weather.h"
#include "fonts.h"
#include "ota.h"

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char monthsOfTheYr[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JLY", "AUG", "SPT", "OCT", "NOV", "DEC"};

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -6 * 3600;
const int   daylightOffset_sec = 3600;
unsigned long mainTimerLastTime = 0;
unsigned long mainTimerDelay = 500;

void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.begin(57600);
  dht.begin();
  delay(1000);
  Serial.println("\n Starting");
  Serial.println("DS1307RTC Test");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  pinMode(TRIGGER_PIN, INPUT);

  

  if(!setupWifiManager()) {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi 
    Serial.println("WiFi connected.");
    EEPROM.begin(512);
    setupOTA();

    if (!rtc.isrunning()) {
      Serial.println("RTC is NOT running, let's set the time!");
      
      // Get time from server
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      Serial.println("Getting time");
      while (!time(nullptr)) {
        Serial.print(".");
        delay(1000);
      }
      
      struct tm timeinfo;
      if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
      } else if(getLocalTime(&timeinfo)) {
      
        Serial.println("Time obtained from server");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        Serial.print("Day of week: ");
        Serial.println(&timeinfo, "%A");
        Serial.print("Month: ");
        Serial.println(&timeinfo, "%B");
        Serial.print("Day of Month: ");
        Serial.println(&timeinfo, "%d");
        Serial.print("Year: ");
        Serial.println(&timeinfo, "%Y");
        Serial.println(timeinfo.tm_year);
        Serial.print("Hour: ");
        Serial.println(&timeinfo, "%H");
        Serial.print("Hour (12 hour format): ");
        Serial.println(&timeinfo, "%I");
        Serial.print("Minute: ");
        Serial.println(&timeinfo, "%M");
        Serial.print("Second: ");
        Serial.println(&timeinfo, "%S");
        
        // When the datetime is retrieved from the server configure the rtc time
        rtc.adjust(DateTime(timeinfo.tm_year, timeinfo.tm_mon+1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));
      }
    }

  }

  setupMatrix();
}

void loop() {

  if (mainTimerLastTime == 0) {
    char txtBuffer[12];
    checkButton();
    DateTime now = rtc.now();
  
    matrix->clear();
    
    sprintf(txtBuffer, "%02d:%02d", now.hour(), now.minute());
    displayText(txtBuffer, 2, 3, 9);
    sprintf(txtBuffer, "%02d %s %04d", now.day(), monthsOfTheYr[(now.month()-1)], now.year()-100);
    displayText(txtBuffer, 1, 0, 25);
    
    if(WiFi.status() != WL_CONNECTED) {
      getLocalTemperature();
    } else {
      handleOTA();
      matrix->drawRect(61,1, 2,2, LED_GREEN_MEDIUM);
      getWeather();
    }
  
    matrix->show();
  }

  if ((millis() - mainTimerLastTime) > mainTimerDelay) {
    mainTimerLastTime = 0;
  }
}
