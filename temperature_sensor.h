// Example testing sketch for various DHT humidity/temperature sensors written by ladyada
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

#define DHTPIN 3     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

char localTempWeather[12];
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
unsigned long temperatureSensorLastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long temperatureSensorTimerDelay = 10000;

void getLocalTemperature() {
  if (temperatureSensorLastTime == 0) {
    
    int localHumidity = dht.readHumidity();
    int localTemp = dht.readTemperature(true);
  
    temperatureSensorLastTime = millis();

    sprintf(localTempWeather, "%02dF %02d%%", localTemp, localHumidity);
    displayText(localTempWeather, 3/4, 1, 0);

  }

  if ((millis() - temperatureSensorLastTime) > temperatureSensorTimerDelay) {
    temperatureSensorLastTime = 0;
  }
}
