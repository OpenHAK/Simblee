/*
   Copyright (c) 2017 RF Digital Corp. All Rights Reserved.

   The source code contained in this file and all intellectual property embodied in
   or covering the source code is the property of RF Digital Corp. or its licensors.
   Your right to use this source code and intellectual property is non-transferable,
   non-sub licensable, revocable, and subject to terms and conditions of the
   SIMBLEE SOFTWARE LICENSE AGREEMENT.
   http://www.simblee.com/licenses/SimbleeSoftwareLicenseAgreement.txt

   THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.

   This heading must NOT be removed from this file.
*/

#ifndef _SimbleeToF_H_
#define _SimbleeToF_H_

#include <Arduino.h>
#include <Wire.h>
#include "i2c_tof.h"

#define EEPROM_ADDRESS  0x50

class SimbleeToF {
  public:
    boolean begin(void);
    int tof_simple_measurement(void);
    int tof_measurement(int *iDist_mm, int *iAmplitude, uint16_t *u16ExposureTime_usec, int *iErrCode, int *iExtendError, uint32_t u32MeasFlags, uint32_t u32MaxWaitTime_ms);
    int tof_measurement_avg(int nValueToAveage, int *piAvgDist_mm, uint32_t u32MeasFlag);
	
	  int multipoint_tof_simple_measurement(uint16_t *pu16PerPixelMeas_mm);
	  int multipoint_tof_measurement(uint16_t *pu16PerPixelMeas_mm, uint16_t *pu16PerPixelAmplitude, uint8_t *pu8PerPixelErr, uint8_t *pu8PerPixelExtErr, uint8_t *pu8GrpErr, uint16_t *u16ExposureTime_usec, uint32_t u32MeasFlags);

    void calibration(bool loadCal);
    int loadCalibration(void);

  private:
    uint8_t eeReadByte(uint16_t address);
    bool _eepromCalValid = false;
    int _iDist_mm;
    int _iAmplitude;
    uint16_t _u16ExposureTime_usec;
    int _iErrCode;
    int _iExtendError;
    uint32_t _u32MeasFlags;
    uint32_t _u32MaxWaitTime_ms; 
    int _iRtn;
    int _iCnt = 0;
    int _iGoodCnt = 0;
    int _iMaxTries;
    int _iTotalDist_mm = 0;
	  uint16_t _u16PerPixelAmplitude[9];
    uint8_t _u8PerPixelErr[9];
    uint8_t _u8PerPixelExtErr[9];
    uint8_t _u8GrpErr;
    uint16_t _u16CalibrationFactorData[gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS];
	
};

#endif
