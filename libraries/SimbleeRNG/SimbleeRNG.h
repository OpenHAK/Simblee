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

#ifndef _SIMBLEERNG_H_
#define _SIMBLEERNG_H_

#include <stdint.h>
#include <stdbool.h>

// Generate a true non-deterministic random number stream based on internal thermal noise

class SimbleeRNG
{
public:
  static void generate(uint8_t *stream, uint32_t len);
};

#endif
