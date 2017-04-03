  /*
  ******************************************************************************
  * @file     : USB2XXXSMBusTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX IIC test demo
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
#include "usb2iic.h"

#define GET_FIRMWARE_INFO   1
#define AUTO_GET_SLAVE_ADDR 0


int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevIndex = 0;
    int IICIndex = 0;
    bool state;
    int ret;
    unsigned int PinValue;
    short Buffer[40960];
    short SlaveAddr = 0x50;
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
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //获取固件信息
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,FunctionStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Info:\n");
	    printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%s\n",FunctionStr);
    }
#endif
    //初始化配置I2C
    ret = SMBUS_Init(DevIndex,IICIndex,100000,1);
    if(ret != IIC_SUCCESS){
        printf("Config IIC failed!\n");
        return 0;
    }else{
        printf("Config IIC IIC_SUCCESS!\n");
    }
#if AUTO_GET_SLAVE_ADDR
    //获取总线上能应答的从机地址
    short SlaveAddrs[128];  //地址存储缓冲区
    int SlaveAddrNum = 0;   //返回应答的地址个数
    SlaveAddrNum = IIC_GetSlaveAddr(DevIndex,IICIndex,SlaveAddrs);
    if(SlaveAddrNum >= 0){
        printf("Get %d slave address!\n",SlaveAddrNum);
        for(int i=0;i<SlaveAddrNum;i++){
            printf("%02X ",SlaveAddrs[i]);
            SlaveAddr = SlaveAddrs[i];
        }
        printf("\n");
    }else{
        printf("Get slave address error!\n");
        return 0;
    }
#endif
    //SMBUS_QuickCommand
    ret = SMBUS_QuickCommand(DevIndex,IICIndex,SlaveAddr,1);
    if(ret != IIC_SUCCESS){
        printf("SMBUS_QuickCommand Failed!\n");
        printf("Error Code:%d\n",ret);
        return 0;
    }
    //SMBUS_WriteByte
    ret = SMBUS_WriteByte(DevIndex,IICIndex,SlaveAddr,0x00,1);
    if(ret != IIC_SUCCESS){
        printf("SMBUS_WriteByte Failed!\n");
        printf("Error Code:%d\n",ret);
        return 0;
    }
    //
    //延时
#ifndef OS_UNIX
    Sleep(10);
#else
    usleep(10*1000);
#endif
    //延时
#ifndef OS_UNIX
    Sleep(10);
#else
    usleep(10*1000);
#endif

    //关闭设备
    USB_CloseDevice(DevIndex);
	return 0;
}

