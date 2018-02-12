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

#ifndef _LIBSIMBLEECOM_H_
#define _LIBSIMBLEECOM_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*
SimbleeCOM is a highly reliable wifi immune wireless protocol that simply works!

SimbleeCOM uses the RADIO and TIMER0 perpherials.

The peripherals are aquired by SimbleeCOM_begin() and released by SimbleeCOM_end().
Any attempt to use the peripherals directly while aquired by SimbleeCOM will result in a
HardFault exception.
*/

// SimbleeCOM receive event type
typedef struct
{
  uint32_t esn;
  uint8_t payload[15];
  uint8_t len;
  int8_t rssi;
} SimbleeCOM_event_t;

enum { LOW_LATENCY = 1, LONG_RANGE = 2 };

// LONG_RANGE or LOW_LATENCY
extern uint32_t SimbleeCOM_mode;

// -20 dBm to +4 dBm - default +4 dBm
extern int8_t SimbleeCOM_tx_power_level;

/* Perform initialization for SimbleeCOM and start receiving.
 */
void SimbleeCOM_begin(void);

/* SimbleeCOM Transmit payload (max 15 bytes).
 *
 * Inputs:
 *   payload - Pointer to payload buffer.
 *   len - Length of payload.  Zero IS a valid length.
 *
 * Returns:
 *   True if the packet has been queued, False if the queue if full.
 *
 * Notes:
 *   Once the transmission is complete, Receive will be restarted if it was running.
 */
bool SimbleeCOM_tx(const uint8_t *payload, uint8_t len);

/* Start receiving SimbleeCOM packets.
 */
void SimbleeCOM_start_rx(void);

/* Stop receiving SimbleeCOM packets.
 */
void SimbleeCOM_stop_rx(void);

/* Get SimbleeCOM receive event information.
 *
 * Inputs:
 *   event - Pointer to an event buffer.
 *   len - Size of the event buffer.
 *
 * Notes:
 *   Should be called when a receive event is signaled via SWI0.
 */
bool SimbleeCOM_get_rx_event(SimbleeCOM_event_t *event, uint8_t len);

/* Perform termination for SimbleeCOM.
 */
void SimbleeCOM_end(void);

/* optional SimbleeCOM onReceive callback
 */
__attribute__((weak))
void SimbleeCOM_onReceive(
  unsigned int esn,
  const char *payload,
  int len,
  int rssi);

#ifdef __cplusplus
}
#endif

#endif  // _LIBRSIMBLEECOM_H_
