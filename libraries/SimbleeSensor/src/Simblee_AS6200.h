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

#ifndef _Simblee_AS6200_H_
#define _Simblee_AS6200_H_

#include <Arduino.h>
#include <Wire.h>


#define AS6200_ADDRESS  0x49  

#define AS6200_TVAL        0x00  //Temperature register
#define AS6200_CONFIG      0x01  //Configuration register
#define AS6200_TLOW        0x02  //Low temperature threshold register
#define AS6200_THIGH       0x03  //High temperature threshold register

enum
{
  AS6200_CONVERSION_125ms       = 0xC0,
  AS6200_CONVERSION_250ms       = 0x80,  
  AS6200_CONVERSION_1000ms      = 0x40,
  AS6200_CONVERSION_2000ms      = 0x00,
  AS6200_SLEEP_MODE             = 0x100,  //enables sleep mode, otherwise in continuous conversion mode
  AS6200_INTERRUPT_MODE         = 0x200,  //enables interrupt mode, otherwise in comparator mode
  AS6200_INTERRUPT_POLARITY     = 0x400,   //enables active high polarity, otherise active low output
  AS6200_CONSECUTIVE_FAULTS_1   = 0x0000, //num of consecutive faults (temperature exceeding THIGH or falls below TLOW) before indicating a fault condition
  AS6200_CONSECUTIVE_FAULTS_2   = 0x0800, 
  AS6200_CONSECUTIVE_FAULTS_4   = 0x1000, 
  AS6200_CONSECUTIVE_FAULTS_6   = 0x1800,
  AS6200_SINGLESHOT_CONVERSION  = 0x0800  //enables single shot conversion if in sleep mode
};

class Simblee_AS6200 {
  public:
    boolean begin(void);

    int16_t readRawTemperature(void); 
    float readTemperatureF(void);
    float readTemperatureC(void);
    void setConfig(uint16_t config); //set config register
    void setThresholds(int8_t lowerThreshold, int8_t upperThreshold); //lower and upper thresholds in Celsius

  private:

   
};

#endif
