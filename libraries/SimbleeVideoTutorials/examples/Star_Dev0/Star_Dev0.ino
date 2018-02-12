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

char device = 'C';
char payload[] = {device, 0};
int button = 5;
int led_blue = 4;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_blue, OUTPUT);
  pinMode(button, INPUT);
  Serial.begin(9600);
  SimbleeCOM.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  payload[1] = digitalRead(button);
  digitalWrite(led_blue, payload[1]);
  SimbleeCOM.send(payload, sizeof(payload));
  delay(100);
}
