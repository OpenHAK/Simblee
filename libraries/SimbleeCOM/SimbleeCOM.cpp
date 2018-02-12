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

#include "SimbleeCOM.h"

void SimbleeCOMClass::SWI0_Handler(void)
{
  SimbleeCOM_event_t event;
  if (SimbleeCOM_get_rx_event(&event, sizeof(event)))
  {
    if (SimbleeCOM_onReceive)
      SimbleeCOM_onReceive(
        event.esn,
        (const char *)event.payload,
        event.len,
        event.rssi);
  }
}

SimbleeCOMClass::SimbleeCOMClass()
{
  mode = LONG_RANGE;
  txPowerLevel = +4;

  dynamic_attachInterrupt(SWI0_IRQn, SWI0_Handler);
  NVIC_SetPriority(SWI0_IRQn, 3);
}

unsigned int SimbleeCOMClass::getESN()
{
  return Simblee_getESN();
}

void SimbleeCOMClass::proximityMode(bool enable)
{
  if (enable)
    NRF_GPIO->OUTSET = (1 << 31);
  else
    NRF_GPIO->OUTCLR = (1 << 31);
}

void SimbleeCOMClass::begin()
{
  SimbleeCOM_mode = mode;
  SimbleeCOM_tx_power_level = txPowerLevel;
  
  SimbleeCOM_begin();
}

void SimbleeCOMClass::end()
{
  SimbleeCOM_end();
}

bool SimbleeCOMClass::send(const char *data, int len)
{
  return SimbleeCOM_tx((const uint8_t*)data, len);
}

void SimbleeCOMClass::startReceive()
{
  SimbleeCOM_start_rx();
}

void SimbleeCOMClass::stopReceive()
{
  SimbleeCOM_stop_rx();
}

SimbleeCOMClass SimbleeCOM;
