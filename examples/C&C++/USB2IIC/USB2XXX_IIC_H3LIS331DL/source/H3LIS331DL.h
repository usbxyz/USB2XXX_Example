/*
 * H3LIS331DL.h
 * A library for 3-Axis Digital Accelerometer(±400g)
 *  
 * Copyright (c) 2014 seeed technology inc.
 * Website    : www.seeed.cc
 * Author     : lawliet zou
 * Create Time: April 2014
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include "stdint.h"
#include "usb2iic.h"

#ifndef H3LIS331DL_H
#define H3LIS331DL_H

#define MEMS_SET                        0x01
#define MEMS_RESET                      0x00

#define H3LIS331DL_MEMS_I2C_ADDRESS     0x19//0x32

//Register and define
#define H3LIS331DL_WHO_AM_I             0x0F   // device identification register

// CONTROL REGISTER 1 
#define H3LIS331DL_CTRL_REG1            0x20
#define H3LIS331DL_PM                   BIT(5) //PowerMode selection: 000 - power down / 001 - normal mode / other - low power
#define H3LIS331DL_DR                   BIT(3) //output data rate: 00 - 50hz / 01 - 100hz / 10 - 400hz / 11 - 1000hz
#define H3LIS331DL_ZEN                  BIT(2) //Z-axis enable: 0 - disable / 1 - enable
#define H3LIS331DL_YEN                  BIT(1) //Y-axis enable: 0 - disable / 1 - enable
#define H3LIS331DL_XEN                  BIT(0) //Y-axis enable: 0 - disable / 1 - enable

//CONTROL REGISTER 2 
#define H3LIS331DL_CTRL_REG2            0x21
#define H3LIS331DL_BOOT                 BIT(7) //reboot memory content, default is 0
#define H3LIS331DL_HPM                  BIT(5) //High-pass-filter mode selection, default is 00
#define H3LIS331DL_FDS                  BIT(4) //Filter data selection, default is 0
#define H3LIS331DL_HPEN2                BIT(3) //High-pass filter enabled for interrupt 2 source, default is 0
#define H3LIS331DL_HPEN1                BIT(2) //High-pass filter enabled for interrupt 1 source, default is 0
#define H3LIS331DL_HPCF                 BIT(0) //High-pass filter cutoff frequency configuration, default is 00

//CONTROL REGISTER 3 
#define H3LIS331DL_CTRL_REG3            0x22
#define H3LIS331DL_IHL                  BIT(7) //Interrupt active high,low. default is 0
#define H3LIS331DL_PP_OD                BIT(6) //Push-pull/open drain selection on interrupt pad. default is 0
#define H3LIS331DL_LIR2                 BIT(5) //Latch interrupt request on INT2_SRC register, with INT2_SRC register cleared by read INT2_SRC itself. default is 0
#define H3LIS331DL_I2_CFG               BIT(3) //Data signal on INT2 pad control bits, default is 00
#define H3LIS331DL_LIR1                 BIT(2) //Latch interrupt request on the INT1_SRC register, with the INT1_SRC register cleared by reading the INT1_SRC register.
#define H3LIS331DL_I1_CFG               BIT(0) //Data signal on INT1 pad control bits, default is 00

//CONTROL REGISTER 4
#define H3LIS331DL_CTRL_REG4            0x23
#define H3LIS331DL_BDU                  BIT(7) //Block data update, default is 0
#define H3LIS331DL_BLE                  BIT(6) //Big/little endian data selection, default is 0
#define H3LIS331DL_FS                   BIT(4) //Full scale selection, default is 00(00:100g;01:200g;11:400g)
#define H3LIS331DL_ST_SIGN              BIT(3) //
#define H3LIS331DL_ST                   BIT(1) //
#define H3LIS331DL_SIM                  BIT(0) // SPI serial interface mode selection, default is 0

//CONTROL REGISTER 5
#define H3LIS331DL_CTRL_REG5            0x24 
#define H3LIS331DL_TURN_ON              BIT(0) // Turn-on mode selection selection for sleep to wake function. default is 00

#define H3LIS331DL_HP_FILTER_RESET      0x25   // 

//REFERENCE/DATA_CAPTURE
#define H3LIS331DL_REFERENCE_REG        0x26   //
#define H3LIS331DL_REF                  BIT(0) //

//STATUS_REG_AXIES 
#define H3LIS331DL_STATUS_REG           0x27   //

//OUTPUT REGISTER
#define H3LIS331DL_OUT_X_L              0x28   //x-axis acceleration data
#define H3LIS331DL_OUT_X_H              0x29   
#define H3LIS331DL_OUT_Y_L              0x2A   //y-axis acceleration data
#define H3LIS331DL_OUT_Y_H              0x2B
#define H3LIS331DL_OUT_Z_L              0x2C   //z-axis acceleration data
#define H3LIS331DL_OUT_Z_H              0x2D


//INTERRUPT 1 CONFIGURATION 
#define H3LIS331DL_INT1_CFG             0x30

//INTERRUPT 2 CONFIGURATION 
#define H3LIS331DL_INT2_CFG             0x34
#define H3LIS331DL_ANDOR                BIT(7)
#define H3LIS331DL_INT_6D               BIT(6)

//INT REGISTERS 
#define H3LIS331DL_INT1_THS             0x32
#define H3LIS331DL_INT1_DURATION        0x33
#define H3LIS331DL_INT2_THS             0x36
#define H3LIS331DL_INT2_DURATION        0x37

//INTERRUPT 1 SOURCE REGISTER 
#define H3LIS331DL_INT1_SRC             0x31
#define H3LIS331DL_INT2_SRC             0x35

//INT_CFG  bit mask
#define H3LIS331DL_INT_AND              0x80
#define H3LIS331DL_INT_OR               0x00
#define H3LIS331DL_INT_ZHIE_ENABLE      0x20
#define H3LIS331DL_INT_ZHIE_DISABLE     0x00
#define H3LIS331DL_INT_ZLIE_ENABLE      0x10
#define H3LIS331DL_INT_ZLIE_DISABLE     0x00
#define H3LIS331DL_INT_YHIE_ENABLE      0x08
#define H3LIS331DL_INT_YHIE_DISABLE     0x00
#define H3LIS331DL_INT_YLIE_ENABLE      0x04
#define H3LIS331DL_INT_YLIE_DISABLE     0x00
#define H3LIS331DL_INT_XHIE_ENABLE      0x02
#define H3LIS331DL_INT_XHIE_DISABLE     0x00
#define H3LIS331DL_INT_XLIE_ENABLE      0x01
#define H3LIS331DL_INT_XLIE_DISABLE     0x00

//INT_SRC  bit mask
#define H3LIS331DL_INT_SRC_IA           0x40
#define H3LIS331DL_INT_SRC_ZH           0x20
#define H3LIS331DL_INT_SRC_ZL           0x10
#define H3LIS331DL_INT_SRC_YH           0x08
#define H3LIS331DL_INT_SRC_YL           0x04
#define H3LIS331DL_INT_SRC_XH           0x02
#define H3LIS331DL_INT_SRC_XL           0x01

//STATUS REGISTER bit mask
#define H3LIS331DL_STATUS_REG_ZYXOR     0x80    // 1:new data set has over written the previous one
                                                // 0:no overrun has occurred (default)  
#define H3LIS331DL_STATUS_REG_ZOR       0x40    // 0:no overrun has occurred (default)
                                                // 1:new Z-axis data has over written the previous one
#define H3LIS331DL_STATUS_REG_YOR       0x20    // 0:no overrun has occurred (default)
                                                // 1:new Y-axis data has over written the previous one
#define H3LIS331DL_STATUS_REG_XOR       0x10    // 0:no overrun has occurred (default)
                                                // 1:new X-axis data has over written the previous one
#define H3LIS331DL_STATUS_REG_ZYXDA     0x08    // 0:a new set of data is not yet avvious one
                                                // 1:a new set of data is available 
#define H3LIS331DL_STATUS_REG_ZDA       0x04    // 0:a new data for the Z-Axis is not availvious one
                                                // 1:a new data for the Z-Axis is available
#define H3LIS331DL_STATUS_REG_YDA       0x02    // 0:a new data for the Y-Axis is not available
                                                // 1:a new data for the Y-Axis is available
#define H3LIS331DL_STATUS_REG_XDA       0x01    // 0:a new data for the X-Axis is not available
                                                // 1:a new data for the X-Axis is available
#define H3LIS331DL_DATAREADY_BIT        H3LIS331DL_STATUS_REG_ZYXDA

#define ValBit(VAR,Place)         (VAR & (1<<Place))
#define BIT(x) ( (x) )

typedef uint8_t H3LIS331DL_Axis_t;
typedef uint8_t H3LIS331DL_IntConf_t;

//define structure
typedef enum {
    MEMS_SUCCESS  = 0x01,
    MEMS_ERROR  = 0x00  
} status_t;

typedef enum {
    MEMS_ENABLE   = 0x01,
    MEMS_DISABLE    = 0x00  
} State_t;

typedef struct {
    int16_t AXIS_X;
    int16_t AXIS_Y;
    int16_t AXIS_Z;
} AxesRaw_t;

typedef enum {  
    H3LIS331DL_ODR_50Hz    = 0x00,
    H3LIS331DL_ODR_100Hz   = 0x01,  
    H3LIS331DL_ODR_400Hz   = 0x02,
    H3LIS331DL_ODR_1000Hz  = 0x03
} H3LIS331DL_ODR_t;

typedef enum {
    H3LIS331DL_CONTINUOUS_MODE = 0x00,
    H3LIS331DL_SINGLE_MODE     = 0x01,
    H3LIS331DL_SLEEP_MODE      = 0x02
} H3LIS331DL_Mode_M_t;

typedef enum {
    H3LIS331DL_POWER_DOWN   = 0x00,
    H3LIS331DL_NORMAL       = 0x01,
    H3LIS331DL_LOW_POWER_05 = 0x02,
    H3LIS331DL_LOW_POWER_1  = 0x03,
    H3LIS331DL_LOW_POWER_2  = 0x04,
    H3LIS331DL_LOW_POWER_5  = 0x05,
    H3LIS331DL_LOW_POWER_10 = 0x06,
} H3LIS331DL_Mode_t;

typedef enum {
    H3LIS331DL_HPM_NORMAL_MODE_RES = 0x00,
    H3LIS331DL_HPM_REF_SIGNAL      = 0x01,
    H3LIS331DL_HPM_NORMAL_MODE     = 0x02,
} H3LIS331DL_HPFMode_t;

typedef enum {
    H3LIS331DL_HPFCF_0 = 0x00,
    H3LIS331DL_HPFCF_1 = 0x01,
    H3LIS331DL_HPFCF_2 = 0x02,
    H3LIS331DL_HPFCF_3 = 0x03,
} H3LIS331DL_HPFCutOffFreq_t;

typedef enum {
    H3LIS331DL_INT_SOURCE      = 0x00,
    H3LIS331DL_INT_1OR2_SOURCE = 0x01,
    H3LIS331DL_DATA_READY      = 0x02,
    H3LIS331DL_BOOT_RUNNING    = 0x03
} H3LIS331DL_INT_Conf_t;

typedef enum {
    H3LIS331DL_SLEEP_TO_WAKE_DIS    = 0x00,
    H3LIS331DL_SLEEP_TO_WAKE_ENA    = 0x03,
} H3LIS331DL_Sleep_To_Wake_Conf_t;

typedef enum {
    H3LIS331DL_FULLSCALE_2    = 0x00,//±100 g
    H3LIS331DL_FULLSCALE_4    = 0x01,//±200 g
    H3LIS331DL_FULLSCALE_8    = 0x03,//±400 g
} H3LIS331DL_Fullscale_t;

typedef enum {
    H3LIS331DL_BLE_LSB        = 0x00,
    H3LIS331DL_BLE_MSB        = 0x01
} H3LIS331DL_Endianess_t;

typedef enum {
    H3LIS331DL_SPI_4_WIRE = 0x00,
    H3LIS331DL_SPI_3_WIRE = 0x01
} H3LIS331DL_SPIMode_t;

typedef enum {
    H3LIS331DL_X_ENABLE     = 0x01,
    H3LIS331DL_X_DISABLE    = 0x00,
    H3LIS331DL_Y_ENABLE     = 0x02,
    H3LIS331DL_Y_DISABLE    = 0x00,
    H3LIS331DL_Z_ENABLE     = 0x04,
    H3LIS331DL_Z_DISABLE    = 0x00    
} H3LIS331DL_AXISenable_t;

typedef enum {
    H3LIS331DL_UP_SX  = 0x44,
    H3LIS331DL_UP_DX  = 0x42,
    H3LIS331DL_DW_SX  = 0x41,
    H3LIS331DL_DW_DX  = 0x48,
    H3LIS331DL_TOP    = 0x60,
    H3LIS331DL_BOTTOM = 0x50
} H3LIS331DL_POSITION_6D_t;

typedef enum {
    H3LIS331DL_INT_MODE_OR            = 0x00,
    H3LIS331DL_INT_MODE_6D_MOVEMENT   = 0x01,
    H3LIS331DL_INT_MODE_AND           = 0x02,
    H3LIS331DL_INT_MODE_6D_POSITION   = 0x03  
} H3LIS331DL_IntMode_t;


void H3LIS331DL_init(int DevHandle,int IICIndex,H3LIS331DL_ODR_t  odr, H3LIS331DL_Mode_t mode,H3LIS331DL_Fullscale_t fullScale);
void importPara(int16_t val_x, int16_t val_y, int16_t val_z);
void readXYZ(int16_t* x, int16_t* y, int16_t* z);
void getAcceleration(double* xyz);
//Sensor Configuration Functions
status_t getWHO_AM_I(unsigned char* val);
status_t setODR(H3LIS331DL_ODR_t dr);
status_t setMode(H3LIS331DL_Mode_t pm);
status_t setAxis(H3LIS331DL_Axis_t axis);
status_t setFullScale(H3LIS331DL_Fullscale_t fs);
status_t setBDU(State_t bdu);
status_t setBLE(H3LIS331DL_Endianess_t ble);
status_t setSelfTest(State_t st);
status_t setSelfTestSign(State_t st_sign);
status_t turnONEnable(H3LIS331DL_Sleep_To_Wake_Conf_t stw);
status_t setBOOT(State_t boot);
status_t setFDS(State_t fds);
status_t setSPI34Wire(H3LIS331DL_SPIMode_t sim);

//Filtering Functions
status_t setHPFMode(H3LIS331DL_HPFMode_t hpm);
status_t setHPFCutOFF(H3LIS331DL_HPFCutOffFreq_t hpf);
status_t setFilterDataSel(State_t state);
status_t setReference(int8_t ref);

//Interrupt Functions
status_t setIntHighLow(State_t hil);
status_t setIntPPOD(State_t pp_od);
status_t setInt1DataSign(H3LIS331DL_INT_Conf_t i_cfg);
status_t setInt2DataSign(H3LIS331DL_INT_Conf_t i_cfg);
status_t setInt1HPEnable(State_t stat);
status_t setInt2HPEnable(State_t stat);
status_t int1LatchEnable(State_t latch);
status_t int2LatchEnable(State_t latch);
status_t resetInt1Latch(void);
status_t resetInt2Latch(void);
status_t setInt1Configuration(H3LIS331DL_IntConf_t ic);
status_t setInt2Configuration(H3LIS331DL_IntConf_t ic);
status_t setInt1Threshold(unsigned char ths);
status_t setInt2Threshold(unsigned char ths);
status_t setInt1Duration(unsigned char id);
status_t setInt2Duration(unsigned char id);
status_t setInt1Mode(H3LIS331DL_IntMode_t int_mode);
status_t setInt2Mode(H3LIS331DL_IntMode_t int_mode);
status_t getInt1Src(unsigned char* val);
status_t getInt2Src(unsigned char* val);
status_t getInt1SrcBit(unsigned char statusBIT, unsigned char* val);
status_t getInt2SrcBit(unsigned char statusBIT, unsigned char* val); 

//Other Reading Functions
status_t getStatusReg(unsigned char* val);
status_t getStatusBit(unsigned char statusBIT, unsigned char* val);
status_t H3LIS331DL_getAccAxesRaw(AxesRaw_t* buff);
status_t get6DPositionInt1(unsigned char* val);
status_t get6DPositionInt2(unsigned char* val);
    
uint8_t readReg(unsigned char deviceAddr, unsigned char Reg, unsigned char* Data);
uint8_t readRegs(unsigned char deviceAddr, unsigned char StartReg, unsigned char* Data,unsigned char DataLen);
uint8_t writeReg(unsigned char deviceAddress, unsigned char WriteAddr, unsigned char Data);

#endif /*__H3LIS331DL_H */
