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

#include "SimbleeCOM.h"

struct payloadStructure {
  char counter;
  int16_t valA;
  int32_t valB;
  float temp;
} __attribute__((packed));

payloadStructure payload;

// initialize hardware
const int buttonA = 5;
const int buttonB = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
  Serial.begin(9600);
  SimbleeCOM.mode = LONG_RANGE;
  SimbleeCOM.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  payload.counter += 1;

  // read button values
  payload.valA = digitalRead(buttonA);
  payload.valB = digitalRead(buttonB);

  // read temperature
  payload.temp = Simblee_temperature(FAHRENHEIT);

  Serial.print("Counter: ");
  Serial.println(payload.counter, DEC);
  Serial.print("ButtonA Value: ");
  Serial.println(payload.valA, DEC);
  Serial.print("ButtonB Value: ");
  Serial.println(payload.valB, DEC);
  Serial.print("Temperature: ");
  Serial.println(payload.temp, DEC);
  Serial.println();
  SimbleeCOM.send((char*)&payload, sizeof(payload));
  delay(1000);
}
