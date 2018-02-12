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
#include "SimbleeBLE.h"

volatile int& SimbleeBLEClass::radioActive = SimbleeBLE_radioActive;

SimbleeBLEClass::SimbleeBLEClass()
{
  ::SimbleeBLE_used = 1;

  customUUID = "";
	deviceName = "Simblee";
	advertisementData = "sketch";

  manufacturerName = NULL;
  modelNumber = NULL;
  serialNumber = NULL;
  hardwareRevision = NULL;
  softwareRevision = NULL;

  iBeacon = false;
  // iBeacon Proximity UUID
  uint8_t uuid[16] = {0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2, 0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0};
  memcpy(iBeaconUUID, uuid, sizeof(iBeaconUUID));
  iBeaconMajor = 0;
  iBeaconMinor = 0;
  iBeaconMeasuredPower = 0xC5;
	advertisementInterval = 80;
	txPowerLevel = +4;
  connectable = true;
}

void SimbleeBLEClass::begin()
{
  // declared in variant.h
  extern bool override_uart_limit;

  if (! override_uart_limit)
  {
    if (UART0_State != UART0_State_NotStarted && UART0_BaudRate() > 9600)
    {
      const char *error = "BLE + UART > 9600 baud not permitted due to critical BLE timing requirements.\r\n"
        "To override, add: override_uart_limit = true; to the top of setup() in your sketch.";

      // attempt to notify user of error condition
      const char *p = error;
      while (*p)
        UART0_TX(*p++);

      // don't continue
      while (1)
        ;
    }
  }

  SimbleeBLE_custom_uuid = customUUID;
  SimbleeBLE_device_name = deviceName;
  SimbleeBLE_advertisement_data = advertisementData;

  SimbleeBLE_manufacturer_name = manufacturerName;
  SimbleeBLE_model_number = modelNumber;
  SimbleeBLE_serial_number = serialNumber;
  SimbleeBLE_hardware_revision = hardwareRevision;
  SimbleeBLE_software_revision = softwareRevision;

  SimbleeBLE_ibeacon = iBeacon;
  memcpy(SimbleeBLE_ibeacon_uuid, iBeaconUUID, sizeof(SimbleeBLE_ibeacon_uuid));
  SimbleeBLE_ibeacon_major = iBeaconMajor;
  SimbleeBLE_ibeacon_minor = iBeaconMinor;
  SimbleeBLE_ibeacon_measured_power = iBeaconMeasuredPower;
  SimbleeBLE_advertisement_interval = advertisementInterval;
  SimbleeBLE_tx_power_level = txPowerLevel;
  SimbleeBLE_connectable = connectable;

  SimbleeBLE_begin();
}

void SimbleeBLEClass::end()
{
  SimbleeBLE_end();
}

bool SimbleeBLEClass::send(char data)
{
	return SimbleeBLE_send(&data, 1);
}

bool SimbleeBLEClass::send(const char *data, int len)
{
	return SimbleeBLE_send(data, len);
}

bool SimbleeBLEClass::sendByte(uint8_t data)
{
  return SimbleeBLE_send((char*)&data, 1);
}

bool SimbleeBLEClass::sendInt(int data)
{
  return SimbleeBLE_send((char*)&data, sizeof(int));
}

bool SimbleeBLEClass::sendFloat(float data)
{
  return SimbleeBLE_send((char*)&data, sizeof(float));
}

void SimbleeBLEClass::updateConnInterval(int min_conn_interval_ms, int max_conn_interval_ms)
{
  SimbleeBLE_update_conn_interval(min_conn_interval_ms, max_conn_interval_ms);
}

int SimbleeBLEClass::getConnInterval(void)
{
  return SimbleeBLE_get_conn_interval();
}

#include "../SimbleeCOM/SimbleeCOM.cpp"

// dual mode timeslot start/stop (do not exceed 10ms!)
void SimbleeBLEClass::SWI3_Handler(void)
{
  // dual mode start event
  if (SimbleeBLE_dualModeEvent == 1)
  {
    SimbleeCOM.mode = LOW_LATENCY;
    SimbleeCOM.begin();
    
    if (SimbleeBLE_onDualModeStart)
      SimbleeBLE_onDualModeStart();
  }
  // dual mode stop event
  else if (SimbleeBLE_dualModeEvent == 2)
  {
    if (SimbleeBLE_onDualModeStop)
      SimbleeBLE_onDualModeStop();
      
    SimbleeCOM.end();
  }
}

void SimbleeBLEClass::proximityMode(bool enable)
{
  if (enable)
    NRF_GPIO->OUTSET = (1 << 31);
  else
    NRF_GPIO->OUTCLR = (1 << 31);
}

void SimbleeBLEClass::dualModeBegin(void)
{
  dynamic_attachInterrupt(SWI3_IRQn, SWI3_Handler);
  NVIC_SetPriority(SWI3_IRQn, 3);
  
  SimbleeBLE_dualModeBegin();
}

void SimbleeBLEClass::dualModeEnd(void)
{
  SimbleeBLE_dualModeEnd();
}

SimbleeBLEClass SimbleeBLE;
