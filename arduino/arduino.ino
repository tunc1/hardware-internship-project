#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial s(6,7);

void setup()
{
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("Hello World");
  s.begin(9600);
  double temperature=analogRead(A5)*500/1024;
  s.write(temperature);
}
void loop()
{
  if(s.available()>0)
  {
    String cities=s.readString();
    splitCities(cities);
    double temperature=analogRead(A5)*500/1024;
    s.write(temperature);
  }
}
void showCity(String city)
{
  city+="C";
  city.toUpperCase();
  lcd.clear();
  lcd.print(city);
}
void splitCities(String cities)
{
  int index=cities.indexOf(",");
  if(index==-1)
  {
    showCity(cities);
    delay(1000);
  }
  else
  {
    String city=cities.substring(0,index);
    String tmp=cities.substring(index+1);
    showCity(city);
    delay(2000);
    splitCities(tmp);
  }
}
