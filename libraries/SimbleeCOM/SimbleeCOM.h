/*
 * Copyright (c) 2015 RF Digital Corp. All Rights Reserved.
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

#ifndef _SIMBLEECOM_H_
#define _SIMBLEECOM_H_

#include "Arduino.h"
#include "libSimbleeCOM.h"

class SimbleeCOMClass
{
public:
  // LONG_RANGE or LOW_LATENCY - default LONG_RANGE
  int mode;

  // -20 dBm to +4 dBm - default +4 dBm
  int txPowerLevel;

private:
  static void SWI0_Handler(void);

public:  
  void startReceive();
  void stopReceive();

public:
  static unsigned int getESN();
  static void proximityMode(bool enable);
  
  SimbleeCOMClass();
  
  void begin();
  void end();
  
  bool send(const char *data, int len);
};

extern SimbleeCOMClass SimbleeCOM;

#endif
