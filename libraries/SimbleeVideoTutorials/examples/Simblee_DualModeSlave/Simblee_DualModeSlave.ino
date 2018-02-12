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
uint8_t ledStatus = 0;
uint8_t onButton = 5;
uint8_t offButton = 6;
uint8_t readOnButton = 0;
uint8_t readOffButton = 0;
bool ledStatusUpdate = false;

void setup() {
  pinMode(ledGREEN, OUTPUT);
  pinMode(onButton, INPUT);
  pinMode(offButton, INPUT);
  SimbleeCOM.mode = LOW_LATENCY;
  SimbleeCOM.begin();
}

void loop() {
  if (digitalRead(onButton)) {
    if (!readOnButton) {
      if (!ledStatus) {
        ledStatus = 1;
        ledStatusUpdate = true;
      }
      readOnButton = 1;
    }
  } else {
    readOnButton = 0;
  }

  if (digitalRead(offButton)) {
    if (!readOffButton) {
      if (ledStatus) {
        ledStatus = 0;
        ledStatusUpdate = true;
      }
      readOffButton = 1;
    }
  } else {
    readOffButton = 0;
  }
}

void SimbleeCOM_onReceive(unsigned int esn, const char* payload, int len, int rssi) {
  if (payload[0] == '?') {
    if (ledStatusUpdate) {
      SimbleeCOM.send((ledStatus ? "1" : "0"), 1);
    }
  } else if (payload[0] == '1') {
    ledStatus = 1;
  } else {
    ledStatus = 0;
  }

  ledStatusUpdate = false;

  digitalWrite(ledGREEN, ledStatus);
}
