/** ************************************************************************* *
 * @file hal_hostregs.h
 *
 * @project ADDA
 * Copyright (c) 2015-2017 Heptagon
 *
 * @author BHa <bryant.hansen@mesa-imaging.ch>
 *
 *
 ** ************************************************************************* */

#ifndef _HAL_HOSTREGS_H_
#define _HAL_HOSTREGS_H_

#include "bits.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name Interrupt Control and Status Register (1 byte) - Address 0x00
 * @{
 */
#define ICSR_INT_CLR_CFG            BIT0
#define ICSR_INT_CLR_TYPE           BIT1
#define ICRS_INT_MODE               BIT2
#define ICRS_POLARITY               BIT3
#define ICSR_INT_STATUS_MASK        (BIT4 | BIT5 | BIT6 | BIT7)
#define ICSR_INT_STATUS_SHIFT       4
#define ICSR_INT_RESULT             BIT4
#define ICSR_INT_M2H_REQ            BIT5
#define ICSR_INT_M2H_ACK            BIT6
#define ICSR_INT_RESET              BIT7
/** @} */


/**
 * @name Interrupt Enable Register (1 byte) - Address 0x02
 * @{
 */
#define IER_ENABLE_MASK         (BIT0 | BIT1 | BIT2 | BIT3)
#define IER_ENABLE_SHIFT        0
#define IER_CLEAR_MASK          (BIT4 | BIT5 | BIT6 | BIT7)
#define IER_CLEAR_SHIFT         4
/** @} */

/**
 * @name Command register (1 byte) - Address 0x04
 * @{
 */
#define CMD_TASK_MASK               0x0F
#define CMD_SINGLE_MEASURE          0x01
#define CMD_CONTINUOUS_MEAS_START   0x02
#define CMD_CONTINUOUS_MEAS_STOP    0x03
#define CMD_GO_STANDBY              0x10
#define CMD_GO_MCPU_OFF             0x11
#define CMD_WAKE_UP_MCPU_2_ON_MODE  0x12
/** @TODO determine if we need to declare (and handle) the reset & valid bits */
#define CMD_RESET_MASK              BIT6
#define CMD_VALID_MASK              BIT7
/** @} */

/**
 * @name Device Status Register (2 bytes) - Address 0x06
 * @{
 */


#define DSTATUS_PM_STATUS_STANDBY   0x00
#define DSTATUS_PM_STATUS_MCPU_OFF  0x10  
#define DSTATUS_PM_STATUS_MCPU_ON   0x18  
#define DSTATUS_PM_STATUS_MASK      0x1F  
#define DSTATUS_PM_STATUS_IDLE      0x0100
#define DSTATUS_SINGLE_MEAS_RUNNING 0x0200
#define DSTATUS_CONT_MEAS_RUNNING   0x0300
#define DSTATUS_CONT_MEAS_WAITING   0x0400
#define DSTATUS_CONT_MEAS_STOPPING  0x0500
#define DSTATUS_INIT_STATE_0        0x1000 
#define DSTATUS_INIT_STATE_1        0x1100 
#define DSTATUS_INIT_STATE_2        0x1200
#define DSTATUS_INIT_STATE_3        0x1300
#define DSTATUS_INIT_STATE_4        0x1400

#define DSTATUS_ERR_OTP_CRC         0xE000
#define DSTATUS_ERR_TRIMDATA_CHKSM  0xE100
#define DSTATUS_ERR_WASP_VERSION    0xE200
#define DSTATUS_ERR_EULER_VERSION   0xE300
#define DSTATUS_ERR_CRC_CFG         0xE400
/** @} */

/**
 * @name Result Register (2 bytes) - Address 0x08
 * @{
 */
#define DISTANCE_MM_MASK_OLD        0x1FFC
#define DISTANCE_MM_SHIFT_OLD       2
#define DISTANCE_MM_MASK            0x1FFE
#define DISTANCE_MM_SHIFT           1

#define RSLTR_ERR_CODE_MASK         (BIT14 | BIT13)
#define RSLTR_ERR_CODE_SHIFT        13
#define RSLTR_ERR_CODE_VALID        0x0
#define RSLTR_ERR_CODE_NEAR         0x1
#define RSLTR_ERR_CODE_FAR          0x2
#define RSLTR_ERR_CODE_ERROR        0x3
#define RSLTR_ERR_N_VALID_MASK      (BIT15 | BIT14 | BIT13)
#define RSLTR_VALID                 BIT15
/** @} */


/**
 * @name Result Confidence Register (2 bytes)  - Address 0x0A
 * @{
 */
#define CNFD_VEC_AMPL_MASK          0x7FF0
#define CNFD_VEC_AMPL_SHIFT         4
#define CNFD_PIXEL_VALID_MASK       0x000F
#define CNFD_PIXEL_VALID_SHIFT      0
#define CNFD_REFRESH_ERROR_MASK     BIT15
#define CNFD_REFRESH_ERROR_SHIFT    15
/** @} */

/**
 * @name Refresh Register (2 bytes)  - Address 0x0C
 * @{
 */
#define REFRESH_RATE_MASK           0x007F
#define REFRESH_RATE_SHIFT          0
#define VCSEL_THRESHOLD_MASK        (BIT8  | BIT9  | BIT10 | BIT11)
#define VCSEL_THRESHOLD_SHIFT       8
#define VCSEL_PEAK_MASK             (BIT12 | BIT13 | BIT14 | BIT15)
#define VCSEL_PEAK_SHIFT            12

/** @} */

/**
 * @name VCSEL Driver Peak Current (4 bits)
 * @{
 */
#define I_PK_CFG_08mA               0
#define I_PK_CFG_09mA               1
#define I_PK_CFG_10mA               2
#define I_PK_CFG_11mA               3
#define I_PK_CFG_12mA               4
#define I_PK_CFG_13mA               5
#define I_PK_CFG_14mA               6
#define I_PK_CFG_15mA               7
#define I_PK_CFG_16mA               8
#define I_PK_CFG_17mA               9
#define I_PK_CFG_18mA               10
#define I_PK_CFG_19mA               11
#define I_PK_CFG_20mA               12
#define I_PK_CFG_22mA               13
#define I_PK_CFG_24mA               14
/** @} */


/**
 * @name VCSEL Driver Threshold Current (4 bits)
 * @{
 */
#define I_TH_CFG_0_0mA              0
#define I_TH_CFG_2_0mA              1
#define I_TH_CFG_2_2mA              2
#define I_TH_CFG_2_4mA              3
#define I_TH_CFG_2_6mA              4
#define I_TH_CFG_2_8mA              5
#define I_TH_CFG_3_0mA              6
#define I_TH_CFG_3_2mA              7
#define I_TH_CFG_0_00mA             8
#define I_TH_CFG_4_0mA              9
#define I_TH_CFG_4_4mA              10
#define I_TH_CFG_4_8mA              11
#define I_TH_CFG_5_2mA              12
#define I_TH_CFG_5_6mA              13
#define I_TH_CFG_6_0mA              14
#define I_TH_CFG_6_4mA              15
/** @} */

/**
 * @name Auxiliary Command Configuration Register (2 bytes)  - Address 0x0E
 * ${
 */
#define ADDA_AUX_MIN_VLD_PIX_MASK   (BIT12 | BIT13 | BIT14 | BIT15)
#define ADDA_AUX_MIN_VLD_PIX_SHIFT  12
/** @} */


/** @name PMU configuration register (2 bytes)   - Address 0x14
 * ${
 */
#define PMU_CFG_PATCH_MEM_DVDD_MASK   (BIT6 | BIT5 | BIT4)
#define PMU_CFG_RESERVED            BIT7
#define PATCH_CODE_LOAD_ENABLE      BIT8
#define MCPU_INIT_STATE             BIT9
#define PATCH_MEM_ENABLE            BIT10
#define PATCH_CODE_RETAIN           BIT11

/** @} */


/** @name I2C Access Configuration Register (1 byte)   - Address 0x1C
 * ${
 */
#define I2C_AUTO_INC                BIT0
#define I2C_WORD_ACCESS             BIT1
#define I2C_AUTO_INC_DATAP          BIT2
#define I2C_WORD_ACCESS_DATAP       BIT3
#define I2C_MCPU_I2C_DEBUG          BIT4
#define I2C_HOST_DEBUG_ACC          BIT5
#define I2C_MCPU_DEBUG_ACC          BIT6
/** @} */

/** @name MCPU power management control register   - Address 0x1E
 * ${
 */
#define MCPU_SLEEP_REQ              BIT0
#define MCPU_STANDBY_REQ            BIT1
#define MCPU_PM_WATCHDOG_RESET_FLAG BIT2
/** @} */

/**
 * @name HW/FW Configuration Register 0 (2 bytes)   - Address 0x20
 * @{
 */
#define SAT_THRESHOLD_MASK          0x07FF
#define SAT_THRESHOLD_SHIFT         0
#define HFCFG0_MODE_OP_LONG         0x0
#define HFCFG0_MODE_OP_DEFAULT      0x1
#define HFCFG0_MODE_OP_HIGH_SPEED   0x2
#define HFCFG0_MODE_OP_HIGH_ACC     0x3
#define HFCFG0_MODE_OP_CUSTOM       0x4
#define HFCFG0_MODE_OP_MASK         0xF000
#define HFCFG0_MODE_OP_SHIFT        12
/** @} */

/**
 * @name HW/FW Configuration Register 1 (2 bytes)  - Address 0x22
 *
 * @{
 */
#define HFCFG1_MIN_CONF_THRESHOLD_MASK   0x0FFF
#define HFCFG1_MIN_CONF_THRESHOLD_SHIFT  0
#define MOD_FREQUENCY_MASK               0xF000
#define MOD_FREQUENCY_SHIFT              12
/** @} */

/**
 * @name HW/FW Configuration Register 2 (2 bytes)  - Address 0x24
 *
 * @{
 */
#define INT_TIME_MASK               0xF000
#define INT_TIME_SHIFT              12
/** @} */

/**
 * @name HW/FW Configuration Register 3 (2 bytes)  - Address 0x26
 * @{
 */
#define HARM_CANC                   BIT4
#define IT_MODE                     BIT7
#define IT_MODE_SHIFT               7
#define HFCFG3_COND_MEAS_EVENT_ENABLE       BIT11
#define HFCFG3_COND_MEAS_EVENT_ENABLE_SHIFT 11 
/** @} */

#ifdef __cplusplus
}
#endif

#endif // _HAL_HOSTREGS_H_

/******************** END of hal_hostregs.h *****************/