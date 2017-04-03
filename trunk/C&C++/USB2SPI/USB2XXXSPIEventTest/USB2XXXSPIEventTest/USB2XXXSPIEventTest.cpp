  /*
  ******************************************************************************
  * @file     : USB2XXXSPIEventTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI test demo
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
#include "usb_device.h"
#include "usb2spi.h"


int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    SPI_CONFIG SPIConfig;
    int DevIndex = 0;
    int SPIIndex = 0;
    bool state;
    int ret;
    unsigned char WriteBuffer[64];
    unsigned char ReadBuffer[64];
    //扫描查找设备
    ret = USB_ScanDevice(NULL);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevIndex);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //获取固件信息
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,NULL);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
	    printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%08X\n",DevInfo.Functions);
    }
 
    //配置SPI总线相关参数
    SPIConfig.Mode = 0;
    SPIConfig.ClockSpeedHz = 50000000;
    SPIConfig.CPHA = 0;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = 0;
    SPIConfig.Master = 1;
    SPIConfig.SelPolarity = 0;
    ret = SPI_Init(DevIndex,SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //SPI发送数据
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    //指定引脚为低电平的时候发送数据
    ret = SPI_WriteBytesOfEvent(DevIndex,SPIIndex,WriteBuffer,sizeof(WriteBuffer),SPI_EVENT_P15,EVENT_TYPE_LOW,10000);
    if(ret != SPI_SUCCESS){
        printf("SPI write data error!\n");
        return 0;
    }else{
        printf("SPI_WriteBytesOfEvent Success!\n");
    }
    getchar();
    //指定引脚为高电平的时候读取数据
    ret = SPI_ReadBytesOfEvent(DevIndex,SPIIndex,ReadBuffer,sizeof(ReadBuffer),SPI_EVENT_P15,EVENT_TYPE_HIGH,10000);
    if(ret != SPI_SUCCESS){
        printf("SPI read data error!\n");
        return 0;
    }else{
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
            if(((i+1)%16)==0){
                printf("\n");
            }
        }
        printf("\n");
    }
    getchar();
    //指定引脚出现下降沿的时候SPI先发送数据，再接收数据，整个过程片选信号一直有效
    int IntervalTime = 10;//发送和接收数据之间的时间间隔，单位为us
    ret = SPI_WriteReadBytesOfEvent(DevIndex,SPIIndex,WriteBuffer,sizeof(WriteBuffer),ReadBuffer,sizeof(ReadBuffer),IntervalTime,SPI_EVENT_P15,EVENT_TYPE_FALLING,10000);
    if(ret != SPI_SUCCESS){
        printf("SPI write read data error!\n");
        return 0;
    }else{
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
            if(((i+1)%16)==0){
                printf("\n");
            }
        }
        printf("\n");
    }
    getchar();
    printf("Test SPI_SUCCESS!\n");
    return 0;
}

