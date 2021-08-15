#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const String SSID="SSID";
const String WIFI_PASSWORD="password";
const String FIREBASE_URL="firebase_url/";
const String WEATHER_API="http://api.openweathermap.org/data/2.5/weather?appid={APIKEY}&q=";

WiFiClientSecure wifiClientSecure;
WiFiClient wifiClient;
HTTPClient http;

SoftwareSerial s(D7,D6);

void setup()
{
  s.begin(9600);
  Serial.begin(9600);
  connectToWifi();
  wifiClientSecure.setInsecure();
  http.setTimeout(1000);
}
void connectToWifi()
{
  WiFi.begin(SSID,WIFI_PASSWORD);
  while(WiFi.status()!=WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWIFI connected");
}
void loop()
{
  if(s.available()>0)
  {
    double temperature=s.read();
    updateTemperature(temperature);
    String cities=getSelectedCities();
    char tmp[cities.length()];
    cities.toCharArray(tmp, cities.length());
    s.write(tmp);
  }
  delay(2000);
}
void updateTemperature(double temperature)
{
  http.begin(wifiClientSecure,FIREBASE_URL+"temperature.json");
  int httpResponseCode=http.PUT(String(temperature));
  if(httpResponseCode<=0)
  {
    Serial.print("Error on sending PUT Request: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}
String getSelectedCities()
{
  http.begin(wifiClientSecure,FIREBASE_URL+"cities.json"); 
  http.GET();
  String cities=http.getString();
  http.end();
  cities.remove(0,1);
  cities.remove(cities.length()-1,1);
  return splitCities(cities)+" ";
}
String splitCities(String cities)
{
  int index=cities.indexOf(",");
  if(index==-1)
    return cities+":"+getTemperature(cities);
  else
  {
    String city=cities.substring(0,index);
    String tmp=cities.substring(index+1);
    return city+":"+getTemperature(city)+","+splitCities(tmp);
  }
}
String getTemperature(String city)
{
  http.begin(wifiClient,WEATHER_API+city);
  http.GET();
  String response=http.getString();
  http.end();
  DynamicJsonDocument json(1024);
  deserializeJson(json, response);
  int temperature=(int)json["main"]["temp"];
  temperature-=273;
  return String(temperature);
}