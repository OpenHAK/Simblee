/*
 * Copyright (c) 2017 RF Digital Corp. All Rights Reserved.
 *
 * The source code contained in this file and all intellectual property embodied in
 * or covering the source code is the property of RF Digital Corp. or its licensors.
 * Your right to use this source code and intellectual property is non-transferable,
 * non-sub licensable, revocable, and subject to terms and conditions of the
 * SIMBLEE SOFTWARE LICENSE AGREEMENT.
 * http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt
 *
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 *
 * This heading must NOT be removed from this file.
 */

#include <SimbleeForMobile.h>

uint32_t lastCheck;
uint16_t timeout = 1000;
uint8_t isConnected = 0;
uint8_t newTemp;
float tempVal = 0;
float hot[2] = {23.88, 75};
uint8_t ledHot = 2;
uint8_t ledCold = 4;
uint8_t unit = FAHRENHEIT;
color_t bluish = 0x00162B88;
color_t lighter = 0x0092A6FC;
uint8_t tempID;
uint8_t unitID;
uint8_t sliderID;
uint8_t timeoutText;

void setup() {
  pinMode(ledHot, OUTPUT);
  pinMode(ledCold, OUTPUT);
  digitalWrite(ledHot, LOW);
  digitalWrite(ledCold, HIGH);
  lastCheck = 0;
  Serial.begin(9600);
  SimbleeForMobile.advertisementData = "Temp";
  SimbleeForMobile.begin();
}

void loop() {
  if (millis() - lastCheck > timeout) {
    char data = unit == FAHRENHEIT ? 'f' : 'c';
    Serial.print(data);
    lastCheck = millis();
  }

  if (Serial.available()) {
    char* tempValPtr = (char*) &tempVal;
    uint8_t i = 0;

    while (Serial.available() && i < 4) {
      *tempValPtr++ = Serial.read();
      i++;
      delay(5);
    }

    newTemp = 1;
  }

  if (newTemp) {
    updateTemperature();
    newTemp = 0;
  }

  SimbleeForMobile.process();
}

void updateTemperature() {
  if (tempVal >= hot[unit]) {
    digitalWrite(ledHot, HIGH);
    digitalWrite(ledCold, LOW);
  } else if (tempVal < hot[unit]) {
    digitalWrite(ledHot, LOW);
    digitalWrite(ledCold, HIGH);
  }

  if (isConnected) {
    SimbleeForMobile.updateValue(tempID, (int16_t) tempVal);
  }
}

void ui() {
  SimbleeForMobile.beginScreen(bluish);
  SimbleeForMobile.drawText((SimbleeForMobile.screenWidth / 2) - 135, 80, "SimbleeForMobile UART Temperature", lighter, 16);
  tempID = SimbleeForMobile.drawText((SimbleeForMobile.screenWidth / 2) - 50, 150, (int16_t) tempVal, lighter,100);
  unitID = SimbleeForMobile.drawText((SimbleeForMobile.screenWidth / 2) + 80, 180, unit == FAHRENHEIT ? "F" : "C", lighter, 16);
  sliderID = SimbleeForMobile.drawSlider(20, 320, SimbleeForMobile.screenWidth - 40, 1, 10, lighter);
  timeoutText = SimbleeForMobile.drawText((SimbleeForMobile.screenWidth / 2) - 10, 360, timeout / 1000, lighter, 16);
  SimbleeForMobile.drawText((SimbleeForMobile.screenWidth / 2) + 10, 360, "s", lighter, 16);
  SimbleeForMobile.setEvents(tempID, EVENT_PRESS);
  SimbleeForMobile.setEvents(unitID, EVENT_PRESS);
  SimbleeForMobile.setEvents(sliderID, EVENT_PRESS | EVENT_RELEASE);
  SimbleeForMobile.endScreen();
}

void ui_event(event_t &event) {
  if (event.type == EVENT_PRESS) {
    if (event.id == tempID) {
      lastCheck = millis() - timeout;
    } else if (event.id == unitID) {
      unit = !unit;
      SimbleeForMobile.updateText(unitID, unit == FAHRENHEIT ? "F" : "C");
      lastCheck = millis() - timeout;
    }
  } else if (event.id == sliderID && event.type == EVENT_RELEASE) {
    timeout = event.value * 1000;
    SimbleeForMobile.updateValue(timeoutText, event.value);
    lastCheck = millis() - timeout;
  }
}

void SimbleeForMobile_onConnect() {
  isConnected = 1;
  lastCheck = millis() - timeout;
}

void SimbleeForMobile_onDisconnect() {
  isConnected = 0;
  newTemp = 0;
  timeout = 1000;
  unit = FAHRENHEIT;
  digitalWrite(ledHot, LOW);
  digitalWrite(ledCold, HIGH);
}

