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

#ifndef _Simblee_ENS210_H_
#define _Simblee_ENS210_H_

#include <Arduino.h>
#include <Wire.h>


#define ENS210_ADDRESS  0x43

#define PART_ID     0x00
#define UID         0x04
#define SYS_CTRL    0x10
#define SYS_STAT    0x11
#define SENS_RUN    0x21
#define SENS_START  0x22
#define SENS_STOP   0x23
#define SENS_STAT   0x24
#define T_VAL       0x30
#define H_VAL       0x33

enum tempMode {
  Celsius, // Celsius
  Fahrenheit, // Fahrenheit
  Kelvin, // Kelvin
};


class Simblee_ENS210 {
  public:
    boolean begin(void);
    float readTemp(void); // returns Celsius
    float readTemp(uint8_t mode); // mode Celsius, Fahrenheit or Kelvin
    float readHumidity(void); // returns Humidity
    void readAll(float *tempC, float *tempF, float *tempK, float *humidity); // all measurements

    void setTempContinuous(bool value); //enable and disable continuous mode
    void setHumidityContinuous(bool value); //enable and disable continuous mode

    uint8_t readStatus(void); //read sensor status BIT0 = temp  BIT1 = humidity, 0 = idle, 1 = measuring
    uint16_t readPartID(void);  //read part ID
    uint64_t readUID(void); // read part UID

  private:

    uint32_t crc7(uint32_t val);
    bool _isTempContinuous = false;
    bool _isHumidityContinuous = false;

};

#endif
