  /*
  ******************************************************************************
  * @file     : USB2XXXADCTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX ADC test demo
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
#include <stdint.h>
#include "usb_device.h"
#include "usb2adc.h"

#define CONTINUE_GET_ADC_DATA   1
#define ADC_NUMS                10

uint32_t BitCount(uint32_t data)
{
  uint32_t count=0;
  while(data){
    count++;
    data&=(data-1);
  }
  return count;
}
#if CONTINUE_GET_ADC_DATA
int WINAPI GetAdcDataHandle(int DevIndex,unsigned short *pData,int DataNum)
{
    printf("Get %d Byte Data\n",DataNum);
    printf("ADC Data = %fV\n",(pData[0]*3.3)/4095);
    return 0;
}
#endif
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevIndex = 0;
    bool state;
    int ret;
    char ADC_Channel = 0x01;
    short Buffer[40960];
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
    char FuncStr[256]={0};
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Info:\n");
        printf("    Name:%s\n",DevInfo.FirmwareName);
        printf("    Build Date:%s\n",DevInfo.BuildDate);
        printf("    Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("    Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("    Functions:%08X\n",DevInfo.Functions);
        printf("    Functions:%s\n",FuncStr);
    }
    
    ret = ADC_Init(DevIndex,ADC_Channel,1000000);
    if(ret != ADC_SUCCESS){
        printf("Init adc error!\n");
        return 0;
    }

    ret = ADC_Read(DevIndex,Buffer,ADC_NUMS);
    if(ret != ADC_SUCCESS){
        printf("Read adc error!\n");
        return 0;
    }else{
        for(int i=0;i<ADC_NUMS*BitCount(ADC_Channel);i++){
            printf("ADC Data[%d] = %fV\n",i,(Buffer[i]*3.3)/4095);
        }
    }
#ifndef OS_UNIX
    Sleep(100);
#else
    usleep(100*1000);
#endif
    ret = ADC_Read(DevIndex,Buffer,ADC_NUMS);
    if(ret != ADC_SUCCESS){
        printf("Read adc error!\n");
        return 0;
    }else{
        for(int i=0;i<ADC_NUMS*BitCount(ADC_Channel);i++){
            printf("ADC Data[%d] = %fV\n",i,(Buffer[i]*3.3)/4095);
        }
    }
#if CONTINUE_GET_ADC_DATA
    ret = ADC_StartContinueRead(DevIndex,ADC_Channel,1000000,10240,GetAdcDataHandle);
    if(ret != ADC_SUCCESS){
        printf("Start Continue Read adc error!\n");
        return 0;
    }else{
        printf("Start Continue Read ADC ADC_SUCCESS!\n");
    }
#ifndef OS_UNIX
    Sleep(3000);
#else
    usleep(3000*1000);
#endif

    ret = ADC_StopContinueRead(DevIndex);
    if(ret != ADC_SUCCESS){
        printf("Stop Continue Read adc error!\n");
        return 0;
    }
#endif
/*
    uint16_t ReadDataBuffer[20480];
    int ReadDataNum;
    int AllDataNum = 0;
    do{
        ReadDataNum = ADC_GetData(DevIndex,ReadDataBuffer,20480);
        AllDataNum += ReadDataNum;
    }while(ReadDataNum > 0);
    printf("Get %d Byte Data\n",AllDataNum);
    printf("ADC Data = %fV\n",(ReadDataBuffer[0]*3.3)/4095);
*/
    //关闭设备
    USB_CloseDevice(DevIndex);
	return 0;
}

