//Abs value setup (Completed)
#include <math.h>


//LCD setup (Completed)
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


//Servo setup (Completed)
#include <Servo.h>
Servo Yservo;
Servo Xservo;
int xpin=10;
int ypin=9;
int anglex=90;
int angley=90;


//DHT-11 setup (Completed)
#include "DHT.h"
#define Type DHT11
int sensePin=8;
DHT HT(sensePin,Type);
float humidity;
float tempC;


//NWSE photoresistor setups (Completed)
int ps1=A3;//N
int ps2=A2;//W
int ps3=A1;//S
int ps4=A0;//E
int northval;
int westval;
int southval;
int eastval;
int threshold=40;


//Voltage/Current setup, A6 reads the raw value from the panel (Completed)
int switchpin=3;
int lastSwitchState=HIGH;
int currentSwitchState=HIGH;


//Voltage/Current setup
int voltagePin=A6;
int currentPin=A7;
int rawV;
int rawI;


//Voltage/Current setup, A6 reads the raw value from the panel (Completed)
const float VOLTAGE_DIVIDER_RATIO=1.0;
const float ACS712_SENSITIVITY=0.185;
const float ACS712_ZERO_CURRENT_VOLTAGE=2.5;
float panelVoltage;
float panelCurrent;
float panelPower;


//Energy accumulation setup
unsigned long lastMillis = 0;
float wattHours = 0.0;


//LED indicator setup (Completed)
int ledpin=2;


//Function declaration (Completed)
void setup() {
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


//Function definition/operation
void loop() {
 readPhotoresistors();
 readPowerSensors();
 printDebugData();
 updateEnergyAccumulation();
 readEnvironment();
 handleToggleSwitch();
 updateDisplay();
 updateTrackingAngles();
 moveServos();

 //Error prohibition delay
 delay(50);
}


//Photoresistor function definition
void readPhotoresistors() {
 northval=analogRead(ps1);
 westval=analogRead(ps2);
 southval=analogRead(ps3);
 eastval=analogRead(ps4);
}


//Voltage&Current function definition
void readPowerSensors() {
 long rawVsum = 0;
 long rawIsum = 0;
 
  for (int k=0; k<10; k++) {
   rawVsum += analogRead(voltagePin);
   rawIsum += analogRead(currentPin);
 }
 rawV = rawVsum / 10;
 rawI = rawIsum / 10;
 panelVoltage=(rawV / 1023.0) * 5.0 * VOLTAGE_DIVIDER_RATIO;


 float senseVoltage=(rawI / 1023.0) * 5.0;
 panelCurrent=abs((senseVoltage - ACS712_ZERO_CURRENT_VOLTAGE) / ACS712_SENSITIVITY);
 panelPower=panelVoltage * panelCurrent;
}


//Serial monitor output function definition
void printDebugData() {
 Serial.print("rawV: ");
 Serial.print(rawV);
 Serial.print("  rawI: ");
 Serial.print(rawI);
 Serial.print("  |  V: ");
 Serial.print(panelVoltage, 3);
 Serial.print("  I: ");
 Serial.print(panelCurrent, 3);
 Serial.print("  P: ");
 Serial.println(panelPower, 3);
}


//Energy accumulation function definition
void updateEnergyAccumulation() {
 unsigned long now = millis();
 float hoursElapsed = (now - lastMillis) / 3600000.0;
 wattHours += panelPower * hoursElapsed;
 lastMillis = now;
 Serial.print("E(Wh): ");
 Serial.println(wattHours, 2);
}


//DHT-11 function definition
void readEnvironment() {
 humidity=HT.readHumidity();
 tempC=HT.readTemperature();
}


//Toggle switch function definition
void handleToggleSwitch() {
 currentSwitchState=digitalRead(switchpin);


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
   lcd.print(text0);//"Successful!"
   delay(1000);
   lcd.clear();
 }
 //assign lastSwitchState a value of currentSwitchState to operate toggle
 lastSwitchState = currentSwitchState;
}


//Lcd display function definition
void updateDisplay() {
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

   
 else {
   lcd.noBacklight();
   digitalWrite(ledpin,LOW);
  }
}


//ServoX&ServoY condition function definition
void updateTrackingAngles() {
 int maxVal = max(max(northval, southval), max(eastval, westval));
 int minVal = min(min(northval, southval), min(eastval, westval));


 if (maxVal - minVal < threshold) {
   anglex = 90;
   angley = 90;
 }
 
 
 else {
   if (westval - eastval > threshold) {
     anglex = anglex + 3;
   }
   
   else if (eastval - westval > threshold) {
     anglex = anglex - 3;
   }


   if (northval - southval > threshold) {
     angley = angley + 3;
   }

   else if (southval - northval > threshold) {
     angley = angley - 3;
   }
 }
}


//ServoX&ServoY operating function definition
void moveServos() {
 anglex=constrain(anglex, 20, 160);
 angley=constrain(angley, 20, 160);
 Xservo.write(anglex);
 Yservo.write(angley);
}
