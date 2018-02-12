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

#ifndef _Simblee_CCS811_H_
#define _Simblee_CCS811_H_

#include <Arduino.h>
#include <Wire.h>


#define CCS811_ADDRESS           0x5A   
#define CCS811_STATUS            0x00    
#define CCS811_MEAS_MODE         0x01    
#define CCS811_ALG_RESULT_DATA   0x02
#define CCS811_RAW_DATA          0x03
#define CCS811_ENV_DATA          0x05
#define CCS811_NTC               0x06
#define CCS811_THRESHOLDS        0x10
#define CCS811_BASELINE          0x11
#define CCS811_HW_ID             0x20
#define CCS811_HW_VERSION        0x21
#define CCS811_FW_BOOT_VERSION   0x23
#define CCS811_FW_APP_VERSION    0x24
#define CCS811_ERROR_ID          0xE0
#define CCS811_APP_ERASE         0xF1
#define CCS811_APP_DATA          0xF2
#define CCS811_APP_VERIFY        0xF3
#define CCS811_APP_START         0xF4
#define CCS811_SW_RESET          0xFF
#define CCS811_INTERRUPTPIN      2

enum measMode {
  CCS811_MODE0, // Idle
  CCS811_MODE1, // Constant power, measure every second
  CCS811_MODE2, // Pulsed heating mode, measure every 10 seconds
  CCS811_MODE3, // Low powe pulsed heating mode, measure every 60 seconds
  CCS811_MODE4  // Contact power, measure every 250ms
};


class Simblee_CCS811 {
  public:
    boolean begin(void); // defaults to measure 1/second, no interrupts
    boolean begin(uint8_t driveMode, bool interruptsOnDataRdy, bool interruptsOnThreshold);
    uint16_t readTVOC(void);
    uint16_t readECO2(void);
    void readAll(uint16_t *eCO2, uint16_t *TVOC, uint8_t *stat, uint8_t *errorID, uint8_t *uAcurrent, uint16_t *adc);
    uint16_t readRawADC(void); // 0 - 1023, 1023 = 1.65V
    uint8_t readRawCurrent(void); // 0 - 63uA

    void setEnvData(float humidity, float temperature); // enter humidity and temperature (in Celsius), default is 50% and 25C
    void setThresholds(uint16_t lowMedium, uint16_t mediumHigh, uint8_t hysteresis); // default 1500ppm, 2500ppm, 50ppm

    uint8_t readHWID(void);
    uint8_t readStatus(void);
    uint8_t readErrorID(void);

  private:

    uint8_t _nWake = 4;
    uint8_t _measModeRegister;

};

#endif
