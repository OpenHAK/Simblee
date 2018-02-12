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

#ifndef _SIMBLEEAESCOM_H_
#define _SIMBLEEAESCOM_H_

// SimbleCOM with 128-bit AES CCM (Counter with CBC-MAC) Encryption/Decryption

#include "../SimbleeAES/SimbleeAES.h"
#include "../SimbleeCOM/SimbleeCOM.h"

#define  DEFAULT_FLASH_PAGE  251

// packet counter (CTR mode combines the IV and pktctr into the full 128-bit nonce)
// NOTE: the nonce should never be used more than once for a given key or the security can be compromised.
// for uint32_t, the max payload size is 7 bytes
// uint16_t can be used to increase max payload size to 9 bytes if no more than 65535 messages are needed
typedef uint32_t pktctr_t;

// evenly distribute 10,000 flash updates over full range of pktctr_t
#define  DEFAULT_FLASH_RATE  ((pktctr_t)-1 / 10000)

typedef struct
{
  unsigned int esn;
  uint8_t iv[8];
} simbleeaescom_t;

class SimbleeAESCOMClass : public SimbleeCOMClass
{
private:
  static pktctr_t send_pktctr;
  static uint32_t my_esn;
  
public:
  uint8_t *key;
  simbleeaescom_t *info;
  uint32_t count;

  uint32_t flash_page;
  uint32_t flash_rate;

private:
  friend void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi);
  void onReceive(unsigned int esn, const char *payload, int len, int rssi);

public:
  SimbleeAESCOMClass();
  
  void begin();
  void end();
  
  bool send(const char *data, int len);
};

__attribute__((weak))
void SimbleeAESCOM_onReceive(
  unsigned int info_index,
  const char *payload,
  int len,
  int rssi);

extern SimbleeAESCOMClass SimbleeAESCOM;

#endif
