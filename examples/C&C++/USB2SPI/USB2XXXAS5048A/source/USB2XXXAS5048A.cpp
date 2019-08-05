  /*
  ******************************************************************************
  * @file     : USB2XXXAS5048A.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI AS5048A test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2014, usbxyz.com
  * http://www.usbxyz.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "usb_device.h"
#include "usb2spi.h"

//主函数
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    int SPIIndex = SPI2_CS0;
    bool state;
    int ret,k=0;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //获取固件信息
    char FuncStr[128]={0};
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("Firmware Functions:%s\n",FuncStr);
    }
    //配置SPI总线相关参数
    SPI_CONFIG SPIConfig;
    SPIConfig.Mode = SPI_MODE_HARD_FDX;
    SPIConfig.ClockSpeedHz = 390450*8;
    SPIConfig.CPHA = 1;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = SPI_MSB;
    SPIConfig.Master = SPI_MASTER;
    SPIConfig.SelPolarity = SPI_SEL_LOW;
    ret = SPI_Init(DevHandle[0],SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //循环读数据
    unsigned char write_buffer[]={0xFF,0xFF};
    unsigned char read_buffer[2];
    int ReadNum = 100;
    while(ReadNum--){
        ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,write_buffer,2,read_buffer,2,0);
        if(ret != SPI_SUCCESS){
            printf("Read data error!\n");
            return 0;
        }else{
            char PAR = 0;
            unsigned short data = (read_buffer[0]<<8)|read_buffer[1];
            //打印原始数据
            printf("data = %04X\n",data&0x3FFF);
            //检测Parity bit (EVEN)
            for(int i=0;i<15;i++){
                if(data&(1<<i)){
                    PAR++;
                }
            }
            if((PAR&0x01)!=((data>>15)&0x01)){
                printf("EVEN check error!\n");
            }
            //Error flag indicating a transmission error in a previous host transmission
            if(data&(1<<14)){
                printf("EF is set\n");
            }
        }
        Sleep(1000);
    }
}