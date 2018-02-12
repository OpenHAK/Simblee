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
} __attribute((packed));

payloadStructure received;
volatile bool rx_flag = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SimbleeCOM.mode = LONG_RANGE;
  SimbleeCOM.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (rx_flag == true) {
    Serial.print("Counter: ");
    Serial.println(received.counter, DEC);
    Serial.print("ButtonA Value: ");
    Serial.println(received.valA, DEC);
    Serial.print("ButtonB Value: ");
    Serial.println(received.valB, DEC);
    Serial.print("Temperature: ");
    Serial.println(received.temp, DEC);
    Serial.println();
    rx_flag = false;
  }
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  memcpy(&received, payload, len);
  rx_flag = true;
}

