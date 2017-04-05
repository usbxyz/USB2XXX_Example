  /*
  ******************************************************************************
  * @file     : USB2XXXSPITest.cpp
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
    int DevHandle[10];
    int SPIIndex = 0;
    bool state;
    int ret;
    unsigned char WriteBuffer[64];
    unsigned char ReadBuffer[20480];
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
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,NULL);
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
    //擦出用户数据区域的所有数据
    state = DEV_EraseUserData(DevHandle[0]);
    if(!state){
        printf("Erase user data error!\n");
        return 0;
    }
    //向用户区域写入数据
    unsigned char UserData[512]={0};
    strcpy((char*)UserData,"Hello user define data!");
    state = DEV_WriteUserData(DevHandle[0],0,UserData,sizeof(UserData));
    if(!state){
        printf("User data write data error!\n");
        return 0;
    }
    memset(UserData,0,sizeof(UserData));
    //从用户区读取数据
    state = DEV_ReadUserData(DevHandle[0],0,UserData,sizeof(UserData));
    if(!state){
        printf("User data write data error!\n");
        return 0;
    }else{
        printf("USER DATA = %s \n",UserData);
    }
    return 0;
    //配置SPI总线相关参数
    SPIConfig.Mode = 0;
    SPIConfig.ClockSpeedHz = 50000000;
    SPIConfig.CPHA = 0;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = 0;
    SPIConfig.Master = 1;
    SPIConfig.SelPolarity = 0;
    ret = SPI_Init(DevHandle[0],SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //SPI发送数据
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteBuffer,sizeof(WriteBuffer));
    if(ret != SPI_SUCCESS){
        printf("SPI write data error!\n");
        return 0;
    }
    //SPI异步发送数据，调用该函数后会立即返回，但是SPI数据不一定发送完毕，但是在下一次发送数据之前会保证数据发送完毕
    for(int i=0;i<64;i++){
        ret = SPI_WriteBytesAsync(DevHandle[0],SPIIndex,WriteBuffer,sizeof(WriteBuffer));
        if(ret != SPI_SUCCESS){
            printf("SPI async write data error!\n");
            return 0;
        }
    }
    //SPI接收数据
    ret = SPI_ReadBytes(DevHandle[0],SPIIndex,ReadBuffer,sizeof(ReadBuffer));
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
    //SPI先发送数据，再接收数据，整个过程片选信号一直有效
    int IntervalTime = 10;//发送和接收数据之间的时间间隔，单位为us
    ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteBuffer,sizeof(WriteBuffer),ReadBuffer,sizeof(ReadBuffer),IntervalTime);
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

