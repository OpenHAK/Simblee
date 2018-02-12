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

#include <Simblee_CCS811.h>

Simblee_CCS811 CCS811;

int myPinCallback(uint32_t ulPin)
{
  uint16_t TVOC, eCO2, adc;
  uint8_t stat, errorID, uAcurrent;
  CCS811.readAll(&eCO2, &TVOC, &stat, &errorID, &uAcurrent, &adc);
  printf("Threshold interrupt!\n");
  printf("TVOC: %dppm, eCO2: %dppm, ADC: %d, uA: %d, Stat: %d, ErrorID: %d\n", TVOC, eCO2, adc, uAcurrent, stat, errorID);
  return 0;  // don't exit Simblee_ULPDelay
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  printf("\nRFDigital CCS811 gas sensor advanced example\n\n");
  if (!CCS811.begin(CCS811_MODE4, true, true)) {    //MODE 0-4, enable interrupts, enable interrupts on threshold
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
  Simblee_pinWakeCallback(CCS811_INTERRUPTPIN, LOW, myPinCallback);
  CCS811.setThresholds(400, 800, 50); //lower/middle eCO2 interrupt threshold, middle/upper eCO2 interrupt threshold, hysteresis
  //CCS811.setEnvData(50.5, 25.75); //optionally set current humidity % and temp in C
}

void loop() {
  // put your main code here, to run repeatedly:
}