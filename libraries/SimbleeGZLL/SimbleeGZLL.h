/*
The Gazell protocol is a wireless communication protocol that is used
to setup a robust wireless link between a single host and up to eight
devices in a star network topology.

The Host in a Gazell network is always listening, and it is the Device
that always initiates a communication.

Each packet that a Device sends is required to be acknowledged by the
Host.  Gazell auotmatically handle packet retransmission if necessary.

It is possible for the Host to send data to the Device by piggybacking
data to an acknowledgement (ACK) packet.  Therefore a Host has to wait
for a packet from a Device before it can send any data to it.

Gazell utilizes channel hopping functionality that gives a high date
rate and reliable wireless link.

Gazell requires no connection packets to setup a link, and devices can
enter and exit from the network at any time.
*/

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

/*
  Copyright (c) 2012, Nordic Semiconductor.  All Rights Reserved.

  Gazell is the property of Nordic Semiconductor ASA.
  Terms and conditions of the usage are described in detail in NORDIC
  SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
*/


#ifndef _SIMBLEEGZLL_H_
#define _SIMBLEEGZLL_H_

#include <stdint.h>
#include <stdbool.h>

#include "WString.h"

// needed for enum and callback support
#include "libSimbleeGZLL.h"

class SimbleeGZLLClass
{
public:
  // -20 dBm to +4 dBm - default +4 dBm
  int txPowerLevel;
  uint32_t hostBaseAddress;
  uint32_t deviceBaseAddress;

public:
  SimbleeGZLLClass();

  int begin(device_t device);
  void end();

  // max length is 32 bytes
  bool sendToHost(const char *data, int len);
  bool sendToDevice(device_t device, const char *data, int len);

  bool sendToHost(char data)          { return sendToHost((const char *)&data, sizeof(char)); }
  bool sendToHost(unsigned char data) { return sendToHost((const char *)&data, sizeof(unsigned char)); }
  bool sendToHost(int data)           { return sendToHost((const char *)&data, sizeof(int)); }
  bool sendToHost(unsigned int data)  { return sendToHost((const char *)&data, sizeof(unsigned int)); }
  bool sendToHost(long data)          { return sendToHost((const char *)&data, sizeof(long)); }
  bool sendToHost(unsigned long data) { return sendToHost((const char *)&data, sizeof(unsigned long)); }
  bool sendToHost(float data)         { return sendToHost((const char *)&data, sizeof(float)); }
  bool sendToHost(double data)        { return sendToHost((const char *)&data, sizeof(double)); }
  bool sendToHost(char *data)         { return sendToHost((const char *)data); }
  bool sendToHost(const char *data);
  bool sendToHost(String &data);
  bool sendToHost(const __FlashStringHelper *data) { return sendToHost(reinterpret_cast<const char *>(data)); }

  bool sendToDevice(device_t device, char data)          { return sendToDevice(device, (const char *)&data, sizeof(char)); }
  bool sendToDevice(device_t device, unsigned char data) { return sendToDevice(device, (const char *)&data, sizeof(unsigned char)); }
  bool sendToDevice(device_t device, int data)           { return sendToDevice(device, (const char *)&data, sizeof(int)); }
  bool sendToDevice(device_t device, unsigned int data)  { return sendToDevice(device, (const char *)&data, sizeof(unsigned int)); }
  bool sendToDevice(device_t device, long data)          { return sendToDevice(device, (const char *)&data, sizeof(long)); }
  bool sendToDevice(device_t device, unsigned long data) { return sendToDevice(device, (const char *)&data, sizeof(unsigned long)); }
  bool sendToDevice(device_t device, float data)         { return sendToDevice(device, (const char *)&data, sizeof(float)); }
  bool sendToDevice(device_t device, double data)        { return sendToDevice(device, (const char *)&data, sizeof(double)); }
  bool sendToDevice(device_t device, char *data)         { return sendToDevice(device, (const char *)data); }
  bool sendToDevice(device_t device, const char *data);
  bool sendToDevice(device_t device, String &data);
  bool sendToDevice(device_t device, const __FlashStringHelper *data) { return sendToDevice(device, reinterpret_cast<const char *>(data)); }
};

extern SimbleeGZLLClass SimbleeGZLL;

#endif
