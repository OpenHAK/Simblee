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

#include <SimbleeCOM.h>

uint8_t ledGREEN = 3;
uint8_t ledBLUE = 4;
int blueVal = 255;
int greenVal = 0;
int maxRSSI = -45;
int minRSSI = -95;
float ledStep = 255 / (maxRSSI - minRSSI);
int currRSSI = 0;
uint32_t rssiTime = 0;
uint8_t calcLED = 0;
char node = 0;


void setup() {
  Serial.begin(9600);
  pinMode(ledBLUE, OUTPUT);
  pinMode(ledGREEN, OUTPUT);
  analogWrite(ledBLUE, blueVal);
  analogWrite(ledGREEN, greenVal);
  SimbleeCOM.mode = LONG_RANGE;
  SimbleeCOM.begin();
}

void loop() {
  if (calcLED) {
    updateLEDs();
    calcLED = 0;
  }

  if (millis() - rssiTime > 20) {
    currRSSI = 0;
    node = 0;
    blueVal = 255;
    greenVal = 0;
    analogWrite(ledBLUE, blueVal);
    analogWrite(ledGREEN, greenVal);
  }
}

void updateLEDs() {
  greenVal = (int)((currRSSI - minRSSI) * ledStep);

  if (greenVal > 255) {
    greenVal = 255;
  } else if (greenVal < 0) {
    greenVal = 0;
  }

  blueVal = 255 - greenVal;
  analogWrite(ledBLUE, blueVal);
  analogWrite(ledGREEN, greenVal);
}

void SimbleeCOM_onReceive(unsigned int esn, const char* payload, int len, int rssi) {
  if (rssi > currRSSI || (rssi <= currRSSI && (millis() - rssiTime) > 10)) {
    if (payload[0] != node || rssi != currRSSI) {
      Serial.print("Tracking node: ");
      Serial.print(payload[0]);
      Serial.print(". RSSI: ");
      Serial.println(rssi);
      node = payload[0];
    }

    currRSSI = rssi;
    rssiTime = millis();
    calcLED = 1;
  } else if (rssi == currRSSI && (millis() - rssiTime) <= 10 && payload[0] == node) {
    rssiTime = millis();
  }
}

