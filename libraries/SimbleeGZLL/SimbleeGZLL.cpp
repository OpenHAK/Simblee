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

#include "Arduino.h"
#include "SimbleeGZLL.h"

SimbleeGZLLClass::SimbleeGZLLClass()
{
  ::SimbleeGZLL_used = 1;

	txPowerLevel = +4;
	hostBaseAddress = 0U;
	deviceBaseAddress = 0U;
}

int SimbleeGZLLClass::begin(device_t device)
{
  SimbleeGZLL_tx_power_level = txPowerLevel;
  SimbleeGZLL_host_base_address   = hostBaseAddress;
  SimbleeGZLL_device_base_address = deviceBaseAddress;

  return SimbleeGZLL_begin(device);
}

void SimbleeGZLLClass::end()
{
  SimbleeGZLL_end();
  NRF_RADIO->TASKS_RSSISTOP = 1;
}

bool SimbleeGZLLClass::sendToHost(const char *data, int len)
{
	return SimbleeGZLL_send_to_host(data, len);
}

bool SimbleeGZLLClass::sendToDevice(device_t device, const char *data, int len)
{
  return SimbleeGZLL_send_to_device(device, data, len);
}

bool SimbleeGZLLClass::sendToHost(const char *data)
{
  return sendToHost(data, strlen(data));
}

bool SimbleeGZLLClass::sendToDevice(device_t device, const char *data)
{
  return sendToDevice(device, data, strlen(data));
}

bool SimbleeGZLLClass::sendToHost(String &data)
{
  char buf[32];
  data.toCharArray(buf, sizeof(buf));
  return sendToHost(buf);
}

bool SimbleeGZLLClass::sendToDevice(device_t device, String &data)
{
  char buf[32];
  data.toCharArray(buf, sizeof(buf));
  return sendToDevice(device, buf);
}

SimbleeGZLLClass SimbleeGZLL;
