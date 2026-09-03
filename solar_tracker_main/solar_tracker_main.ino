//Abs value setup
#include <math.h>

//LCD setup
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
int j;
String text0="Successful!";
String text1="Data loading";
String text2="H(%):";
String text3="T(C):";
String text5="V(V):";
String text6="I(A):";
String text7="P(W):";
String text8="E(Wh):";

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
float tempF; // computed but not displayed on LCD (row budget)

//NWSE photoresistor setups (completed)
int ps1=A3;//N
int ps2=A2;//W
int ps3=A1;//S
int ps4=A0;//E
int northval;
int westval;
int southval;
int eastval;
int treshold=40;

//Display toggle switch setup (Completed)
int switchpin=3;
int lastSwitchState=HIGH;

//Voltage/Current setup
int voltagePin=A6;
int currentPin=A7;
const float R1=10000.0; // top resistor
const float R2=10000.0; // bottom resistor
const float VOLTAGE_DIVIDER_RATIO=(R1 + R2) / R2; // =2.0, max readable ~10V (safety feature)
const float ACS712_SENSITIVITY=0.185; // 5A module = 0.185, 20A = 0.100, 30A = 0.066
const float ACS712_ZERO_CURRENT_VOLTAGE=2.5;
float panelVoltage;
float panelCurrent;
float panelPower;

//Energy accumulation setup
unsigned long lastMillis = 0;
float wattHours = 0.0;

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
  lastMillis = millis();
}

//Loop
void loop() {

//Photoresistors loop
northval=analogRead(ps1);//A3
westval=analogRead(ps2);//A2
southval=analogRead(ps3);//A1
eastval=analogRead(ps4);//A0

//Voltage/Current loop
int rawV=analogRead(voltagePin);
panelVoltage=(rawV / 1023.0) * 5.0 * VOLTAGE_DIVIDER_RATIO;

int rawI = analogRead(currentPin);
float senseVoltage=(rawI / 1023.0) * 5.0;
panelCurrent=abs((senseVoltage - ACS712_ZERO_CURRENT_VOLTAGE) / ACS712_SENSITIVITY);
panelPower=panelVoltage * panelCurrent;

//Energy accumulation loop
unsigned long now = millis();
float hoursElapsed = (now - lastMillis) / 3600000.0;
wattHours += panelPower * hoursElapsed;
lastMillis = now;

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

//lcd display ON data display operation
if (currentSwitchState == LOW) {
  lcd.backlight();

  digitalWrite(ledpin,HIGH);

  // Row 0: Humidity + Temp(C)
  lcd.setCursor(0,0);
  lcd.print(text2);//"H(%):"
  lcd.setCursor(5,0);
  lcd.print(humidity,2);
  lcd.print("  ");

  lcd.setCursor(10,0);
  lcd.print(text3);//"T(C):"
  lcd.setCursor(15,0);
  lcd.print(tempC,2);
  lcd.print("  ");

  // Row 1: Voltage + Current
  lcd.setCursor(0,1);
  lcd.print(text5);//"V(V):"
  lcd.setCursor(5,1);
  lcd.print(panelVoltage,2);
  lcd.print("   ");

  lcd.setCursor(10,1);
  lcd.print(text6);//"I(A):"
  lcd.setCursor(15,1);
  lcd.print(panelCurrent,2);
  lcd.print("  ");

  // Row 2: Power
  lcd.setCursor(0,2);
  lcd.print(text7);//"P(W):"
  lcd.setCursor(5,2);
  lcd.print(panelPower,2);
  lcd.print("      ");

  // Row 3: Energy
  lcd.setCursor(0,3);
  lcd.print(text8);//"E(Wh):"
  lcd.setCursor(6,3);
  lcd.print(wattHours,2);
  lcd.print("      ");
}

//lcd display OFF operation
else {
  lcd.noBacklight();
  digitalWrite(ledpin,LOW);
}

//Automated motor adjustment operation
int maxVal = max(max(northval, southval), max(eastval, westval));
int minVal = min(min(northval, southval), min(eastval, westval));

if (maxVal - minVal < treshold) {
  anglex = 90;
  angley = 90;
}
else {
  if (westval - eastval > treshold) {
    anglex = anglex + 3;
  }
  else if (eastval - westval > treshold) {
    anglex = anglex - 3;
  }

  if (northval - southval > treshold) {
    angley = angley + 3;
  }
  else if (southval - northval > treshold) {
    angley = angley - 3;
  }
}

anglex=constrain(anglex, 20, 160);
angley=constrain(angley, 20, 160);

//Moving Servo loop
Xservo.write(anglex);
Yservo.write(angley);

//ENergy storage track loop
Serial.println(wattHours);

//Error prohibition delay
delay(50);
}
