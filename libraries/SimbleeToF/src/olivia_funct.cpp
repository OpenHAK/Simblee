/*!*****************************************************************************
* @file   olivia_funct.c
*
*   Copyright (c) 2015-2017 Heptagon
*
* @brief  Olivia supporting functions
*******************************************************************************/

#include "i2c_tof.h"


#define gCALIBRATION_OLIVIA_F0_DATA_ADDRESS (0xC848)
#define gOLIVIA_EXPOSURE_TIME_ADDRESS (0x2006)

uint16_t pu16OliviaCalDataConfidence_[2];

static const uint16_t u16OliviaDefaultLowAmp_A = 1300;
static const uint16_t u16OliviaDefaultExposureTimeCutoff_B = 5000;
static const uint16_t u16OliviaDefaultHighAmp_C = 5600;
static const uint16_t u16OliviaMaxConfidenceAmp = 6500;
static const int  iAmpFactorNumerator = 9;
static const int  iAmpFactorDenominator = 10;

static const s3dRangeConfig gsOliviaDefaultCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x30FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0xA041U,
    .u16HFCfg_3 = 0x45D4U,
};

static const s3dRangeConfig gsOliviaCal100mmWhiteCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0x1C81U,
    .u16HFCfg_3 = 0x4154U,
};

static const s3dRangeConfig gsOliviaCal400mmBlackCfg = {
    .u16Cmd_CfgR_A = 0xE100U,
    .u16Cmd_CfgR_B = 0x10FFU,
    .u16HFCfg_0 = 0x07D0U,
    .u16HFCfg_1 = 0x5008U,
    .u16HFCfg_2 = 0xFC81U,
    .u16HFCfg_3 = 0x4154U,
};

void tof_olivia_default_config_get(s3dRangeConfig *ps3dRangeConfig)
{
    memcpy(ps3dRangeConfig, &gsOliviaDefaultCfg, sizeof(s3dRangeConfig));
}

void tof_olivia_patch_01000302(void)
{
    //Only patch if the Device Identify is 0x01000302
    if (u32DeviceIdent_ != 0x01000302){
        return;
    }
    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    tof_write_indirect_word(0x0710U, 0xD61AU);
    tof_write_indirect_word(0x0712U, 0xD586U);
    tof_write_indirect_word(0x0730U, 0xD564U);
    tof_write_indirect_word(0x0732U, 0xE500U);
    tof_write_indirect_word(0x0700U, 0x0003U);
}

int tof_olivia_af_confidence_checker_computer(void)
{
    int iLowAmp_A = 0, iHighAmp_C = 0, iRtn = 0;
    int iDist_mm;
    int iAmplitude;
    int iErrCode;
    int iExtendError;
    uint16_t u16ExposureTime_usec;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    iRtn = tof_olivia_single_measure_n_err_get_results(&iDist_mm, &iAmplitude, &u16ExposureTime_usec, &iErrCode, &iExtendError, eTOF_MEASURE_FLAG_EXTRA_ERROR_DETECTION);
    if (iRtn < 0) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get measurement(%d)\n", __FUNCTION__, __LINE__, iRtn);
        pu16OliviaCalDataConfidence_[0] = u16OliviaDefaultLowAmp_A;
        pu16OliviaCalDataConfidence_[1] = u16OliviaDefaultHighAmp_C;
        return 0;
    }
    iLowAmp_A = (iAmpFactorNumerator * iAmplitude) / iAmpFactorDenominator;
    iHighAmp_C = (int) (iLowAmp_A * u16OliviaMaxConfidenceAmp) / (u16OliviaMaxConfidenceAmp - u16OliviaDefaultExposureTimeCutoff_B);

    pu16OliviaCalDataConfidence_[0] = iLowAmp_A;
    pu16OliviaCalDataConfidence_[1] = iHighAmp_C;

    return 0;
}


