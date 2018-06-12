  /*
  ******************************************************************************
  * @file     : USB2XXXOWDS18B20.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2017/05/27 17:15
  * @brief    : DS18B20 test demo
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
#include <time.h>  
#include "usb_device.h"
#include "usb2ow.h"

//主函数
int main(int argc, const char* argv[])
{
    int DevHandle[10];
    uint8_t WriteDataBuffer[64];
    uint8_t ReadDataBuffer[64];
    DEVICE_INFO DevInfo;
    bool state;
    int ret;
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

    OW_Init(DevHandle[0],OW_CH0,500,1);
    while(1)
    {
        WriteDataBuffer[0] = 0xCC;
        WriteDataBuffer[1] = 0x44;
		OW_Reset(DevHandle[0],OW_CH0);
        OW_WriteBytes(DevHandle[0],OW_CH0,WriteDataBuffer,2);
        Sleep(200);
        WriteDataBuffer[0] = 0xCC;
        WriteDataBuffer[1] = 0xBE;
		OW_Reset(DevHandle[0],OW_CH0);
        OW_WriteReadBytes(DevHandle[0],OW_CH0,WriteDataBuffer,2,ReadDataBuffer,2,100);
        int16_t tmp = (ReadDataBuffer[1]<<8)|ReadDataBuffer[0];
        float temperature = (float)tmp * 0.0625;  
        printf("t = %04X\n",tmp);
        printf("temperature = %0.3f\n",temperature);
        Sleep(1000);
    }

    return 0;
}
