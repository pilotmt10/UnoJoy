#include "MegaJoy.h"
#include <EEPROM.h>

#define DEBUG false
#define avgLoops 5
#define calibratePin1 8
#define calibratePin2 9

#define throttleMinPin 3
#define throttleMaxPin 6

int throttleMin = 38;
int throttleMax = 69;

void setup(){
  setupPins();
  setupMegaJoy();
}

int avgRead(int pin) {
  unsigned int avg = 0;
  for (byte i = 0; i < avgLoops; i++) {
    avg += analogRead(pin);
  }
  return avg / avgLoops;
}

void loop(){
  // Always be getting fresh data
  megaJoyControllerData_t controllerData = getControllerData();
  setControllerData(controllerData);
  calibrate();
}

void setupPins(void){
  // Set all the digital pins as inputs
  // with the pull-up enabled, except for the 
  // two serial line pins
  for (int i = 2; i <= 54; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }

//   EEPROM.put(0, throttleMin);
//   EEPROM.put(2, throttleMax);

  throttleMin = EEPROM.get(0, throttleMin);
  throttleMax = EEPROM.get(2, throttleMax);
}

void calibrate(void) {
  if (!digitalRead(calibratePin1) && !digitalRead(calibratePin2)) {
    if (!digitalRead(throttleMinPin)) {
      int value = avgRead(A0);
      EEPROM.put(0, value);
      throttleMin = value;
    } else if (!digitalRead(throttleMaxPin)) {
      int value = avgRead(A0);
      EEPROM.put(2, value);
      throttleMax = value;
    }
  }
}

megaJoyControllerData_t getControllerData(void){
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  megaJoyControllerData_t controllerData = getBlankDataForMegaController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  for (int i = 2; i < 54; i++){
    controllerData.buttonArray[(i - 2) / 8] |= (!digitalRead(i)) << ((i - 2) % 8);
  }
  
  // Set the analog sticks
  //  Unlike UnoJoy, which has 8-bit analog axes for PS3 compatibilty,
  //  MegaJoy uses 10-bit analog values, to fully use the Arduino analogRead range
  controllerData.analogAxisArray[0] = constrain(map(avgRead(A0), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[1] = constrain(map(avgRead(A1), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[2] = constrain(map(avgRead(A2), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[3] = constrain(map(avgRead(A3), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[4] = constrain(map(avgRead(A4), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[5] = constrain(map(avgRead(A5), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[6] = constrain(map(avgRead(A6), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[7] = constrain(map(avgRead(A7), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[8] = constrain(map(avgRead(A8), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[9] = constrain(map(avgRead(A9), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[10] = constrain(map(avgRead(A10), throttleMin, throttleMax, 0, 1024), 0, 1024);
  controllerData.analogAxisArray[11] = constrain(map(avgRead(A11), throttleMin, throttleMax, 0, 1024), 0, 1024);

  if (DEBUG) {
    Serial.print(throttleMin);
    Serial.print(" - ");
    Serial.print(throttleMax);
  }
  
  // And return the data!
  return controllerData;
}
