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

uint8_t newTemp = 0;
float tempVal = 0;
float hot = 75;
uint8_t ledHot = 2;
uint8_t ledCold = 4;

void setup() {
  pinMode(ledHot, OUTPUT);
  pinMode(ledCold, OUTPUT);
  digitalWrite(ledHot, LOW);
  digitalWrite(ledCold, LOW);
  Serial.begin(9600);
}

void loop() {
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
}

void updateTemperature() {
  if (tempVal >= hot) {
    digitalWrite(ledHot, HIGH);
    digitalWrite(ledCold, LOW);
  } else if (tempVal < hot) {
    digitalWrite(ledHot, LOW);
    digitalWrite(ledCold, HIGH);
  }
}