/**********************************************************************************************
*@desc Does an extra check on the measurement to see if value has high confidence that is valid
*@     Note: only valid for Olivia
*@param iDist_mm (IN) - Distance in number of mm (1-2047mm)
*@param iAmp_Y (IN) - (confidence) amplitude as a measure of the quality of the measure(Y-Axis)
*@param u16ExposureTime_usec(IN) - Exposure time that was last use to make the measurement
*@param piErrCode (IN) - Error Code: 00=>no error, 01=>Near target 10=>Far field, 11=> General Err
*@param piExtendError (OUT) - Extend Error Code
*@return updated the piErrCode if the distance is not a high confidence that is valid
*        returns a 7 to signal that values don't have a high confidence that is valid.
***********************************************************************************************
*  Value doesn't have a high confidence, if it in Region AA or Region BB (Right Trapezoid)
*  Region AA ==> (X<B && Y>A)
*  Region BB ==> [(X>B) && (BY < (A-C)X + BC)]
* Pt_C +
*      |
* Pt_A +************|
*      |************|****
*Y-Axis|***Region***|*Region*
*iAmp_Y|*****AA*****|***BB*******
*       ------------+------------
*                 Pt_B    X-Axis -> Measurement Exp Time(u16ExposureTime_X)
***********************************************************************************************/
int tof_olivia_af_confidence_checker(int iDist_mm, int iAmplitude_Y, uint16_t u16ExposureTime_X_usec, int *piErrCode, int *piExtendError)
{
    int iLowAmp_A = (int)u16OliviaDefaultLowAmp_A;                         /*Pt_A*/
    int iExposureTimeCutoff_B = (int)u16OliviaDefaultExposureTimeCutoff_B; /*Pt_B*/
    int iHighAmp_C = (int)u16OliviaDefaultHighAmp_C;                       /*Pt_C*/
    int iAmptoDistFactor = 12;
    int iMinDistance_mm = 100;  /*From DataSheet*/
    int iMaxDistance_mm = 2000; /*From DataSheet*/

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    *piExtendError = eTOF_MEASURE_EXENDED_ERR_NO_ERROR/*0*/; /*TRUSTED VALUE*/

    if (*piErrCode != 0){
        *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
    }

    if (u16ExposureTime_X_usec < iExposureTimeCutoff_B) {
        /*Checking to see if value is in the Region AA */
        if (iAmplitude_Y < iLowAmp_A) {
            *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
            return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        }
    }
    else /*Checking to see if value is in Region BB */
        if ((iExposureTimeCutoff_B*iAmplitude_Y) < (iLowAmp_A - iHighAmp_C)*u16ExposureTime_X_usec + (iExposureTimeCutoff_B*iHighAmp_C)) {
            *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
            return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        }

    if ((iDist_mm < iMinDistance_mm) || (iDist_mm > iMaxDistance_mm)) {
        *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
    }

    if (iAmplitude_Y <= iAmptoDistFactor*iMaxDistance_mm / iDist_mm) {
        *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
        return eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;
    }
    return *piExtendError;
}

/******************************************************************************
*@desc Do the white 100mm calibration step - only outputs a single measurement
*      This is called twice to get two measurement
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_olivia_calibration_white_100mm(uint16_t * pu16MbxCalData)
{
    uint16_t u16DataSizeInWords = u16Generic_calibration_data_size_in_words_;
    return tof_pre130_calibration_shared(gsOliviaCal100mmWhiteCfg, pu16MbxCalData, u16DataSizeInWords);
}

/******************************************************************************
*@desc Do the black 400mm calibration step - only outputs a single measurement
*      This is called twice to get two measurement
*@param u16MbxCalData (IN/OUT) - Fills the field with the data from Mailbox 0x0006
*                                Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_olivia_calibration_black_400mm(uint16_t * pu16MbxCalData)
{
    uint16_t u16DataSizeInWords = u16Generic_calibration_data_size_in_words_;
    return tof_pre130_calibration_shared(gsOliviaCal400mmBlackCfg, pu16MbxCalData, u16DataSizeInWords);
}

/******************************************************************************
*@desc Do the F0 calibration step.
*@param u16F0Data (IN/OUT) - Fills the field with the data from F0
*                          Note: fail will cause this value to be all zero
*@return 0 for SUCCESS or <0 if error.   Note:Leave MCPU in off state
******************************************************************************/
int tof_olivia_calibration_f0_data_get(uint16_t * pu16F0Data)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (iI2cDevice_ == giINVALID_I2C_DEVICE){
        tof_log(eTOF_LOG_ERROR_LVL, "Need to connect to ToF before using API:%s\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    memset(pu16F0Data, 0x00, u16Generic_calibration_f0_data_size_in_words_ * sizeof(uint16_t));

    iRtn = tof_read_mem(gCALIBRATION_OLIVIA_F0_DATA_ADDRESS, u16Generic_calibration_f0_data_size_in_words_ * sizeof(uint16_t), (uint8_t *)pu16F0Data);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "Error - %s::unable to read F0 Data(%d)\n", __FUNCTION__, iRtn);
        tof_dump_device_status(NULL);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }
    return gTOF_RTN_SUCCESS /*0*/;
}

/******************************************************************************
*@desc Get the exposure time from the last measurement for Olivia 
*@param pu16ExposureTime_usec (IN/OUT) - The exposure Time in uSeconds
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_olivia_exposure_time(uint16_t *pu16ExposureTime_usec)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    /*For OLIVIA - Get the Exposure time in uSeconds*/
    iRtn = tof_read_indirect_word(gOLIVIA_EXPOSURE_TIME_ADDRESS, pu16ExposureTime_usec);

    return iRtn;
}

