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

#ifndef _SIMBLEEAES_H_
#define _SIMBLEEAES_H_

#include <stdint.h>
#include <stdbool.h>

#define  MAXPLAINTEXT  27
#define  MICSIZE  4

// 128-bit AES CCM (Counter with CBC-MAC) Encryption/Decryption

class SimbleeAES
{
public:
  static uint8_t encrypt(const uint8_t *key, const uint8_t *iv, uint64_t pktctr, uint8_t direction, const uint8_t *plainText, uint8_t plainTextLen, uint8_t *cypherText);
  static uint8_t decrypt(const uint8_t *key, const uint8_t *iv, uint64_t pktctr, uint8_t direction, const uint8_t *cypherText, uint8_t cypherTextLen, uint8_t *plainText);
};

#endif
