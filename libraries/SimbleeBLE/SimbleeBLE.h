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

#ifndef _SIMBLEEBLE_H_
#define _SIMBLEEBLE_H_

#include <stdint.h>
#include <stdbool.h>

// needed for callback support
#include "libSimbleeBLE.h"

class SimbleeBLEClass
{
public:
  // default "" (previous fixed Simblee uuid)
  const char *customUUID;

  // default "Simblee"
  const char *deviceName;

  // default "sketch"
  const char *advertisementData;

  // Device Information Service strings
  const char *manufacturerName;
  const char *modelNumber;
  const char *serialNumber;
  const char *hardwareRevision;
  const char *softwareRevision;

  bool iBeacon;
  uint8_t iBeaconUUID[16];
  unsigned short iBeaconMajor;
  unsigned short iBeaconMinor;
  // default 2's complement iBeacon Power Measurement at 1 meter (default is 0xC5 = -59dBm)
  unsigned char iBeaconMeasuredPower;

  // in ms (converted to 0.625ms units) - range 20ms to 10.24s - default 80ms
  int advertisementInterval;

  // -20 dBm to +4 dBm - default +4 dBm
  int txPowerLevel;

  // default true (false = nonconnectable advertising)
  // nonconnectable forces minimum of 100ms advertisement_interval
  bool connectable;

  // true if the radio is active (or about to become active), false if not
  static volatile int& radioActive;

public:
  SimbleeBLEClass();

  void begin();
  void end();

  bool send(char data);
  bool send(const char *data, int len);

  bool sendByte(uint8_t data);
  bool sendInt(int data);
  bool sendFloat(float data);
  
  // request central role use a different connection interval in the given range
  // the central role may reject the request (or even pick a value outside the range)
  void updateConnInterval(int min_conn_interval_ms, int max_conn_interval_ms);

  int getConnInterval(void);
  
private:
  static void SWI3_Handler(void);
  
public:
 static void proximityMode(bool enable);
  
  void dualModeBegin(void);
  void dualModeEnd(void);
};

#include "../SimbleeCOM/SimbleeCOM.h"

__attribute__((weak))
void SimbleeBLE_onDualModeStart();

__attribute__((weak))
void SimbleeBLE_onDualModeStop();

extern SimbleeBLEClass SimbleeBLE;

#endif
