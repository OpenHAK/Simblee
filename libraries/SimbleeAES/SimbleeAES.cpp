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
#include "SimbleeAES.h"

typedef struct
{
  uint8_t key[16];
  uint64_t pktctr;
  uint8_t direction;
  uint8_t iv[8];
} ccm_t;

typedef struct
{
  uint8_t header;
  uint8_t length;
  uint8_t rfu;
  uint8_t payload[MAXPLAINTEXT + MICSIZE];
} in_t;

typedef struct
{
  uint8_t header;
  uint8_t length;
  uint8_t rfu;
  uint8_t payload[MAXPLAINTEXT + MICSIZE];
} out_t;

// plainTextLen is truncated at 27 bytes
// adds the 4 byte Message Integrity Check (MIC)
uint8_t SimbleeAES::encrypt(const uint8_t *key, const uint8_t *iv, uint64_t pktctr, uint8_t direction, const uint8_t *plainText, uint8_t plainTextLen, uint8_t *cypherText)
{
  ccm_t ccm;

  in_t in;
  out_t out;
  
  uint8_t scratch[43];
  
  memset(&ccm, 0, sizeof(ccm));
  
  memcpy(ccm.key, key, sizeof(ccm.key));
  memcpy(ccm.iv, iv, sizeof(ccm.iv));
  ccm.pktctr = pktctr;
  ccm.direction = direction;
  
  NRF_CCM->ENABLE = (CCM_ENABLE_ENABLE_Enabled << CCM_ENABLE_ENABLE_Pos);
  
  NRF_CCM->CNFPTR = (uint32_t)&ccm;

  NRF_CCM->SCRATCHPTR = (uint32_t)scratch;  
  
  NRF_CCM->EVENTS_ENDKSGEN = 0;
  NRF_CCM->TASKS_KSGEN = 1;
  while (! NRF_CCM->EVENTS_ENDKSGEN)
    ;

  memset(&in, 0, sizeof(in));
  memset(&out, 0, sizeof(out));
  
  NRF_CCM->INPTR = (uint32_t)&in;
  NRF_CCM->OUTPTR = (uint32_t)&out;

  NRF_CCM->MODE = (CCM_MODE_MODE_Encryption << CCM_MODE_MODE_Pos);  

  if (plainTextLen > 27)
    plainTextLen = 27;

  in.length = plainTextLen;
  memcpy(in.payload, plainText, plainTextLen);
  
  NRF_CCM->EVENTS_ERROR = 0;

  NRF_CCM->EVENTS_ENDCRYPT = 0;
  NRF_CCM->TASKS_CRYPT = 1;
  while (! NRF_CCM->EVENTS_ENDCRYPT)
    ;

  uint8_t cypherTextLen = 0;
      
  if (! NRF_CCM->EVENTS_ERROR)
    if (memcmp(in.payload, out.payload, plainTextLen) != 0)
    {
      cypherTextLen = out.length;
      memcpy(cypherText, out.payload, cypherTextLen);
    }

  NRF_CCM->ENABLE = (CCM_ENABLE_ENABLE_Disabled << CCM_ENABLE_ENABLE_Pos);
  
  return cypherTextLen;
}

// drops the 4 byte Message Integrity Check (MIC)
uint8_t SimbleeAES::decrypt(const uint8_t *key, const uint8_t *iv, uint64_t pktctr, uint8_t direction, const uint8_t *cypherText, uint8_t cypherTextLen, uint8_t *plainText)
{
  ccm_t ccm;

  in_t in;
  out_t out;
  
  uint8_t scratch[43];
  
  memset(&ccm, 0, sizeof(ccm));
  
  memcpy(ccm.key, key, sizeof(ccm.key));
  memcpy(ccm.iv, iv, sizeof(ccm.iv));
  ccm.pktctr = pktctr;
  ccm.direction = direction;

  NRF_CCM->ENABLE = (CCM_ENABLE_ENABLE_Enabled << CCM_ENABLE_ENABLE_Pos);

  NRF_CCM->CNFPTR = (uint32_t)&ccm;

  NRF_CCM->SCRATCHPTR = (uint32_t)scratch;
  
  NRF_CCM->EVENTS_ENDKSGEN = 0;
  NRF_CCM->TASKS_KSGEN = 1;
  while (! NRF_CCM->EVENTS_ENDKSGEN)
    ;

  memset(&in, 0, sizeof(in));
  memset(&out, 0, sizeof(out));
  
  NRF_CCM->INPTR = (uint32_t)&in;
  NRF_CCM->OUTPTR = (uint32_t)&out;

  NRF_CCM->MODE = (CCM_MODE_MODE_Decryption << CCM_MODE_MODE_Pos);  

  if (cypherTextLen > MAXPLAINTEXT + MICSIZE)
    cypherTextLen = MAXPLAINTEXT + MICSIZE;

  in.length = cypherTextLen;
  memcpy(in.payload, cypherText, cypherTextLen);
  
  NRF_CCM->EVENTS_ERROR = 0;

  NRF_CCM->EVENTS_ENDCRYPT = 0;
  NRF_CCM->TASKS_CRYPT = 1;
  while (! NRF_CCM->EVENTS_ENDCRYPT)
    ;

  uint8_t plainTextLen = 0;

  if (! NRF_CCM->EVENTS_ERROR)
    if (NRF_CCM->MICSTATUS == 1)
    {
      plainTextLen = out.length;
      memcpy(plainText, out.payload, plainTextLen);
    }

  NRF_CCM->ENABLE = (CCM_ENABLE_ENABLE_Disabled << CCM_ENABLE_ENABLE_Pos);
  
  return plainTextLen;
}
