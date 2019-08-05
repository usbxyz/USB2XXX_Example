  /*
  ******************************************************************************
  * @file     : USB2XXX_DAC_Test.cpp
  * @Copyright: toomoss 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX DAC test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2014, toomoss.com
  * http://www.toomoss.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "usb_device.h"
#include "usb2dac.h"
#include <math.h>
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    bool state;
    int ret;
    unsigned short DACBuffer[512];//缓冲区数据不能大于512
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
    //产生方波信号
    for(int i=0;i<256;i++){
        DACBuffer[i] = 0xFFF;
    }
    for(int i=256;i<512;i++){
        DACBuffer[i] = 0x0;
    }
    //初始化ADC,初始化配置后会自动输出波形
    int TimeOfUs = 4;//每个数据输出的时间周期 = TimeOfUs*0.25 us
    ret = DAC_Init(DevHandle[0],TimeOfUs,DACBuffer,512);//周期 = 0.25*TimeOfUs*512 us
    if(ret != DAC_SUCCESS){
        printf("Init dac error!\n");
        return 0;
    }
    getchar();
    //输出正弦信号
    float dcVoltage = 1;    //直流分量
    float amplitude = 2.3;  //幅值，幅值加上直流分量不能大于3.3V
    int DataNum=64;//周期 = 0.25*TimeOfUs*DataNum us
    for(int i=0;i<DataNum;i++){
        DACBuffer[i] = ((dcVoltage+amplitude*(1+sin(2*3.141592653589793*i/DataNum))/2.0)*4095)/3.3;
    }
    //初始化ADC,初始化配置后会自动输出波形
    ret = DAC_Init(DevHandle[0],TimeOfUs,DACBuffer,DataNum);//周期 = 0.25*TimeOfUs*512 us
    if(ret != DAC_SUCCESS){
        printf("Init dac error!\n");
        return 0;
    }
    getchar();
    //输出三角波信号
    for(int i=0;i<DataNum/2;i++){
        DACBuffer[i] = (dcVoltage+amplitude*(i/(DataNum/2.0)))*4095.0/3.3;
    }
    for(int i=0;i<DataNum/2;i++){
        DACBuffer[i+DataNum/2] = (dcVoltage+amplitude*((DataNum/2.0-i)/(DataNum/2.0)))*4095.0/3.3;
    }
    getchar();
    //停止数据输出
    DAC_Stop(DevHandle[0]);
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    return 0;
}

