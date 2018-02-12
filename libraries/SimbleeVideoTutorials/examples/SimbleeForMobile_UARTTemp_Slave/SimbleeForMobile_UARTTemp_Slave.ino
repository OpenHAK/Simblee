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

uint8_t led = 3;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    char data = Serial.read();
    uint8_t unit = FAHRENHEIT;

    if (data == 'c') {
      unit = CELSIUS;
    }

    float tempVal = Simblee_temperature(unit);
    unsigned char* tempValPtr = (unsigned char*) &tempVal;
    uint8_t i = 0;

    for (; i < 4; i++) {
      Serial.write(tempValPtr[i]);
    }
  }
}
