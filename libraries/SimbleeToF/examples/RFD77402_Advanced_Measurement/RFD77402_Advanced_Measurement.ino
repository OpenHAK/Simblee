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
 
#include <Wire.h>
#include "SimbleeToF.h"

SimbleeToF tof;

void setup()
{
  Serial.begin(115200);
  delay(100);
  printf("RFDigital RFD77402 IoT 3D ToF Sensor advanced measurement example\n\n");
  if (!tof.begin()) {
    printf("Failed to intialize Sensor, Halting!\n");
    while (1);
  }
}

void loop()
{
  printMeasurements();
  delay(1000);
}

void printMeasurements(void)
{
  int iRtn;
  int iErrCode;
  int iDist_mm;
  int iAmplitude;
  int iCount;
  int iAvgDist_mm;
  int iExtendError;
  uint16_t u16ExposureTime_usec;
  uint32_t u32MeasFlags = 0;
  uint32_t u32MaxWaitTime_ms = 200; //max wait time is 200ms
  uint8_t numAverages = 4; //average 4 measurments
  
  iCount = 0;
  printf("\ndisplaying one-shot ToF measurement per line - only printing valid measures\n");
  do {
    iErrCode = iDist_mm = 0;

    iRtn = tof.tof_measurement(&iDist_mm, &iAmplitude, &u16ExposureTime_usec, &iErrCode, &iExtendError, u32MeasFlags, u32MaxWaitTime_ms);
    if (iRtn > 0) {
      printf("Dist_mm:%3d mm              Err:%-2d  Amp:%-4d   Exposure_Time:%-4d uSeconds\n", iDist_mm, iErrCode, iAmplitude, u16ExposureTime_usec);
    }
    iCount++;
  } while (iCount < 10);


  iCount = 0;
  printf("\nndisplaying average of %d measurements - only printing valid measures\n", numAverages);
  do {
    iErrCode = iDist_mm = 0;

    iRtn = tof.tof_measurement_avg(numAverages, &iAvgDist_mm, u32MeasFlags);
    if (iRtn >= 0) {
      printf("Dist_mm:%d mm\n", iAvgDist_mm);
    }
    iCount++;
  } while (iCount < 10);

}