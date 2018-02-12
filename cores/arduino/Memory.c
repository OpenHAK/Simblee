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

#include "variant.h"

#include "Memory.h"

int ramUsed(void)
{
  return (uint32_t)&_end - (uint32_t)&_srelocate;
}

int flashUsed(void)
{
  return (uint32_t)&_etextrelocate - (uint32_t)&_sfixed;
}

int flashPageErase( uint8_t page )
{
  // prevent erase of reserved pages
  if (page < 124 || page > 251)
    return 1;

  // page contains sketch
  if (page <= PAGE_FROM_ADDRESS(&_etextrelocate))
    return 2;

  simblee_flash_erase(page);

  // success
  return 0;
}

int flashWrite( uint32_t *address, uint32_t value )
{
  uint8_t page = PAGE_FROM_ADDRESS(address);

  // prevent erase of reserved pages
  if (page < 124 || page > 251)
    return 1;

  // page contains sketch
  if (page <= PAGE_FROM_ADDRESS(&_etextrelocate))
    return 2;

  simblee_flash_write(address, value);

  // success
  return 0;
}

int flashWriteBlock( void *dst, const void *src, int cb )
{
  uint32_t *d = dst;
  const uint32_t *s = src;
  int rc;

  // convert count of bytes to count of uint32_t
  int n = (cb >> 2);

  // round up if remainder
  if (cb & 0x02)
    n++;

  // copy block one uint32_t at a time
  while (n-- > 0)
  {
    rc = flashWrite(d++, *s++);
    if (rc != 0)
      return rc;
  }

  return 0;
}
