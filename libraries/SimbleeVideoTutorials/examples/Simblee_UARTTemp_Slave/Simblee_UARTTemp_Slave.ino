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
uint16_t timeout = 1000;
uint32_t lastSend = 0;

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  Serial.begin(9600);
}

void loop() {
  if (millis() - lastSend > timeout) {
    float tempVal = Simblee_temperature(FAHRENHEIT);
    unsigned char* tempFloat = (unsigned char*) &tempVal;
    uint8_t i = 0;

    for (; i < sizeof(tempVal); i++) {
      Serial.write(tempFloat[i]);
    }

    lastSend = millis();
  }
}
