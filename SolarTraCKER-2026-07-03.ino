//lcd setup (I2C)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
int j;
String text1="Humidity:";
String text2="Temp(C):";
String text3="Temp(F):";
String text4="Data loading...";

//servo setup
#include <Servo.h>
Servo Yservo;
Servo Xservo;
int xpin=10;
int ypin=9;
int anglex=90;
int angley=90;

//Overall delaytime
int dt=300;

//DHT-11 setup
#include "DHT.h"
#define Type DHT11
int sensePin=8;
DHT HT(sensePin,Type);
float humidity;
float tempC;
float tempF;

//Solar panel setup (INA219)
#include <Wire.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219;
float panelVoltage;
float panelCurrent;
float panelPower;
float totalEnergy_Wh=0;
String text5="Solar Flux:";

//NWSE Photoresioresistors setup
int ps1=A3;
int ps2=A2;
int ps3=A1;
int ps4=A0;
int northval;
int westval;
int southval;
int eastval;
int treshold=20;

//Display toggle switch setup 
int switchpin=A6;
int lastSwitchState=HIGH;

//SD card setup
#include <SPI.h>
#include <SD.h>
int csPin=2;
File dataFile;

//initial on program
void setup() {
  //initial setup init
  Serial.begin(9600);
  Xservo.attach(xpin);
  Yservo.attach(ypin);
  HT.begin();
  ina219.begin();
  lcd.init();
  lcd.backlight();

  //SD card init
  if (!SD.begin(csPin)) {
    lcd.setCursor(0,0);
    lcd.print("SD card failed!");
  }
  delay(dt);
}

//Loop
void loop() {

//Photoresistors loop
northval=analogRead(ps1);
westval=analogRead(ps2);
southval=analogRead(ps3);
eastval=analogRead(ps4);

//DHT-11 loop
humidity=HT.readHumidity();
tempC=HT.readTemperature();
tempF=HT.readTemperature(true);

//Solar panel loop (INA219)
panelVoltage=ina219.getBusVoltage_V();
panelCurrent=ina219.getCurrent_mA()/1000.0;
panelPower=ina219.getPower_mW()/1000.0;
totalEnergy_Wh =totalEnergy_Wh + panelPower * (dt/3600000.0);

//Toggle Switch operation (alter)
int switchRaw = analogRead(switchpin);
int currentSwitchState = (switchRaw > 512) ? HIGH : LOW;

//Lcd loading page operation
if (currentSwitchState == LOW && lastSwitchState == HIGH) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text4);
  lcd.setCursor(16,0);
  for (j=3; j>=1; j=j-1) {
    lcd.print(j);
    delay(1000);
    lcd.clear();
  }
}

lastSwitchState = currentSwitchState;

//lcd display ON data display (Humidity, TempC, and TempF) operation
if (currentSwitchState == LOW) {
  lcd.backlight();

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
  lcd.print(text5);
  lcd.setCursor(13,3);
  lcd.print(panelPower);
  lcd.print("W"); 
}

//lcd display OFF data display (Humidity, TempC, and TempF) operation
else {
  lcd.noBacklight();
}

//Automated motor adjustment operation
if (westval - eastval > treshold) {
  anglex= anglex-3;
}

else if (eastval - westval > treshold) {
  anglex= anglex+3;
}

if (northval - southval > treshold) {
  angley=angley+3;
}

else if (southval - northval > treshold) {
  angley= angley-3;
}

anglex = constrain(anglex, 0, 180);
angley = constrain(angley, 0, 180);

//Moving Servo loop
Xservo.write(anglex);
Yservo.write(angley);

//Data logging loop print: (Serial + SD, same data)
String logLine = String(millis()) + "," + String(humidity) + "," + String(tempC) + "," + String(tempF) + "," + String(northval) + "," + String(southval) + "," + String(eastval) + "," + String(westval) + "," + String(anglex) + "," + String(angley) + "," + String(panelVoltage) + "," + String(panelCurrent) + "," + String(panelPower) + "," + String(totalEnergy_Wh);

Serial.println(logLine);

dataFile = SD.open("log.csv", FILE_WRITE);
if (dataFile) {
  dataFile.println(logLine);
  dataFile.close();
}

//Error prohibition delay
delay(50);

}