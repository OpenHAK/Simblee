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

#import "Util.h"

char data(NSData *data)
{
    return (char)dataByte(data);
}

uint8_t dataByte(NSData *data)
{
    uint8_t *p = (uint8_t*)[data bytes];
    NSUInteger len = [data length];
    return (len ? *p : 0);
}

int dataInt(NSData *data)
{
    uint8_t *p = (uint8_t*)[data bytes];
    NSUInteger len = [data length];
    return (sizeof(int) <= len ? *(int*)p : 0);
}

float dataFloat(NSData *data)
{
    uint8_t *p = (uint8_t*)[data bytes];
    NSUInteger len = [data length];
    return (sizeof(float) <= len ? *(float*)p : 0);
}
