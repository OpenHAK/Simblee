/*!*****************************************************************************
* @file   euler_reg_addr.h
*
* Copyright (c) 2015-2017 Heptagon
*
* NOTE: Removed Internal commands
*******************************************************************************/


#ifndef _EULER_REG_ADDR_H_
#define _EULER_REG_ADDR_H_

#ifdef __cplusplus
extern "C" {
#endif

#define addr_ADDA_ICSR       0x00	
#define addr_ADDA_IER        0x02	
#define addr_ADDA_CMD        0x04	
#define addr_ADDA_DSTATUS    0x06	
#define addr_ADDA_DSTATUS_UP 0x07	
#define addr_ADDA_RSLTR      0x08
#define addr_ADDA_RSLTR_UP   0x09
#define addr_ADDA_RSL_CON    0x0A	
#define addr_ADDA_RSL_CON_UP 0x0B
#define addr_ADDA_RFSH       0x0C	
#define addr_ADDA_AUXCTRL    0x0E	
#define addr_ADDA_AUXCTRL_UP 0x0F	
#define addr_ADDA_H2M_MBX    0x10	
#define addr_ADDA_H2M_MBX_UP 0x11	
#define addr_ADDA_M2H_MBX    0x12
#define addr_ADDA_M2H_MBX_UP 0x13
#define addr_ADDA_PMU_CFG    0x14		
#define addr_ADDA_PMU_CFG_UP 0x15
#define addr_ADDA_CRC_ST     0x16
#define addr_ADDA_CRC_ST_UP  0x17
#define addr_I2C_ADDR_PTR    0x18	
#define addr_I2C_ADDR_PTR_UP 0x19	
#define addr_I2C_DATA_PORT   0x1A	
#define addr_I2C_ACC_CFG     0x1C	
#define addr_ADDA_MCPU_PMC   0x1E	
#define addr_ADDA_HFCFG0     0x20	
#define addr_ADDA_HFCFG0_UP  0x21	
#define addr_ADDA_HFCFG1     0x22	
#define addr_ADDA_HFCFG1_UP  0x23	
#define addr_ADDA_HFCFG2     0x24	
#define addr_ADDA_HFCFG2_UP  0x25	
#define addr_ADDA_HFCFG3     0x26	
#define addr_ADDA_HFCFG3_UP  0x27	
#define addr_ADDA_CHIP_ID    0x28
#define addr_ADDA_CHIP_ID_UP 0x29
#define addr_ADDA_PATCH_MCFG 0x29

#ifdef __cplusplus
}
#endif

#endif //_EULER_REG_ADDR_H_


/******************** END of euler_reg_addr.h *****************/