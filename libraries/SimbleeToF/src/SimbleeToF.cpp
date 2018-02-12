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

#include "SimbleeToF.h"
#include "i2c_tof.h"

boolean SimbleeToF::begin(void)
{
  char szFullDevName[64];
  _u32MaxWaitTime_ms = 200;  //wait up to 200ms for measurement
  Wire.begin();
  tof_log_set_level(eTOF_LOG_ERROR_LVL);
  int status = tof_connect(szFullDevName, NULL);

  //read eeprom calibration data
  int eeIndex = 0;
  for (int i=0; i<gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS;i++) {
    _u16CalibrationFactorData[i] = eeReadByte(eeIndex++);
    _u16CalibrationFactorData[i] |= (uint16_t)eeReadByte(eeIndex++) << 8;
  }

  if (_u16CalibrationFactorData[0] == 0xCA1B) {
    _eepromCalValid = true;
    printf("Valid Calibration Found\n");
  }
  else {
    _eepromCalValid = false;
    printf("Valid Calibration Not Found\n");
  }

  if (status == 0)
    return true;
  else
    return false;
}

int SimbleeToF::tof_simple_measurement(void)
{
  loadCalibration();
  if (tof_measurement_get_w_wait(&_iDist_mm, &_iAmplitude, &_u16ExposureTime_usec, &_iErrCode, &_iExtendError, _u32MeasFlags, _u32MaxWaitTime_ms))
    return _iDist_mm;
  else
    return -1;
}

int SimbleeToF::tof_measurement(int *iDist_mm, int *iAmplitude, uint16_t *u16ExposureTime_usec, int *iErrCode, int *iExtendError, uint32_t u32MeasFlags, uint32_t u32MaxWaitTime_ms)
{
  loadCalibration();
  return tof_measurement_get_w_wait(iDist_mm, iAmplitude, u16ExposureTime_usec, iErrCode, iExtendError, u32MeasFlags, u32MaxWaitTime_ms);
}

int SimbleeToF::tof_measurement_avg(int nValueToAveage, int *piAvgDist_mm, uint32_t u32MeasFlag)
{
  loadCalibration();
  _iCnt = 0;
  _iGoodCnt = 0;
  _iTotalDist_mm = 0;
  *piAvgDist_mm = 0;

  /*Note: 'too far' or 'too near' will not count as a valid measurement*/
  _iMaxTries = (nValueToAveage * 13 + 9) / 10; /*give 30% more and round up*/

  do {
    /*Check to see if two many invalid values have happen*/
    if (_iCnt > _iMaxTries) {
      *piAvgDist_mm = 0;
      return gTOF_RTN_GENERIC_FAIL; /*-1*/
    }

    /*Get a single measurement*/
    _iRtn = tof_measurement_get_w_wait(&_iDist_mm, &_iAmplitude, &_u16ExposureTime_usec, &_iErrCode, &_iExtendError, _u32MeasFlags, _u32MaxWaitTime_ms);

    /*Only count valid measurement ('too far' or 'too near' will not count as a valid measurement)*/
    if ((_iRtn >= gTOF_RTN_SUCCESS /*0*/) && (_iErrCode == 0) && (_iDist_mm > 0) ) {
      _iTotalDist_mm += _iDist_mm;
      _iGoodCnt++;
    }

    _iCnt++;
  } while (_iGoodCnt < nValueToAveage);

  *piAvgDist_mm = _iTotalDist_mm / nValueToAveage;

  return gTOF_RTN_SUCCESS; /*0*/
}

int SimbleeToF::multipoint_tof_simple_measurement(uint16_t *u16PerPixelMeas_mm)
{
  loadCalibration();
  if (tof_generic_per_pixel_mesurement_get_w_wait(u16PerPixelMeas_mm, _u16PerPixelAmplitude, _u8PerPixelErr, _u8PerPixelExtErr, &_u8GrpErr, &_u16ExposureTime_usec, _u32MeasFlags))
    return 0;
  else
    return -1;
}

int SimbleeToF::multipoint_tof_measurement(uint16_t *u16PerPixelMeas_mm, uint16_t *u16PerPixelAmplitude, uint8_t *u8PerPixelErr, uint8_t *u8PerPixelExtErr, uint8_t *u8GrpErr, uint16_t *u16ExposureTime_usec, uint32_t u32MeasFlags)
{
  loadCalibration();
  return tof_generic_per_pixel_mesurement_get_w_wait(u16PerPixelMeas_mm, u16PerPixelAmplitude, u8PerPixelErr, u8PerPixelExtErr, u8GrpErr, u16ExposureTime_usec, u32MeasFlags);
}

uint8_t SimbleeToF::eeReadByte(uint16_t address)
{
  Wire.beginTransmission(EEPROM_ADDRESS);
  Wire.write(address >> 8);
  Wire.write(address & 0xFF);
  Wire.endTransmission();
  Wire.requestFrom(EEPROM_ADDRESS, 1);
  uint8_t u8data = Wire.read();
  return u8data;
}

void SimbleeToF::calibration(bool loadCal)
{
  if (loadCal == true)
    _eepromCalValid = true;
  else
    _eepromCalValid = false;
}

int SimbleeToF::loadCalibration(void)
{
  if (_eepromCalValid == true)
    return tof_calibration_factors_set(_u16CalibrationFactorData);
  else
    return -1;
}

