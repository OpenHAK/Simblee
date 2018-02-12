/*!*****************************************************************************
* @file   i2c_tof.h
*
* Copyright (c) 2015-2017 Heptagon
*
* @brief  I2C ToF class
*******************************************************************************/

#ifndef _I2C_TOF_H_
#define _I2C_TOF_H_
/*
#ifdef KERNEL_DRIVER
    #include <linux/unistd.h>
    #include <linux/ioctl.h>
    #include <linux/fcntl.h>
    #include <linux/string.h> 
    #include <linux/i2c-dev.h>
    #include <linux/i2c.h>
    #include <linux/module.h>
    #include <linux/kernel.h>
    #include <linux/ioctl.h>
    #include <linux/delay.h>
    #include <linux/init.h>
    #include <linux/device.h>
    #include <linux/fs.h>
    #include <linux/hid.h>
    #include <linux/slab.h>
    #include <asm/uaccess.h>    
    #include <linux/kernel.h>
    #include <linux/module.h>
  #include <linux/kobject.h>
  #define usleep(x) udelay(x)
  #define printf(...) printk( __VA_ARGS__)
#else
    #ifdef WIN32
      #include "stdafx.h"
    #endif //WIN32
    #include <errno.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <string.h> 
    #include <stdint.h>
	#ifndef WIN32
	  #include <unistd.h>
	  #include <sys/ioctl.h>
	  #if ANDROID_USER
		#include <android/log.h>
		#include <linux/i2c.h> //uncomment for Android < 5
		#define printf(...) __android_log_print(ANDROID_LOG_INFO, "hlog", __VA_ARGS__);
	  #else
		#include <linux/i2c-dev.h>
	  #endif //ANDROID_USER
	#endif //WIN32
#endif //KERNEL_DRIVER
*/
#include "euler_reg_addr.h" 
#include "bits.h"
#include "hal_hostregs.h"

//added this
#include <Wire.h>
#define __u8	uint8_t
#define __u16	uint16_t
#define usleep(x)	delayMicroseconds(x)
#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
#define O_ACCMODE	0x0003

