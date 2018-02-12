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

#ifndef _LIBSIMBLEEBLE_H_
#define _LIBSIMBLEEBLE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//
// The firmware version exposed via Device Information.
// This isn't exposed to the Simblee modules and can not
// be overriden
//
#define FIRMWARE_VERSION "1.1.0"

// NULL or "": default uuid (0xfe84, Bluteooth SIG allocated 16-bit uuid for Simblee service)
// "2220": original uuid (0x2220, for backward compatibility - depreciated - please use the default uuid instead)
// "xxxx": 16-bit service uuid (allocated by the Bluetooth SIG)
// "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx": 128-bit uuid
extern const char *SimbleeBLE_custom_uuid;

// default "Simblee"
extern const char *SimbleeBLE_device_name;

// default "sketch"
extern const char *SimbleeBLE_advertisement_data;

//
// Device Information
//
extern const char *SimbleeBLE_manufacturer_name;
extern const char *SimbleeBLE_model_number;
extern const char *SimbleeBLE_serial_number;
extern const char *SimbleeBLE_hardware_revision;
extern const char *SimbleeBLE_software_revision;

// in ms (converted to 0.625ms units) - range 20ms to 10.24s - default 80ms
extern int SimbleeBLE_advertisement_interval;

// default false (false = regular advertising, true = ibeacon advertising)
extern bool SimbleeBLE_ibeacon;

// default iBeacon Proximity UUID
extern uint8_t SimbleeBLE_ibeacon_uuid[16];

// default 0
extern unsigned short SimbleeBLE_ibeacon_major;

// default 0
extern unsigned short SimbleeBLE_ibeacon_minor;

// default 2's complement iBeacon Power Measurement at 1 meter (default is 0xC5 = -59dBm)
extern unsigned char SimbleeBLE_ibeacon_measured_power;

// -20 dBm to +4 dBm - default +4 dBm
extern int8_t SimbleeBLE_tx_power_level;

// use with caution: advanced raw access to the advertisement and scan response packets
extern uint8_t *SimbleeBLE_advdata;
extern uint8_t SimbleeBLE_advdata_len;
extern uint8_t *SimbleeBLE_srdata;
extern uint8_t SimbleeBLE_srdata_len;

// default true (false = nonconnectable advertising)
// nonconnectable forces minimum of 100ms advertisement_interval
extern bool SimbleeBLE_connectable;

extern void SimbleeBLE_onAdvertisement(bool start)     __attribute__((weak));
// received signal strength indication (-0dBm to -127dBm)
extern void SimbleeBLE_onRSSI(int rssi)                __attribute__((weak));
extern void SimbleeBLE_onConnect(void)                 __attribute__((weak));
extern void SimbleeBLE_onDisconnect(void)              __attribute__((weak));
extern void SimbleeBLE_onReceive(char *data, int len)  __attribute__((weak));

// signal numbers 0-15 reserved for use by Simblee
//     1 = Disconnect
// signal numbers 16-31 reserved for use by application
// signal numbers 31-255 reserved for future use
// (return true to continue processing)
extern bool SimbleeBLE_onSignal(int number, char *data, int len)  __attribute__((weak));

// Does a best effort to accomodate the users request.
// This means that it is no longer necessary to check the return value, and being off
// by one byte doesn't stop the device from advertising.
// on advertisement packet overflow, it will truncate the advertisement data first down
// to a single byte, then it will truncate the device name.  if the tx_power_level is
// outside the limits, its modified to the boundary level value.  the same boundary logic
// is also applied to the advertisement_interval.
void SimbleeBLE_begin(void);

void SimbleeBLE_end(void);

bool SimbleeBLE_send(const char *data, uint8_t len);

// request central role use a different connection interval in the given range
// the central role may reject the request (or even pick a value outside the range)
void SimbleeBLE_update_conn_interval(int min_conn_interval_ms, int max_conn_interval_ms);

int SimbleeBLE_get_conn_interval(void);

extern uint8_t SimbleeBLE_dualModeEvent;
void SimbleeBLE_dualModeBegin(void);
void SimbleeBLE_dualModeEnd(void);

void SimbleeBLE_requestHFCLK(void);
void SimbleeBLE_releaseHFCLK(void);
void SimbleeBLE_checkHFCLK(uint32_t* p_is_running);

#ifdef __cplusplus
}
#endif

#endif
