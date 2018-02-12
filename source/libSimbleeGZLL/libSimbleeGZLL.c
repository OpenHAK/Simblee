/*
The Gazell protocol is a wireless communication protocol that is used
to setup a robust wireless link between a single host and up to eight
devices in a star network topology.

The Host in a Gazell network is always listening, and it is the Device
that always initiates a communication.

Each packet that a Device sends is required to be acknowledged by the
Host.  Gazell auotmatically handle packet retransmission if necessary.

It is possible for the Host to send data to the Device by piggybacking
data to an acknowledgement (ACK) packet.  Therefore a Host has to wait
for a packet from a Device before it can send any data to it.

Gazell utilizes channel hopping functionality that gives a high date
rate and reliable wireless link.

Gazell requires no connection packets to setup a link, and devices can
enter and exit from the network at any time.
*/

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

#include <stdio.h>

#include "nrf_gzll.h"

#include "libSimbleeGZLL.h"

extern int SimbleeGZLL_enabled;

// -20 dBm to +4 dBm = default +4 dBm
int SimbleeGZLL_tx_power_level = 4;
uint32_t SimbleeGZLL_host_base_address = 0U;
uint32_t SimbleeGZLL_device_base_address = 0U;

static device_t _device;


// 0 = success
// 1 = init failed
// 2 = set tx_power failed
// 3 = enabled failed
// 4 = set channel selection policy failed
int SimbleeGZLL_begin(device_t device)
{
  _device = device;

  if (! nrf_gzll_init(device == HOST ? NRF_GZLL_MODE_HOST : NRF_GZLL_MODE_DEVICE))
    return 1;

  nrf_gzll_tx_power_t tx_power;
  if (SimbleeGZLL_tx_power_level <= -20)
    tx_power = NRF_GZLL_TX_POWER_N20_DBM;
  else if (SimbleeGZLL_tx_power_level <= -16)
    tx_power = NRF_GZLL_TX_POWER_N16_DBM;
  else if (SimbleeGZLL_tx_power_level <= -12)
    tx_power = NRF_GZLL_TX_POWER_N12_DBM;
  else if (SimbleeGZLL_tx_power_level <= -16)
    tx_power = NRF_GZLL_TX_POWER_N16_DBM;
  else if (SimbleeGZLL_tx_power_level <= -8)
    tx_power = NRF_GZLL_TX_POWER_N8_DBM;
  else if (SimbleeGZLL_tx_power_level <= -4)
    tx_power = NRF_GZLL_TX_POWER_N4_DBM;
  else if (SimbleeGZLL_tx_power_level <= 0)
    tx_power = NRF_GZLL_TX_POWER_0_DBM;
  else
    tx_power = NRF_GZLL_TX_POWER_4_DBM;

  if (! nrf_gzll_set_tx_power(tx_power))
    return 2;

  if (! nrf_gzll_set_device_channel_selection_policy(NRF_GZLL_DEVICE_CHANNEL_SELECTION_POLICY_USE_CURRENT))
    return 4;
 
  if (SimbleeGZLL_host_base_address ) {
   uint8_t msb = SimbleeGZLL_host_base_address >> 24;
   if (msb == 0x55 || msb == 0xAA)
     return 5;  // msb of base address should not be alternating 0s and 1s
	 if ( !nrf_gzll_set_base_address_0(SimbleeGZLL_host_base_address) )
		return 5;
  } 

  if (SimbleeGZLL_device_base_address) {
   uint8_t msb = SimbleeGZLL_device_base_address >> 24;
   if (msb == 0x55 || msb == 0xAA)
     return 6;  // msb of base address should not be alternating 0s and 1s
	 if ( !nrf_gzll_set_base_address_1(SimbleeGZLL_device_base_address) )
		return 6;
  }
	
  if (! nrf_gzll_enable())
    return 3;

  SimbleeGZLL_enabled = 1;

  return 0;
}

bool SimbleeGZLL_send_to_host(const char *data, int len)
{
  if (_device == HOST)
    return false;

  // a zero byte load is valid, but data must be non-null
  if (!data && !len)
    data = &_device;

  return nrf_gzll_add_packet_to_tx_fifo(_device, data, len);
}

bool SimbleeGZLL_send_to_device(device_t device, const char *data, int len)
{
  if (_device != HOST)
    return false;

  return nrf_gzll_add_packet_to_tx_fifo(device, data, len);
}

void SimbleeGZLL_end(void)
{
  nrf_gzll_disable();

  while (nrf_gzll_is_enabled())
    ;

  NVIC_DisableIRQ(RADIO_IRQn);
  NVIC_DisableIRQ(TIMER2_IRQn);

  SimbleeGZLL_enabled = 0;
}

void nrf_gzll_host_rx_data_ready(uint32_t pipe, nrf_gzll_host_rx_info_t rx_info)
{
  char payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
  int length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;

  if (! nrf_gzll_fetch_packet_from_rx_fifo(pipe, payload, &length))
    return;

  if (SimbleeGZLL_onReceive)
     SimbleeGZLL_onReceive(pipe, rx_info.rssi, payload, length);
}

void nrf_gzll_device_tx_success(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
  char payload[NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH];
  int length = 0;

  if (tx_info.payload_received_in_ack)
  {
    length = NRF_GZLL_CONST_MAX_PAYLOAD_LENGTH;
    if (! nrf_gzll_fetch_packet_from_rx_fifo(pipe, payload, &length))
      return;
  }

  if (SimbleeGZLL_onReceive)
     SimbleeGZLL_onReceive(HOST, tx_info.rssi, payload, length);
}

void nrf_gzll_device_tx_failed(uint32_t pipe, nrf_gzll_device_tx_info_t tx_info)
{
}

void nrf_gzll_disabled()
{
}