#ifdef __cplusplus
extern "C" {
#endif

#define gTOF_RTN_SUCCESS (0)
#define gTOF_RTN_GENERIC_FAIL (-1)
#define gTOF_RTN_NOT_INITIALIZED (-2)
#define gTOF_RTN_POWERED_OFF (-3) /*Powered off or unable to communicate with the device*/
#define gMAX_STD_STR_SIZE (128)
#define gTOF_3D_RANGER_ADDRESS (0x4C)
#define giINVALID_I2C_DEVICE (-1)
#define gMAILBOX_MAX_CAL_DATA_SIZE_IN_WORDS  (33) /*# unsigned 16bit words*/
#define gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS (64) /*# unsigned 16bit words*/
#define gCALIBRATION_FACTOR_ADDRESS (0xC010)
#define gDEVICE_SERIAL_ID_ADDRESS (0xC804)
#define gMAX_DEVICE_SERIAL_NUM_SIZE_IN_BYTES (30) /*# of 8bit chars*/
#define gDEVICE_FW_ID_ADDRESS (0xFFC0)
#define gTRINITY_NUM_OBJ_PIXELS (9)
#define gMAX_LOG_PRINT_SIZE  (1000)
#define gLOG_DISPLAY_TAG "TOF_LASER"

typedef enum {
    eToF_TYPE_UNDETECTED,
    eTOF_TYPE_LAURA, 
    eTOF_TYPE_OLIVIA,
    eTOF_TYPE_OLIVIA_20,
    eTOF_TYPE_ELISA,
    eTOF_TYPE_TRINITY, 
} eTofType;

typedef enum {
    eTOF_LOG_DISABLE_LVL,     /*NO Log including error messages - If log level means always print*/
    eTOF_LOG_ERROR_LVL,       /*Only error messages*/
    eTOF_LOG_CMD_DEBUG_LVL,   /*Error and Command messages*/
    eTOF_LOG_ALL_DEBUG_LVL,   /*Error, Command and debug messages*/
    gTOF_LOG_ALL_INFO_LVL,    /*Error, Command, debug, information messages*/
} eTofLogLvl;

typedef enum {
    eTOF_MCPU_STATE_STANDBY = DSTATUS_PM_STATUS_STANDBY, //0x00
    eTOF_MCPU_STATE_OFF = DSTATUS_PM_STATUS_MCPU_OFF,    //0x10
    eTOF_MCPU_STATE_ON = DSTATUS_PM_STATUS_MCPU_ON,      //0x18
} eTofMcpuState;

typedef enum {
    eTOF_MEASURE_FLAG_NORMAL                       = 0x00000000U,
    eTOF_MEASURE_FLAG_EXTRA_ERROR_DETECTION        = 0x00000001U,
    eTOF_MEASURE_FLAG_SMUDGE_DETECTION             = 0x00000003U,
    eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY        = 0x00000004U,
    eTOF_MEASURE_FLAG_OPERATION_MODE_LONG          = 0x00000010U,
    eTOF_MEASURE_FLAG_OPERATION_MODE_HIGH_SPEED    = 0x00000020U,
    eTOF_MEASURE_FLAG_OPERATION_MODE_HIGH_ACCURACY = 0x00000030U,
    eTOF_MEASURE_FLAG_OPERATION_MODE_CUSTOM        = 0x00000040U,
} eTofMeasFlags;

typedef enum {
    eTOF_MEASURE_EXENDED_ERR_NO_ERROR,
    eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR,
    eTOF_MEASURE_EXENDED_ERR_SATURATION_ERROR,
    eTOF_MEASURE_EXENDED_ERR_SMUDGE_ERROR,
} eTofMeasExtendError;

typedef struct s3dRangeConfig {
    uint16_t  u16Cmd_CfgR_A; //Address - 0x0C
    uint16_t  u16Cmd_CfgR_B; //Address - 0x0E 
    uint16_t  u16HFCfg_0;    //Address - 0x20 
    uint16_t  u16HFCfg_1;    //Address - 0x22
    uint16_t  u16HFCfg_2;    //Address - 0x24
    uint16_t  u16HFCfg_3;    //Address - 0x26
} s3dRangeConfig;

extern uint32_t u32DeviceIdent_;
extern uint16_t * pu16CalibrationFactor_;
extern uint16_t u16CalibrationFactorData_[gMAX_CALIBRATION_FACTOR_DATA_SIZE_IN_WORDS];
extern uint16_t pu16OliviaCalDataConfidence_[2];
extern s3dRangeConfig sCurrToFConfig_;
extern char szI2CDevice_[gMAX_STD_STR_SIZE];

extern eTofType CurrTofType_;
extern int iI2cDevice_;
extern char szSerialId_[gMAX_STD_STR_SIZE];
extern int iLogDisplayLevel_;
extern uint16_t u16Generic_calibration_data_size_in_words_;
extern uint16_t u16Generic_calibration_f0_data_size_in_words_;

/*Common APIs - changes base on the device*/
extern void(*tof_generic_default_config_get_)(s3dRangeConfig *ps3dRangeConfig);
extern int(*tof_generic_calibration_white_100mm_)(uint16_t * pu16MbxCalData);
extern int(*tof_generic_calibration_black_400mm_)(uint16_t * pu16MbxCalData);
extern int(*tof_generic_calibration_f0_data_get_)(uint16_t * pu16F0Data);
extern int(*tof_generic_exposure_time_)(uint16_t *pu16ExposureTime_usec);
extern int(*tof_generic_single_measure_n_err_get_results_)(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags);


/*In i2c_tof.c*/
int i2c_open(const char * szDeviceName, int iFlags, int iI2cAddr);
int i2c_close(int iI2cDevice);
int i2c_read_byte(int iDeviceAddr);
int i2c_read_word(int iDeviceAddr);
int i2c_write_byte(int iDeviceAddr, int value);
int i2c_write_word(int iDeviceAddr, int value);

/*In tof_base_funct.c*/
int tof_read_indirect_word(uint16_t u16Addr, uint16_t *pu16Data);
int tof_write_indirect_word(uint16_t u16Addr, uint16_t u16Data);
int tof_write_n_verify_word(int iDeviceAddr, uint16_t u16Data);
int tof_read_mem(uint16_t u16Addr, uint16_t u16SizeInBytes, uint8_t* pu8Buf);
int tof_write_mem(uint16_t u16Addr, uint16_t u16SizeInBytes, uint8_t* pu8Buf);
int tof_display_log(const char *szTag, const char *szFmt, va_list args);
int tof_log(int iLogType, const char *szFmt, ...);
void tof_print_hex(uint8_t *pu8Buff, int iSizeInBytes, eTofLogLvl eLogLvl);
eTofType tof_device_type(void);
void tof_log_set_level(int iLogLevel);

/*In tof_main_funct.c*/
int tof_mcpu_state_set(eTofMcpuState eState);
int tof_mcpu_state_set_forcibly(eTofMcpuState eState);
int tof_mcpu_state_set_w_retry(eTofMcpuState eState, int iAllowForce);
int tof_mcpu_on_and_wait_for_idle(int iMaxTries);
int tof_device_configuration(s3dRangeConfig sConfig);
void tof_initialize_internal_values(void);
int tof_connect(const char * szDevice, s3dRangeConfig *sConfig);
int tof_reinitialize_device(void);
void tof_disconnect(void);
int tof_dev_status_last_get(int iDisp, int *piMcpuStatus);
void tof_calibration_factors_set_factory(void);
int tof_calibration_factors_set(uint16_t * pu16CalibrationFactorData);
uint16_t * tof_calibration_factors_get(void);
int  tof_measurement_issue_continuous_measure_request(uint16_t u16RefreshRate_us);
int tof_poweron_w_cfg(s3dRangeConfig *sConfig, eTofType eDetectDeviceType);
int tof_measurement_issue_measure_request(uint32_t u32MeasFlags);
int tof_measurement_get_w_wait(int *piDist_mm, int *piAmplitude, uint16_t *u16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags, uint32_t u32MaxWaitTime_ms);
int tof_check_interrupt(uint16_t u16DoneBitMask);
int tof_wait_on_interrupt_blocking(uint16_t u16DoneBitMask, uint16_t u16MaxTries);
int tof_mailbox_read_cmd(uint16_t u16MailBoxCmd, uint16_t *pu16OutputData, uint16_t u16OutputSizeInWords);
int tof_mailbox_write_cmd(uint16_t u16MailBoxCmd, uint16_t *pu16InputData, uint16_t u16DataSizeInWords);
int tof_module_fw_id_read(void);
int tof_module_serial_id_read(char * szSerialId);
int tof_calibration_shared(s3dRangeConfig sCalibrationCfg, uint16_t *pu16MbxCalData, uint16_t u16DataSizeInWords);
int tof_device_initialization_status(int reinitializeIfUninitialized);
void tof_dump_device_status(char *szOutput);
void tof_dump_cfg_setting_get(char *szOutput);

/*In tof_pre_fw_130.c*/
int tof_pre130_mailbox_cmd_0x0006(uint16_t *pu16MbxCalData);
int tof_pre130_calibration_shared(s3dRangeConfig sCalibrationCfg, uint16_t *pu16MbxCalData, uint16_t u16DataSizeInWords);

/*In tof_mult_dev_funct.c*/
void tof_device_setup_empty_device(void);
int tof_module_select_and_config(eTofType eDetectDevice);

/*In Olivia_funct.c*/
void tof_olivia_default_config_get(s3dRangeConfig *ps3dRangeConfig);
int tof_olivia_af_confidence_checker_computer(void);
int tof_olivia_af_confidence_checker(int iDist_mm, int iAmplitude_Y, uint16_t u16ExposureTime_X_usec, int *piErrCode, int *piExtendError);
int tof_olivia_calibration_white_100mm(uint16_t *pu16MbxCalData);
int tof_olivia_calibration_black_400mm(uint16_t *pu16MbxCalData);
int tof_olivia_calibration_f0_data_get(uint16_t *pu16F0Data);
int tof_olivia_exposure_time(uint16_t *pu16ExposureTime_usec);
int tof_olivia_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags);

