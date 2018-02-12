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

uint8_t relay = 0;
uint8_t buttonA = 5;
uint8_t buttonB = 6;
uint8_t relayStatus = 0;
uint8_t readButtonA = 0;
uint8_t readButtonB = 0;

void setup() {
  pinMode(relay, OUTPUT);
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
}

void loop() {
  if (digitalRead(buttonA)) {
    if (!readButtonA) {
      readButtonA = 1;
      relayStatus = 1;
      digitalWrite(relay, relayStatus);
    }
  } else {
    readButtonA = 0;
  }

  if (digitalRead(buttonB)) {
    if (!readButtonB) {
      readButtonB = 1;
      relayStatus = 0;
      digitalWrite(relay, relayStatus);
    }
  } else {
    readButtonB = 0;
  }
}

