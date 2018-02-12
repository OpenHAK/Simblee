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

#include "SimbleeAESCOM.h"

#include "../SimbleeAES/SimbleeAES.cpp"
#include "../SimbleeCOM/SimbleeCOM.cpp"

pktctr_t SimbleeAESCOMClass::send_pktctr = (pktctr_t)0xffffffff;
uint32_t SimbleeAESCOMClass::my_esn = SimbleeCOMClass::getESN();

SimbleeAESCOMClass::SimbleeAESCOMClass()
{
  key = NULL;
  info = NULL;
  count = 0;
  
  flash_page = DEFAULT_FLASH_PAGE;
  flash_rate = DEFAULT_FLASH_RATE;
  
  if (! flash_rate)
    flash_rate = 1;  
}

void SimbleeAESCOMClass::begin()
{
  if (send_pktctr == (pktctr_t)0xffffffff)
    send_pktctr = (pktctr_t)*ADDRESS_OF_PAGE(flash_page);

  SimbleeCOMClass::begin();
}

void SimbleeAESCOMClass::end()
{
  SimbleeCOMClass::end();
}

bool SimbleeAESCOMClass::send(const char *data, int len)
{
  // find esn
  int i;
  for (int i = 0; i < count; i++)
    if (info[i].esn == my_esn)
      break;
  if (i == count)
    return false;

  send_pktctr++;

  // since the default flash page state is 0xffffffff, this will force a write on first use
  if ((send_pktctr % flash_rate) == 0)
  {
    flashPageErase(flash_page);
    flashWrite(ADDRESS_OF_PAGE(flash_page), send_pktctr + flash_rate - 1);
  }

  // <pktctr> <variable data> <4 byte mic>
  uint8_t buf[15];
  memset(buf, 0, sizeof(buf));

  // put pktctr
  *(pktctr_t*)buf = send_pktctr;

  // truncate to max bytes
  if (len > 15 - MICSIZE - sizeof(pktctr_t))
    len = 15 - MICSIZE - sizeof(pktctr_t);

  uint8_t direction = 0;
    
  // encrypt payload
  uint8_t rc = SimbleeAES::encrypt(key, info[i].iv, send_pktctr, direction, (const uint8_t *)data, len, buf + sizeof(pktctr_t));
  if (rc != len + MICSIZE)
    return false;

  // send
  SimbleeCOM.send((const char *)buf, len + sizeof(pktctr_t) + MICSIZE);
  
  return true;
}

void SimbleeAESCOMClass::onReceive(unsigned int esn, const char *payload, int len, int rssi)
{
  // must contain at least pktctr and 4 byte mic
  if (len < sizeof(pktctr_t) + MICSIZE)
    return;
  
  // find esn
  int i;
  for (int i = 0; i < count; i++)
    if (info[i].esn == esn)
      break;
  if (i == count)
    return;

  uint8_t buf[15 - MICSIZE - sizeof(pktctr_t)];

  // get pktctr
  pktctr_t pktctr = *(pktctr_t*)payload;

  uint8_t direction = 0;

  // decrypt payload
  uint8_t rc = SimbleeAES::decrypt(key, info[i].iv, pktctr, direction, (const uint8_t *)payload + sizeof(pktctr_t), len - sizeof(pktctr_t), buf);
  if (rc != len - MICSIZE - sizeof(pktctr_t))
    return;

  // callback
  if (SimbleeAESCOM_onReceive)
    SimbleeAESCOM_onReceive(i, (char *)buf, len - MICSIZE - sizeof(pktctr_t), rssi);
}

void SimbleeCOM_onReceive(unsigned int esn, const char *payload, int len, int rssi)
{
  SimbleeAESCOM.onReceive(esn, payload, len, rssi);
}

SimbleeAESCOMClass SimbleeAESCOM;
