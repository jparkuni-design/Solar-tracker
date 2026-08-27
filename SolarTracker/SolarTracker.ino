//LCD setup
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
int j;
String text0="Successful!";
String text1="Data loading";
String text2="Hum(%):";
String text3="T(C):";
String text4="T(F):";
String text5="Volt(V)";
String text6="Cur(mA)";
String text7="Pow(W)";

//Servo setup (completed)
#include <Servo.h>
Servo Yservo;
Servo Xservo;
int xpin=10;
int ypin=9;
int anglex=90;
int angley=90;

//Overall delaytime
int dt=300;

//DHT-11 setup (completed)
#include "DHT.h"
#define Type DHT11
int sensePin=8;
DHT HT(sensePin,Type);
float humidity;
float tempC;
float tempF;

//NWSE photoresistor setups (completed)
int ps1=A3;
int ps2=A2;
int ps3=A1;
int ps4=A0;
int northval;
int westval;
int southval;
int eastval;
int treshold=20;

//Display toggle switch setup (Completed)
int switchpin=3;
int lastSwitchState=HIGH;

int voltagePin=A6;
int currentPin=A7;
const float R1=10000.0; // top resistor
const float R2=10000.0; // bottom resistor
const float VOLTAGE_DIVIDER_RATIO=(R1 + R2) / R2; //=2.0, max readable ~10V (safety feature)
const float ACS712_SENSITIVITY=0.185; // 5A module = 0.185, 20A = 0.100, 30A = 0.066
const float ACS712_ZERO_CURRENT_VOLTAGE=2.5; 
float panelVoltage;
float panelCurrent;
float panelPower;

//Energy accumulation (Wh) setup
unsigned long prevTime=0;
float energyWh=0;
unsigned long printTime=0;
const unsigned long printEvery=1000; //ms between Serial Wh prints

//LED indicator setup (Completed)
int ledpin=2;

//initial on program
void setup() {
  //initial setup init
  Serial.begin(9600);
  Xservo.attach(xpin);
  Yservo.attach(ypin);
  HT.begin();
  lcd.init();
  lcd.backlight();
  pinMode(switchpin, INPUT_PULLUP);
  digitalWrite(switchpin, HIGH);
  pinMode(ledpin,OUTPUT);
  prevTime=millis();
  printTime=millis();
}

//Loop
void loop() {

//Photoresistors loop
northval=analogRead(ps1);//A0
westval=analogRead(ps2);//A1
southval=analogRead(ps3);//A2
eastval=analogRead(ps4);//A3


int ps1=A3;
int ps2=A2;
int ps3=A1;
int ps4=A0;

//Voltage/Current loop
int rawV=analogRead(voltagePin);
panelVoltage=(rawV / 1023.0) * 5.0 * VOLTAGE_DIVIDER_RATIO;

int rawI = analogRead(currentPin);

float senseVoltage=(rawI / 1023.0) * 5.0;
panelCurrent=(senseVoltage - ACS712_ZERO_CURRENT_VOLTAGE) / ACS712_SENSITIVITY;
panelPower=panelVoltage * panelCurrent;

//Energy accumulation (Wh) loop
unsigned long now=millis();
float hours=(now - prevTime) / 3600000.0;
energyWh += panelPower * hours;
prevTime=now;

if (now - printTime >= printEvery) {
  Serial.print("Energy: ");
  Serial.print(energyWh, 4);
  Serial.println(" Wh");
  printTime=now;
}

//DHT-11 loop
humidity=HT.readHumidity();
tempC=HT.readTemperature();
tempF=HT.readTemperature(true);

//Toggle Switch operation (digital read, clean HIGH/LOW)
int currentSwitchState=digitalRead(switchpin);

//Lcd loading page operation
if (currentSwitchState == LOW && lastSwitchState == HIGH) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text1);//"Data loading…"
  lcd.setCursor(16,0);
  for (j=3; j>=1; --j) {
    lcd.print(j);
    delay(1000);
    lcd.clear();
  }
  lcd.setCursor(0,0);
  lcd.print(text0);//"Successful!";
  delay(1000);
  lcd.clear();
}

lastSwitchState = currentSwitchState;

//lcd display ON data display (Humidity, TempC, and TempF) operation
if (currentSwitchState == LOW) {
  lcd.backlight();

  digitalWrite(ledpin,HIGH);

  lcd.setCursor(0,0);
  lcd.print(text2);//"Hum(%):"
  lcd.setCursor(8,0);
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print(text3);//T(C)
  lcd.setCursor(5,1);
  lcd.print(tempC);
  lcd.print("C");

  lcd.setCursor(7,2);
  lcd.print(text4);//T(F)
  lcd.setCursor(10,2);
  lcd.print(tempF);
  lcd.print("F");

  lcd.setCursor(0,3);
  lcd.print(text5);//"V(V):"
  lcd.setCursor(6,3);
  lcd.print(panelVoltage);
  lcd.print("V");

  lcd.setCursor(12,3);
  lcd.print(text6);//"A(A):"
  lcd.setCursor(16,3);
  lcd.print(panelCurrent);
  lcd.print("A");

  lcd.setCursor(0,4);
  lcd.print(text7);//"P|V*A(W):";
  lcd.setCursor(10,4);
  lcd.print(panelPower);
  lcd.print("W");
}

//lcd display OFF data display (Humidity, TempC, and TempF) operation
else {
  lcd.noBacklight();
  digitalWrite(ledpin,LOW);
}

//Automated motor adjustment operation
if (westval - eastval > treshold) {
  anglex=anglex-3;
}

else if (eastval - westval > treshold) {
  anglex=anglex+3;
}

if (northval - southval > treshold) {
  angley=angley+3;
}

else if (southval - northval > treshold) {
  angley=angley-3;
}

anglex=constrain(anglex, 0, 180);
angley=constrain(angley, 0, 180);

//Moving Servo loop
Xservo.write(anglex);
Yservo.write(angley);


//Error prohibition delay
delay(50);
}
