#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

String openWeatherMapApiKey = "9e6f202bdc986d0226ba7aa0aad40255";

String zip = "76227";
String city = "Celina";
String countryCode = "US";
String units = "imperial";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 600000;

auto temp = 0;
auto humidity = 0;
char weather[12];

String jsonBuffer;

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void getWeather() {
  String serverPath = "http://api.openweathermap.org/data/2.5/weather?zip=" + zip + "," + countryCode + "&units=" + units + "&appid=" + openWeatherMapApiKey;

  int localHumidity = dht.readHumidity();
  int localTemp = dht.readTemperature(true);
  
  //if ((millis() - lastTime) > timerDelay)
  if (lastTime == 0) {
    
    jsonBuffer = httpGETRequest(serverPath.c_str());
    Serial.println(jsonBuffer);
    DynamicJsonDocument myObject(1024);
    deserializeJson(myObject, jsonBuffer);

    if(myObject["main"]["temp"] > 0) {
      temp = myObject["main"]["temp"].as<int>();  
    }
    if(myObject["main"]["humidity"]) {
      humidity = myObject["main"]["humidity"].as<int>();  
    }
    
    auto pressure = myObject["main"]["pressure"].as<int>();
    auto windSpeed = myObject["main"]["speed"].as<int>();
 
    /*
    Serial.print("JSON object = ");
    Serial.println(myObject);*/
    Serial.print("Temperature: ");
    Serial.println(temp);
    Serial.print("Pressure: ");
    Serial.println(pressure);
    Serial.print("Humidity: ");
    Serial.println(humidity);
    Serial.print("Wind Speed: ");
    Serial.println(windSpeed);
  
    lastTime = millis();

  }

  if ((millis() - lastTime) > timerDelay) {
    lastTime = 0;
  }
  
  sprintf(weather, "%02d/%02dF %02d%/%02d%%", temp, localTemp, humidity, localHumidity);
  displayText(weather, 2/4, 1, 0);
}
