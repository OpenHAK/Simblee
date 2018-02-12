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

#ifndef _LIBSIMBLEE_H_
#define _LIBSIMBLEE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MILLISECONDS(x) ((uint64_t)x)
#define SECONDS(x)      (x * MILLISECONDS(1000))
#define MINUTES(x)      (x * SECONDS(60))
#define HOURS(x)        (x * MINUTES(60))
#define DAYS(x)         (x * HOURS(24))
#define INFINITE (-1)

#define CELSIUS 0
#define FAHRENHEIT 1

#define LOW 0
#define HIGH 1
#define DISABLE 5

extern int SimbleeBLE_used;
extern int SimbleeGZLL_used;
extern int SimbleeBLE_enabled;
extern int SimbleeGZLL_enabled;

extern int ota_bootloader_used;

// pass the pin number so you can use the same interrupt handler for all pins
// return 0 to reset wake_detect bit (and not exit Simblee_ULPDelay)
// return 1 to leave wake_detect bit alone (and exit Simblee_ULPDelay)
typedef int (*pin_callback_t)(uint32_t ulPin);
typedef int (*pin_void_callback_t)(void);

/**
 * \brief Ultra low power delay for the amount of time (in milliseconds) specified as parameter.
 *        You can use the following macros: MILLISECONDS(x), SECONDS(x), MINUTES(x), HOURS(x), DAY(x), INFINITE
 *        ie: Simblee_ULPDelay( MINUTES(2) + SECONDS(30) )
 *
 *        Simblee_pinWake() can be used to cause HIGH / LOW edges on GPIO pins to exit ulp_delay.
 *        see documentation for Simblee_pinWake for more details.
 *
 * \param ms the number of milliseconds to switch to ultra low power (max is DAYS(7) unless INFINITE)
 */
extern void Simblee_ULPDelay( uint64_t ms );

/**
 * \brief Call Simblee_ULPDelay_Exit() from a callback to make Simblee_ULPDelay() exit in the loop.
 *        Callbacks include SimbleeBLE_onConnect(), SimbleeBLE_onReceive(), etc.
 */
extern void Simblee_ULPDelay_Exit();

// the same function is used for both BLE and NonBLE (so alias it both ways)
#define SimbleeBLE_ULPDelay(x) Simblee_ULPDelay(x)

/**
 * \brief Configures the pin to wake the device from system off.
 *
 * \param ulPin The number of the digital pin you want to read (int)
 * \param dwWake Either DISABLED, HIGH or LOW
 */
extern void Simblee_pinWake( uint32_t ulPin, uint32_t dwWake );

/**
 * \brief Test whether a pin caused us to wakeup and exit ulp_delay
 *
 * \param ulPin The number of the digital pin you want to read (int)
 *
 *        You must call Simblee_resetPinWoke() to reset this condition after processing
 */
extern int Simblee_pinWoke( uint32_t ulPin );

/**
 * \brief Reset state of pin that caused us to wakeup and exit ulp_delay
 *
 * \param ulPin The number of the digital pin you want to reset
 */
extern void Simblee_resetPinWake( uint32_t ulPin );

/**
 * \brief Configures the pin to wake the device from system off and execute a callback.
 *
 * \param ulPin The number of the digital pin you want to read (int)
 * \param dwWake Either DISABLED, HIGH or LOW
 * \param callback The callback function to execute when the pin is detected.
 *                 Here is an example pin callback function:
 *                 int my_pin_callback(uint32_t pin)
 *                 {
 *                   // do something
 *                   // return 0 to reset wake_detect, return 1 to exit Simblee_ULPDelay
 *                   return 0;
 *                 }
 *                 To call this function on the when pin 5 goes high:
 *                 Simblee_pinWake(5, HIGH, my_pin_callback);
 */
extern void Simblee_pinWakeCallback( uint32_t ulPin, uint32_t dwWake, pin_callback_t callback ) ;
extern void Simblee_pinWakeVoidCallback( uint32_t ulPin, uint32_t dwWake, pin_void_callback_t callback ) ;

/**
 * \brief Reset the system
 */
extern void Simblee_systemReset(void);

/**
 * \brief Turn the system off
 */
extern void Simblee_systemOff(void);

/**
 * \brief Get a sample from the temperature sensor
 *
 * \param scale Either CELSIUS or FAHRENHEIT
 *
 * returns:
 *     degrees f (-198.00 to +260.00)
 *     degrees c (-128.00 to +127.00)
 */
extern float Simblee_temperature(int scale);

void simblee_ppi_channel_assign(uint8_t channel_num, const volatile void * event_endpoint, const volatile void * task_endpoint);
void simblee_ppi_channel_unassign(uint8_t channel_num);

void simblee_nvic_enableirq(uint8_t irq_num);
void simblee_nvic_enableirq_priority(uint8_t irq_num, uint8_t priority);
void simblee_nvic_disableirq(uint8_t irq_num);

void simblee_nvic_setpriority(uint8_t irq_num, uint32_t priority);

// 1 if the BLE radio is active (or about to become active), 0 if not
extern volatile int SimbleeBLE_radioActive;

/* Get the unique factory assigned ESN for the device.
 *
 * Returns:
 *   The unique factory assigned ESN.
 */
uint32_t Simblee_getESN();

void simblee_flash_erase(uint8_t page);
void simblee_flash_write(uint32_t *address, uint32_t value);

// 0=power-on-reset or brown out reset
// 1=reset from low power comparitor
// 2=reset from cpu lock-up detected
// 3=reset from debug interface mode
// 4=reset from Simblee_systemReset()
// 5=reset from pinWake during Simblee_systemOff()
// 6=reset from pin reset
int Simblee_resetReason(void);

void Simblee_setWatchDogTimer(int ms);
void Simblee_resetWatchDogTimer(void);

#ifdef __cplusplus
}
#endif

#endif
