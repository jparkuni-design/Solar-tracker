//lcd setup
#include <LiquidCrystal.h>
int rs=2;
int en=3;
int d4=4;
int d5=5;
int d6=6;
int d7=7;
String text1="Humidity:";
String text2="Temp(C):";
String text3="Temp(F):";
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//servo setup
#include <Servo.h>
Servo myServo;
int servox=A5;
int anglex;
int servoy=A4;
int angley;

//Overall delaytime
int dt=300;

//DHT-11 setup
#include "DHT.h"
#define Type DHT11
int sensepin=8;
DHT HT(sensePin,Type);
float humidity;
float tempC;
float tempF;

//Solar panel setup
float fluxval;
string text4="Solar Flux:"


//initial on program
void setup() {
  Serial.begin(9600);
  pinMode(servox,INPUT);
  pinMode(servoy,INPUT);
  HT.begin();
  delay(dt);
  lcd.begin(20,4);
}

//infinite loop program
void loop() {

//DHT-11 loop
humidity=HT.readHumidity();
tempC=HT.readTemperature();
tempF=HT.readTemperature(true);

//lcd display loop program
  lcd.setCursor(0,0);
  lcd.print(text1);
  lcd.setCursor(11,0);
  lcd.print(humidity);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print(text2);
  lcd.setCursor(10,1);
  lcd.print(tempC);
  lcd.print("C");
  lcd.setCursor(0,2);
  lcd.print(text3);
  lcd.setCursor(10,2);
  lcd.print(tempF);
  lcd.print("F");
  lcd.setCursor(0,3);
  lcd.print(text4);
  lcd.setCursor(13,3);
  lcd.print(fluxval);
  lcd.print("%"); 

//









































}
