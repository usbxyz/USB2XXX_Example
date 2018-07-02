  /*
  ******************************************************************************
  * @file     : USB2XXXGPIOTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX GPIO test demo
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
#include "usb2gpio.h"
#include "usb_device.h"


int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    bool state;
    int ret;
    unsigned int PinValue;
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
    char FuncStr[256]={0};
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Info:\n");
        printf("    Name:%s\n",DevInfo.FirmwareName);
        printf("    Build Date:%s\n",DevInfo.BuildDate);
        printf("    Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("    Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("    Functions:%08X\n",DevInfo.Functions);
        printf("    Functions:%s\n",FuncStr);
        printf("%08X",DevInfo.FirmwareVersion);
    }
	//设置GPIO电平（需要适配器支持）
	DEV_SetPowerLevel(DevHandle[0],POWER_LEVEL_3V3);
    //输出测试——没上下拉
    GPIO_SetOutput(DevHandle[0],0xFFFF,0);
    for(int i=0;i<10;i++){
        GPIO_Write(DevHandle[0],0xFFFF,0xAAAA);
        GPIO_Write(DevHandle[0],0xFFFF,0x5555);
    }
    //输出测试——上拉
    GPIO_SetOutput(DevHandle[0],0xFFFF,1);
    for(int i=0;i<10;i++){
        GPIO_Write(DevHandle[0],0xFFFF,0xAAAA);
        GPIO_Write(DevHandle[0],0xFFFF,0x5555);
    }
    //输出测试——下拉
    GPIO_SetOutput(DevHandle[0],0xFFFF,2);
    for(int i=0;i<10;i++){
        GPIO_Write(DevHandle[0],0xFFFF,0xAAAA);
        GPIO_Write(DevHandle[0],0xFFFF,0x5555);
    }
    //测试输入——浮空
    GPIO_SetInput(DevHandle[0],0xFFFF,0);
    GPIO_Read(DevHandle[0],0xFFFF,&PinValue);
    printf("READ DATA(Float):%04X\n",PinValue);
    //测试输入——上拉输入
    GPIO_SetInput(DevHandle[0],0xFFFF,1);
    GPIO_Read(DevHandle[0],0xFFFF,&PinValue);
    printf("READ DATA(Pu):%04X\n",PinValue);
    //测试输入——下拉输入
    GPIO_SetInput(DevHandle[0],0xFFFF,2);
    GPIO_Read(DevHandle[0],0xFFFF,&PinValue);
    printf("READ DATA(Pd):%04X\n",PinValue);
    //测试开漏输入——浮空
    GPIO_SetOpenDrain(DevHandle[0],0xFFFF,0);
    GPIO_Read(DevHandle[0],0xFFFF,&PinValue);
    printf("READ DATA(OD-Float):%04X\n",PinValue);
    //测试开漏输入——上拉输入
    GPIO_SetOpenDrain(DevHandle[0],0xFFFF,1);
    GPIO_Read(DevHandle[0],0xFFFF,&PinValue);
    printf("READ DATA(OD-Pu):%04X\n",PinValue);
    //测试开漏输入——下拉输入
    GPIO_SetOpenDrain(DevHandle[0],0xFFFF,2);
    GPIO_Read(DevHandle[0],0xFFFF,&PinValue);
    printf("READ DATA(OD-Pd):%04X\n",PinValue);
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    return 0;
}

