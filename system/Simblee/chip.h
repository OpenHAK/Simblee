/*
 * Modified by RF Digital Corp. 2015.
 * www.RFDigital.com
 * 
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 */

/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _LIBSIMBLEESYSTEM_
#define _LIBSIMBLEESYSTEM_

/*
 * Core and peripherals registers definitions
 */
#include "include/startup_nrf51822.h"
#include "include/system_nrf51.h"
#include "include/nrf51.h"
#include "include/nrf51_bitfields.h"

/* Define attribute */
#if defined (  __GNUC__  ) /* GCC CS3 */
    #define WEAK __attribute__ ((weak))
#endif

/* Define NO_INIT attribute */
#if defined (  __GNUC__  )
    #define NO_INIT
#endif

// volatile or __attribute__((used))__ will not retain symbols
#define  KEEPTEXT  __attribute__((section(".keeptext")))
#define  KEEPDATA  __attribute__((section(".keepdata")))
#define  KEEPBSS   __attribute__((section(".keepbss")))

/*
 * Peripherals
 *
 * Here we include all the header files of the peripheral devices.
 * The header files should be in the "include" folder and the
 * source files should be in the "source" folder.
 *
 * ex. #include "include/peripheral_device.h"
 */

#include "include/nrf_gpiote.h"

#endif /* _LIBSIMBLEESYSTEM_ */
