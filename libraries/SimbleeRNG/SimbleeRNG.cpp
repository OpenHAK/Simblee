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
#include "SimbleeRNG.h"

void SimbleeRNG::generate(uint8_t *stream, uint32_t len)
{
  NRF_RNG->TASKS_STOP = 1;
  
  NRF_RNG->CONFIG = (RNG_CONFIG_DERCEN_Enabled << RNG_CONFIG_DERCEN_Pos);
  
  NRF_RNG->TASKS_START = 1;

  while (len-- > 0)
  {
    NRF_RNG->EVENTS_VALRDY = 0;
    while (! NRF_RNG->EVENTS_VALRDY)
      ;
    *stream++ = NRF_RNG->VALUE;
  }
  
  NRF_RNG->TASKS_STOP = 1;
}