/*In Olivia20_funct.c*/
void tof_olivia20_default_config_get(s3dRangeConfig *ps3dRangeConfig);
int tof_olivia20_af_confidence_checker_computer(void);
int tof_olivia20_af_confidence_checker(int *piErrCode, int iDist_mm, int iAmplitude, uint16_t u16ExposureTime_usec);
int tof_olivia20_calibration_white_100mm(uint16_t *pu16MbxCalData);
int tof_olivia20_calibration_black_400mm(uint16_t *pu16MbxCalData);
int tof_olivia20_calibration_f0_data_get(uint16_t *pu16F0Data);
int tof_olivia20_exposure_time(uint16_t *pu16ExposureTime_usec);
int tof_olivia20_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags);

/*In Elisa_funct.c*/
void tof_elisa_default_config_get(s3dRangeConfig *ps3dRangeConfig);
int tof_elisa_calibration_white_100mm(uint16_t * pu16MbxCalData);
int tof_elisa_calibration_black_400mm(uint16_t * pu16MbxCalData);
int tof_elisa_calibration_f0_data_get(uint16_t * pu16F0Data);
int tof_elisa_exposure_time(uint16_t *pu16ExposureTime_usec);
int tof_elisa_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags);

/*In trinity_funct.c*/
void tof_trinity_default_config_get(s3dRangeConfig *ps3dRangeConfig);
int tof_trinity_calibration_white_100mm(uint16_t * pu16MbxCalData);
int tof_trinity_calibration_black_400mm(uint16_t * pu16MbxCalData);
int tof_trinity_calibration_f0_data_get(uint16_t * pu16F0Data);
int tof_trinity_exposure_time(uint16_t *pu16ExposureTime_usec);
int tof_generic_per_pixel_mesurement_get_w_wait( uint16_t * pu16PerPixelMeas_mm, uint16_t * pu16PerPixelAmplitude, uint8_t * pu8PerPixelErr, uint8_t *pu8PerPixelExtErr, uint8_t *pu8GrpErr, uint16_t *pu16ExposureTime_usec, uint32_t u32MeasFlags);
int tof_trinity_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags);

#ifdef __cplusplus
}
#endif


#endif //_I2C_TOF_H_
/******************** END of i2c_tof.h *****************/
