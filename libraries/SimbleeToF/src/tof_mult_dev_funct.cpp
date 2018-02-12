/*!*****************************************************************************
* @file   tof_mult_dev_funct.c
*
*   Copyright (c) 2015-2017 Heptagon
*
* @brief  Main high-level ToF functions
*******************************************************************************/

#include "i2c_tof.h"

static const int MAILBOX_0X0006_DATA_SIZE_IN_WORDS = 27;/*# unsigned 16bit words*/
static const int MAILBOX_0X0009_DATA_SIZE_IN_WORDS = 33;/*# unsigned 16bit words*/
static const int gCALIBARTION_F0_DATA_SIZE_IN_WORDS = 76;    /*# unsigned 16bit words*/
static const int gCALIBARTION_PRE_FW130_F0_DATA_SIZE_IN_WORDS = 59;  /*# unsigned 16bit words*/

void tof_none_default_config_get(s3dRangeConfig *ps3dRangeConfig)
{    
    tof_log(eTOF_LOG_ERROR_LVL, "tof_generic_default_config_get_ is not support for this device\n");
}
int tof_none_calibration_white_100mm(uint16_t * pu16MbxCalData)
{
    tof_log(eTOF_LOG_ERROR_LVL, "tof_generic_calibration_white_100mm_ is not support for this device\n");
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}
int tof_none_calibration_black_400mm(uint16_t * pu16MbxCalData)
{
    tof_log(eTOF_LOG_ERROR_LVL, "tof_generic_calibration_black_400mm_ is not support for this device\n");
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}
int tof_none_calibration_f0_data_get(uint16_t * pu16F0Data)
{
    tof_log(eTOF_LOG_ERROR_LVL, "tof_generic_calibration_f0_data_get_ is not support for this device\n");
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}
int tof_none_exposure_time(uint16_t *pu16ExposureTime_usec)
{
    tof_log(eTOF_LOG_ERROR_LVL, "tof_generic_exposure_time_ is not support for this device\n");
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}
int tof_none_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags)
{
    tof_log(eTOF_LOG_ERROR_LVL, "tof_generic_single_measure_n_err_get_results_ is not support for this device\n");
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/*************************************************************************************
*@desc setup and configure an empty device (default)
*************************************************************************************/
void tof_device_setup_empty_device(void)
{
    u16Generic_calibration_data_size_in_words_ = 0;
    u16Generic_calibration_f0_data_size_in_words_ = 0;
    tof_generic_default_config_get_ = tof_none_default_config_get;
    tof_generic_calibration_white_100mm_ = tof_none_calibration_white_100mm;
    tof_generic_calibration_black_400mm_ = tof_none_calibration_black_400mm;
    tof_generic_calibration_f0_data_get_ = tof_none_calibration_f0_data_get;
    tof_generic_exposure_time_ = tof_none_exposure_time;
    tof_generic_single_measure_n_err_get_results_ = tof_none_single_measure_n_err_get_results;
}


/*************************************************************************************
*@desc setup and configure Olivia
*************************************************************************************/
static void tof_device_setup_olivia(void)
{
    u16Generic_calibration_data_size_in_words_ = MAILBOX_0X0006_DATA_SIZE_IN_WORDS;
    u16Generic_calibration_f0_data_size_in_words_ = gCALIBARTION_PRE_FW130_F0_DATA_SIZE_IN_WORDS;
    tof_generic_default_config_get_ = tof_olivia_default_config_get;
    tof_generic_calibration_white_100mm_ = tof_olivia_calibration_white_100mm;
    tof_generic_calibration_black_400mm_ = tof_olivia_calibration_black_400mm;
    tof_generic_calibration_f0_data_get_ = tof_olivia_calibration_f0_data_get;
    tof_generic_exposure_time_ = tof_olivia_exposure_time;
    tof_generic_single_measure_n_err_get_results_ = tof_olivia_single_measure_n_err_get_results;
}

/*************************************************************************************
*@desc setup and configure Olivia 2.0
*************************************************************************************/
static void tof_device_setup_olivia20(void)
{
    u16Generic_calibration_data_size_in_words_ = MAILBOX_0X0009_DATA_SIZE_IN_WORDS;
    u16Generic_calibration_f0_data_size_in_words_ = gCALIBARTION_F0_DATA_SIZE_IN_WORDS;
    tof_generic_default_config_get_ = tof_olivia20_default_config_get;
    tof_generic_calibration_white_100mm_ = tof_olivia20_calibration_white_100mm;
    tof_generic_calibration_black_400mm_ = tof_olivia20_calibration_black_400mm;
    tof_generic_calibration_f0_data_get_ = tof_olivia20_calibration_f0_data_get;
    tof_generic_exposure_time_ = tof_olivia20_exposure_time;
    tof_generic_single_measure_n_err_get_results_ = tof_olivia20_single_measure_n_err_get_results;
}

/*************************************************************************************
*@desc setup and configure Elisa
*************************************************************************************/
static void tof_device_setup_elisa(void)
{
    u16Generic_calibration_data_size_in_words_ = MAILBOX_0X0009_DATA_SIZE_IN_WORDS;
    u16Generic_calibration_f0_data_size_in_words_ = gCALIBARTION_F0_DATA_SIZE_IN_WORDS;
    tof_generic_default_config_get_ = tof_elisa_default_config_get;
    tof_generic_calibration_white_100mm_ = tof_elisa_calibration_white_100mm;
    tof_generic_calibration_black_400mm_ = tof_elisa_calibration_black_400mm;
    tof_generic_calibration_f0_data_get_ = tof_elisa_calibration_f0_data_get;
    tof_generic_exposure_time_ = tof_elisa_exposure_time;
    tof_generic_single_measure_n_err_get_results_ = tof_elisa_single_measure_n_err_get_results;
}


/*************************************************************************************
*@desc setup and configure Trinity
*************************************************************************************/
static void tof_device_setup_trinity(void)
{
    u16Generic_calibration_data_size_in_words_ = MAILBOX_0X0006_DATA_SIZE_IN_WORDS;
    u16Generic_calibration_f0_data_size_in_words_ = gCALIBARTION_PRE_FW130_F0_DATA_SIZE_IN_WORDS;
    tof_generic_default_config_get_ = tof_trinity_default_config_get;
    tof_generic_calibration_white_100mm_ = tof_trinity_calibration_white_100mm;
    tof_generic_calibration_black_400mm_ = tof_trinity_calibration_black_400mm;
    tof_generic_calibration_f0_data_get_ = tof_trinity_calibration_f0_data_get;
    tof_generic_exposure_time_ = tof_trinity_exposure_time;
    tof_generic_single_measure_n_err_get_results_ = tof_trinity_single_measure_n_err_get_results;
}


/*************************************************************************************
*@desc Select which ToF device and configure code to use it
*@param eDetectDevice (IN) - If eToF_TYPE_UNDETECTED then detect it, else use this
*                            instead of auto-detecting it
*@return 0 for SUCCESS or <0 if error
*************************************************************************************/
int tof_module_select_and_config(eTofType eDetectDevice)
{
    int iRtn;
    char szSerialId[gMAX_STD_STR_SIZE];
    static int iFirst = TRUE;
    eTofLogLvl eLogLevel = eTOF_LOG_CMD_DEBUG_LVL;

    if (iFirst == TRUE){
        eLogLevel = eTOF_LOG_ERROR_LVL;
        iFirst = FALSE;
    }


    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    iRtn = tof_module_fw_id_read();
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "Error - %s::unable to get the devuce Firmware ID(%d)\n", __FUNCTION__, iRtn);
        goto FAIL;
    }

    iRtn = tof_module_serial_id_read(szSerialId);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "Error - %s::unable to read memory(%d)\n", __FUNCTION__, iRtn);
        goto FAIL;
    }

    if (strncmp((char *)szSerialId, "MLAX", 4) == 0){
        CurrTofType_ = eTOF_TYPE_OLIVIA;
        tof_log(eLogLevel, "OLIVA: (0x%08X - %s) was detected\n", u32DeviceIdent_, szSerialId);
    }
    else if (strncmp((char *)szSerialId, "MLBX", 4) == 0){
        CurrTofType_ = eTOF_TYPE_OLIVIA_20;
        tof_log(eLogLevel, "OLIVA 2.0: (0x%08X - %s) was detected\n", u32DeviceIdent_, szSerialId);
    }
    else if (strncmp((char *)szSerialId, "MUAX", 4) == 0){
        CurrTofType_ = eTOF_TYPE_ELISA;
        tof_log(eLogLevel, "ELISA: (0x%08X - %s) was detected\n", u32DeviceIdent_, szSerialId);
    }
    else if (strncmp((char *)szSerialId, "MPAX", 4) == 0){
        CurrTofType_ = eTOF_TYPE_TRINITY;
        tof_log(eLogLevel, "TRINITY: (0x%08X - %s) was detected\n", u32DeviceIdent_, szSerialId);
    }
    else { 
        CurrTofType_ = eToF_TYPE_UNDETECTED;
        tof_log(eLogLevel, "UNKNOWN: (0x%08X - %s) was detected\n", u32DeviceIdent_, szSerialId);
    }

    /*Over Ride detection*/
    if (eDetectDevice != eToF_TYPE_UNDETECTED){
        tof_log(eLogLevel, "NOTE: OVERRIDE device detection to %d\n", (int)eDetectDevice);
        CurrTofType_ = eDetectDevice;
    }

    switch (CurrTofType_)
    {
    case eTOF_TYPE_OLIVIA:
        tof_device_setup_olivia();
        break;
    case eTOF_TYPE_OLIVIA_20:
        tof_device_setup_olivia20();
        break;
    case eTOF_TYPE_ELISA:
        tof_device_setup_elisa();
        break;
    case eTOF_TYPE_TRINITY:
        tof_device_setup_trinity();
        break;
    default:
        tof_log(eTOF_LOG_ERROR_LVL, "ERROR: unknown type: %d\n", (int)CurrTofType_);
        tof_device_setup_empty_device();
        goto FAIL;
    }
    return iRtn;

FAIL:
    tof_dump_device_status(NULL);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************** END of tof_mult_dev_funct.c *****************/