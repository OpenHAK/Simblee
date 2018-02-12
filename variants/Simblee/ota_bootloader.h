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

#ifndef _OTA_BOOTLOADER_
#define _OTA_BOOTLOADER_

#include "Arduino.h"

#ifndef OTA_BOOTLOADER_TYPE
#define  OTA_BOOTLOADER_TYPE  dual_bank
#endif

// force double expansion
#define  _OTA_BOOTLOADER_NAME(x)  ota_bootloader_ ## x
#define  OTA_BOOTLOADER_NAME(x)  _OTA_BOOTLOADER_NAME(x)

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

KEEPTEXT void OTA_BOOTLOADER_NAME(OTA_BOOTLOADER_TYPE)() { }

int ota_bootloader_used = 1;

extern void ota_bootloader_start(void);

#ifdef __cplusplus
}
#endif

#endif  // _OTA_BOOTLOADER_