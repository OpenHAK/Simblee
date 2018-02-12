/*
 * Modified by RF Digital Corp. 2015.
 * www.RFDigital.com
 * 
 * THE SOURCE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
 */

/*
  Copyright (c) 2011-2012 Arduino.  All right reserved.

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

#include "WInterrupts.h"

void attachInterrupt(uint32_t pin, callback_t callback, uint32_t mode)
{
  pinMode(pin, INPUT);
  Simblee_pinWakeVoidCallback(pin, mode, callback);
}

void detachInterrupt(uint32_t pin)
{
  Simblee_pinWakeVoidCallback(pin, DISABLE, NULL);
}

void dynamic_attachInterrupt(uint8_t IRQn, callback_t callback)
{
  dynamic_handlers[IRQn] = callback;
  simblee_nvic_enableirq(IRQn);
}

void dynamic_attachInterrupt_priority(uint8_t IRQn, callback_t callback, uint8_t priority)
{
  dynamic_handlers[IRQn] = callback;
  simblee_nvic_enableirq_priority(IRQn, priority);
}

void dynamic_detachInterrupt(uint8_t IRQn)
{
  simblee_nvic_disableirq(IRQn);
  dynamic_handlers[IRQn] = NULL;
}

void attachPinInterrupt(uint32_t pin, pin_callback_t callback, uint32_t mode)
{
  Simblee_pinWakeCallback(pin, mode, callback);
}

void detachPinInterrupt(uint32_t pin)
{
  Simblee_pinWakeCallback(pin, DISABLE, NULL);
}