/******************************************************************************
*@desc Puts Olivia into a higher sensitivity mode
*@return 0 for SUCCESS or <0 if error
******************************************************************************/
int tof_olivia_enable_higher_sensitivity_mode(void)
{
    int iRtn;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    if (CurrTofType_ != eTOF_TYPE_OLIVIA) {
        tof_log(eTOF_LOG_ERROR_LVL, "Function:%s only works for Olivia\n", __FUNCTION__);
        return gTOF_RTN_GENERIC_FAIL /*-1*/;
    }

    /*Issue Mailbox Command 0x81C7*/
    iRtn = i2c_write_word(addr_ADDA_H2M_MBX, 0x81C7);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to write H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Wait until Mailbox is ready*/
    iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_M2H_ACK/*Bit 6*/,150 /*15mSec Max wait*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - timeout on ICSR wait for H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Issue Mailbox Command 0x640*/
    iRtn = i2c_write_word(addr_ADDA_H2M_MBX, 0x640);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/){
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to write H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }

    /*Wait until Mailbox is ready*/
    iRtn = tof_wait_on_interrupt_blocking(ICSR_INT_M2H_ACK/*Bit 6*/, 150 /*15mSec Max wait*/);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - timeout on ICSR wait for H2M MBX(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FAIL;
    }
    return gTOF_RTN_SUCCESS /*0*/;

FAIL:
    tof_dump_device_status(NULL);
    return gTOF_RTN_GENERIC_FAIL /*-1*/;
}

/******************************************************************************
*@desc get a ToF single measurement results once it is ready
*@param piDist_mm (OUT) - Distance in number of mm (1-4095mm): if error value is 00
*@param piAmplitude (OUT) - vector amplitude as a measure of the quality of the measure
*@param pu16ExposureTime_usec (OUT) - The Exposure Time in uSeconds that was use to get the measurement
*@param piErrCode (OUT) - Error Code: 00=>no error, 01=>Near target, 10=>Far field, 11=> General Err
*@param piExtendError(OUT) - Extend Error code (Enable by options in u32MeasFlags)
*@param u32MeasFlags (IN) - Configure what measure flags should be use when getting the measurement
*@return Distance in number of mm or 0xFFFF if error. 
*@NOTE: Leave in standby state, unless eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY then left in MCPU ON state
******************************************************************************/
int tof_olivia_single_measure_n_err_get_results(int *piDist_mm, int *piAmplitude, uint16_t *pu16ExposureTime_usec, int *piErrCode, int *piExtendError, uint32_t u32MeasFlags)
{
    int iRtn;
    int iFunctionRtn = gTOF_RTN_GENERIC_FAIL /*-1*/;

    tof_log(eTOF_LOG_CMD_DEBUG_LVL, "%s::%d starting\n", __FUNCTION__, __LINE__);

    /*Default values if there is an error*/
    *piDist_mm = 0xFFFF;
    *piAmplitude = 0;
    *pu16ExposureTime_usec = 0U; 
    *piErrCode = RSLTR_ERR_CODE_ERROR/*3*/;
    *piExtendError = eTOF_MEASURE_EXENDED_ERR_GENERAL_ERROR;

    iRtn = i2c_read_word(addr_ADDA_RSLTR);
    if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
        tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - Unable to get Mesaurement results(%d)\n", __FUNCTION__, __LINE__, iRtn);
        goto FINISH;
    }

    *piDist_mm = (iRtn & DISTANCE_MM_MASK_OLD) >> DISTANCE_MM_SHIFT_OLD;
    *piErrCode = (iRtn & RSLTR_ERR_CODE_MASK) >> RSLTR_ERR_CODE_SHIFT;
    if (iRtn & RSLTR_VALID) {
        /*Get the confidence value of the last measurement value*/
        iRtn = i2c_read_word(addr_ADDA_RSL_CON);
        if (iRtn < gTOF_RTN_SUCCESS /*0*/) {
            tof_log(eTOF_LOG_ERROR_LVL, "%s::%d - unable to get Amplitude value(%d)\n", __FUNCTION__, __LINE__, iRtn);
            goto FINISH;
        }
        *piAmplitude = (iRtn & CNFD_VEC_AMPL_MASK) >> CNFD_VEC_AMPL_SHIFT;

        tof_generic_exposure_time_(pu16ExposureTime_usec);
        iFunctionRtn = *piDist_mm;
    }
    else {
        *piDist_mm = 0xFFFF;
        iFunctionRtn = 0;
    }

    *piExtendError = eTOF_MEASURE_EXENDED_ERR_NO_ERROR;
    if (u32MeasFlags & eTOF_MEASURE_FLAG_EXTRA_ERROR_DETECTION) {
        tof_olivia_af_confidence_checker(*piDist_mm, *piAmplitude, *pu16ExposureTime_usec, piErrCode, piExtendError);
    }


FINISH:
    if ((u32MeasFlags & eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY) != eTOF_MEASURE_FLAG_DONT_LEAVE_IN_STANDBY) {
        /*Go into Stand-by mode - low power mode*/
        i2c_write_word(addr_ADDA_CMD, CMD_VALID_MASK | CMD_GO_STANDBY /*0x0090*/);
    }
    return iFunctionRtn;
}

/******************** END of olivia_funct.c *****************/