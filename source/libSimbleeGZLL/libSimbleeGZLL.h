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

#ifndef _LIBSIMBLEEGZLL_H_
#define _LIBSIMBLEEGZLL_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum
{
  DEVICE0,
  DEVICE1,
  DEVICE2,
  DEVICE3,
  DEVICE4,
  DEVICE5,
  DEVICE6,
  DEVICE7,
  HOST
} device_t;

// -20 dBm to +4 dBm - default +4 dBm
extern int SimbleeGZLL_tx_power_level;
extern uint32_t SimbleeGZLL_host_base_address;
extern uint32_t SimbleeGZLL_device_base_address;

extern void SimbleeGZLL_onReceive(device_t device, int rssi, char *data, int len)  __attribute__((weak));

// 0 = success
// 1 = init failed
// 2 = set tx_power failed
// 3 = enabled failed
// 4 = set channel selection policy failed
// 5 = failed to set host base address
// 6 = failed to set devive base address
int SimbleeGZLL_begin(device_t device);

void SimbleeGZLL_end(void);

// max length is 32 (NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH)
bool SimbleeGZLL_send_to_host(const char *data, int len);

bool SimbleeGZLL_send_to_device(device_t device, const char *data, int len);

#ifdef __cplusplus
}
#endif

#endif
