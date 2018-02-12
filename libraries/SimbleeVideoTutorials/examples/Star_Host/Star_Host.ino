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
char device = 'A'; // Host: 'A', Nodes: 'B', 'C', 'D'
char payload[] = {device, 0};
char pay0 = 0;
char pay1 = 0;
int led_red = 2;
int led_green = 3;
int led_blue = 4;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  Serial.begin(9600);
  SimbleeCOM.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Device 0 State:");
  Serial.print(pay0, DEC);
  Serial.print("\t");
  Serial.print("Device 1 State:");
  Serial.print(pay1, DEC);
  Serial.println();
  digitalWrite(led_red, pay0);
  digitalWrite(led_blue, pay1);

  if (pay0 && pay1) {
    payload[1] = 1;
  } else {
    payload[1] = 0;
  }

  SimbleeCOM.send(payload, sizeof(payload));
  delay(100);
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi) {
  if (payload[0] == 'B') {
    pay0 = payload[1];
  }

  if (payload[0] == 'C') {
    pay1 = payload[1];
  }
}

