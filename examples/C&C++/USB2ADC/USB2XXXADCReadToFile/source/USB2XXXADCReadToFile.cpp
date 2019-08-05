  /*
  ******************************************************************************
  * @file     : USB2XXXADCReadToFile.cpp
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

FILE *pADCFile;

//连续读取ADC数据的回调函数
int WINAPI GetAdcDataHandle(int DevHandle,unsigned short *pData,int DataNum)
{
    fwrite(pData,sizeof(unsigned short),DataNum,pADCFile);
    return 0;
}

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    bool state;
    int ret;
    char ADC_Channel = 0x0F;//使能ADC_CH0,ADC_CH1,ADC_CH2,ADC_CH3
    short Buffer[40960];
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
        printf("    Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("    Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("    Functions:%08X\n",DevInfo.Functions);
        printf("    Functions:%s\n",FuncStr);
    }
    const char *pADCFileName = "ADC_Data.dat";
    //gets(FileName);
    printf("Start get data to file!\n");
    printf("Input 'q' to exit the data reception!\n");
    //printf("Data saved to data.csv\n");
    pADCFile=fopen(pADCFileName,"wb"); //获取文件的指针
    if(pADCFile == NULL){
        printf("Open file faild\n");
        getchar();
        return 0;
    }

    //启动连续读取ADC数据，数据读取到之后会调用GetAdcDataHandle回调函数，可以在该回调函数里面处理数据
    ret = ADC_StartContinueRead(DevHandle[0],ADC_Channel,100000,10240,GetAdcDataHandle);
    if(ret != ADC_SUCCESS){
        printf("Start Continue Read adc error!\n");
        return 0;
    }else{
        printf("Start Continue Read ADC ADC_SUCCESS!\n");
    }
    //等待输入'q'退出
    char DataTemp;
    while(1){
        scanf("%c",&DataTemp);
        if(DataTemp=='q'){
            break;
        }
    }
    printf("Ready to exit!\n");
    //停止连续读取数据
    ret = ADC_StopContinueRead(DevHandle[0]);
    if(ret != ADC_SUCCESS){
        printf("Stop Continue Read adc error!\n");
        return 0;
    }else{
        printf("Stop Continue Read adc success!\n");
    }
    fclose(pADCFile);
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    return 0;
}

