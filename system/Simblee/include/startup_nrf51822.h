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

#ifndef __STARTUP_NRF51822_H
#define __STARTUP_NRF51822_H

#ifdef __cplusplus
extern "C" {
#endif

/* Vector Table definition */
typedef struct
{
  /* Stack pointer */
  void* Stack;

  /* Cortex-M0 core handlers */
  void* Reset_Handler;
  void* NMI_Handler;
  void* HardFault_Handler;
  void* Reserved1_Handler;
  void* Reserved2_Handler;
  void* Reserved3_Handler;
  void* Reserved4_Handler;
  void* Reserved5_Handler;
  void* Reserved6_Handler;
  void* Reserved7_Handler;
  void* SVC_Handler;
  void* Reserved8_Handler;
  void* Reserved9_Handler;
  void* PendSV_Handler;
  void* SysTick_Handler;

  /* Configurable interrupts */
  void* POWER_CLOCK_IRQHandler;    /* 0  Power Clock */
  void* RADIO_IRQHandler;          /* 1  Radio Controller*/
  void* UART0_IRQHandler;          /* 2  UART0 Controller */
  void* SPI0_TWI0_IRQHandler;      /* 3  SPI0 & TWI0 Controller*/
  void* SPI1_TWI1_IRQHandler;      /* 4  SPI1 & TWI1 Controller*/
  void* Reserved10_IRQHandler;     /* 5  Reserved */
  void* GPIOTE_IRQHandler;         /* 6  GPIO */
  void* ADC_IRQHandler;            /* 7  ADC Controller */
  void* TIMER0_IRQHandler;         /* 8  Timer Counter 0 */
  void* TIMER1_IRQHandler;         /* 9  Timer Counter 1 */
  void* TIMER2_IRQHandler;         /* 10 Timer Counter 2 */
  void* RTC0_IRQHandler;           /* 11 Real Time Clock Controller 0 */
  void* TEMP_IRQHandler;           /* 12 Temperature */
  void* RNG_IRQHandler;            /* 13 Random Number Generator */
  void* ECB_IRQHandler;            /* 14 ECB */
  void* CCM_AAR_IRQHandler;        /* 15 CCM_AAR */
  void* WDT_IRQHandler;            /* 16 Watchdog Timer */
  void* RTC1_IRQHandler;           /* 17 Real Time Clock Controller 1 */
  void* QDEC_IRQHandler;           /* 18 QDEC */
  void* LPCOMP_IRQHandler;         /* 19 LPCOMP */
  void* SWI0_IRQHandler;           /* 20 Software Interrupt 0 */
  void* SWI1_IRQHandler;           /* 21 Software Interrupt 1 */
  void* SWI2_IRQHandler;           /* 22 Software Interrupt 2 */
  void* SWI3_IRQHandler;           /* 23 Software Interrupt 3 */
  void* SWI4_IRQHandler;           /* 24 Software Interrupt 4 */
  void* SWI5_IRQHandler;           /* 25 Software Interrupt 5 */
  void* Reserved11_IRQHandler;     /* 26 Reserved */
  void* Reserved12_IRQHandler;     /* 27 Reserved */
  void* Reserved13_IRQHandler;     /* 28 Reserved */
  void* Reserved14_IRQHandler;     /* 29 Reserved */
  void* Reserved15_IRQHandler;     /* 30 Reserved */
  void* Reserved16_IRQHandler;     /* 31 Reserved */
} vector_table_t;

extern const vector_table_t vector_table;

#define DYNAMIC_HANDLERS

/* Cortex-M0 core handlers */
void NMI_Handler        ( void );
void HardFault_Handler  ( void );
void SVC_Handler        ( void );
void PendSV_Handler     ( void );
void SysTick_Handler    ( void );

/* peripheral handlers */
void POWER_CLOCK_IRQHandler ( void );
void RADIO_IRQHandler       ( void );
void UART0_IRQHandler       ( void );
void SPI0_TWI0_IRQHandler   ( void );
void SPI1_TWI1_IRQHandler   ( void );
void GPIOTE_IRQHandler      ( void );
void ADC_IRQHandler         ( void );
void TIMER0_IRQHandler      ( void );
void TIMER1_IRQHandler      ( void );
void TIMER2_IRQHandler      ( void );
void RTC0_IRQHandler        ( void );
void TEMP_IRQHandler        ( void );
void RNG_IRQHandler         ( void );
void ECB_IRQHandler         ( void );
void CCM_AAR_IRQHandler     ( void );
void WDT_IRQHandler         ( void );
void RTC1_IRQHandler        ( void );
void QDEC_IRQHandler        ( void );
void WUCOMP_IRQHandler      ( void );
void SWI0_IRQHandler        ( void );
void SWI1_IRQHandler        ( void );
void SWI2_IRQHandler        ( void );
void SWI3_IRQHandler        ( void );
void SWI4_IRQHandler        ( void );
void SWI5_IRQHandler        ( void );

#ifdef DYNAMIC_HANDLERS

typedef void (*dynamic_handler_t)(void);
extern dynamic_handler_t *dynamic_handlers/*[32]*/;

#endif

#define SVCALL(number, return_type, signature) \
  _Pragma("GCC diagnostic ignored \"-Wreturn-type\"") \
  _Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
  __attribute__((naked)) static return_type signature \
  { \
    __asm( \
        "svc %0\n" \
        "bx r14" : : "I" (number) : "r0" \
    ); \
  }

#ifdef __cplusplus
}
#endif

#endif