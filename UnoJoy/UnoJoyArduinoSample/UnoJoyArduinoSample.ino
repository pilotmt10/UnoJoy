#include "UnoJoy.h"
#include <EEPROM.h>

#define DEBUG false
#define avgLoops 5
#define calibratePin1 8
#define calibratePin2 9

#define xAxisMinPin 3
#define xAxisMaxPin 6
#define yAxisMinPin 5
#define yAxisMaxPin 4

int xAxisMin = 38;
int xAxisMax = 69;
int yAxisMin = 94;
int yAxisMax = 140;

int avgRead(int pin) {
  unsigned int avg = 0;

  for (byte i = 0; i < avgLoops; i++) {
    avg += analogRead(pin) >> 2;
  }

  return avg / avgLoops;
}

void setup(){
  setupPins();
  setupUnoJoy();
}

void loop(){
  // Always be getting fresh data
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
  calibrate();
}

void setupPins(void){
  // Set all the digital pins as inputs
  // with the pull-up enabled, except for the
  // two serial line pins
  for (int i = 2; i <= 12; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
  pinMode(A4, INPUT);
  digitalWrite(A4, HIGH);
  pinMode(A5, INPUT);
  digitalWrite(A5, HIGH);

  // EEPROM.put(0, xAxisMin);
  // EEPROM.put(2, xAxisMax);
  // EEPROM.put(4, yAxisMin);
  // EEPROM.put(6, yAxisMax);

  xAxisMin = EEPROM.get(0, xAxisMin);
  xAxisMax = EEPROM.get(2, xAxisMax);
  yAxisMin = EEPROM.get(4, yAxisMin);
  yAxisMax = EEPROM.get(6, yAxisMax);
}

void calibrate(void) {
  if (!digitalRead(calibratePin1) && !digitalRead(calibratePin2)) {
    if (!digitalRead(xAxisMinPin)) {
      int value = avgRead(A0);
      EEPROM.put(0, value);
      xAxisMin = value;
    } else if (!digitalRead(xAxisMaxPin)) {
      int value = avgRead(A0);
      EEPROM.put(2, value);
      xAxisMax = value;
    } else if (!digitalRead(yAxisMinPin)) {
      int value = avgRead(A1);
      EEPROM.put(4, value);
      yAxisMin = value;
    } else if (!digitalRead(yAxisMaxPin)) {
      int value = avgRead(A1);
      EEPROM.put(6, value);
      yAxisMax = value;
    }
  }
}

dataForController_t getControllerData(void){

  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  controllerData.triangleOn = !digitalRead(2);
  controllerData.circleOn = !digitalRead(3);
  controllerData.squareOn = !digitalRead(4);
  controllerData.crossOn = !digitalRead(5);
  controllerData.dpadUpOn = !digitalRead(6);
  controllerData.dpadDownOn = !digitalRead(7);
  controllerData.dpadLeftOn = !digitalRead(8);
  controllerData.dpadRightOn = !digitalRead(9);
  controllerData.l1On = !digitalRead(10);
  controllerData.r1On = !digitalRead(11);
  controllerData.selectOn = !digitalRead(12);
  controllerData.startOn = !digitalRead(A4);
  controllerData.homeOn = !digitalRead(A5);

  // Set the analog sticks
  //  Since analogRead(pin) returns a 10 bit value,
  //  we need to perform a bit shift operation to
  //  lose the 2 least significant bits and get an
  //  8 bit number that we can use
  controllerData.leftStickX = constrain(map(avgRead(A0), xAxisMin, xAxisMax, 0, 255), 0, 255);
  controllerData.leftStickY = constrain(map(avgRead(A1), yAxisMin, yAxisMax, 0, 255), 0, 255);
  controllerData.rightStickX = analogRead(A2) >> 2;
  controllerData.rightStickY = analogRead(A3) >> 2;

  if (DEBUG) {
    Serial.print(xAxisMin);
    Serial.print(" - ");
    Serial.print(xAxisMax);
    Serial.print("  x  ");
    Serial.print(yAxisMin);
    Serial.print(" - ");
    Serial.println(yAxisMax);
  }

  // And return the data!
  return controllerData;
}
