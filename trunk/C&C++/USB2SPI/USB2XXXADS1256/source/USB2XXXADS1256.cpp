  /*
  ******************************************************************************
  * @file     : USB2XXXADS1256.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI ADS1256 test demo
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
#include "usb2ads1256.h"
/*
硬件连接说明,注意蓝色端子的电源必须接上，否则可能会导致初始化失败
USB2XXX_CH1  USB2XXX_CH0   ADS1256|
----------------------------------|
P0           P8            DRDY   |
P1           P9            RESET  |
P4           P13           CS     |
P5           P12           SCLK   |
P6           P11           DOUT   |
P7           P10           DIN    |
3V3                        DVDD   |
GND                        DGND   |
*/

//主函数
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    unsigned char WriteBuffer[8]={0};
    unsigned char ReadDataBuffer[8];
    int DevHandle[10];
    int ADS1256Channel = 1;
    bool state;
    int ret,k=0,DataNum=0;
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
    //初始化配置ADS1256
    ADS1256_CONFIG ADS1256Config;
    ADS1256Config.BufferEn = 0;//不使能内部Buffer
    ADS1256Config.PGA = 0;//内部增益为1，具体增益值为2的PGA次方
    ADS1256Config.SampleRate = ADS1256_DRATE_1000SPS;//ADS1256数据采样率;
    ADS1256Config.SampleMode = 0x05;//单端模式，采集AIN0和AIN1信号
    ret = ADS1256_Init(DevHandle[0],ADS1256Channel,&ADS1256Config);
    if(ret != ADS1256_SUCCESS){
        printf("Initialize ADS1256 error!\n");
        return 0;
    }
    //启动ADS1256
    ret = ADS1256_Start(DevHandle[0],ADS1256Channel);
    if(ret != ADS1256_SUCCESS){
        printf("Start ADS1256 error!\n");
        return 0;
    }
    //延时一段时间后读取数据
    Sleep(10);
    int DataBuffer[20*1024];
    ret = ADS1256_Read(DevHandle[0],ADS1256Channel,DataBuffer);
    if(ret < 0){
        printf("Read ADS1256 error!\n");
        return 0;
    }else{
        DataNum += ret;
    }
    //延时一段时间后读取数据
    Sleep(10);
    ret = ADS1256_Read(DevHandle[0],ADS1256Channel,&DataBuffer[DataNum]);
    if(ret < 0){
        printf("Read ADS1256 error!\n");
        return 0;
    }else{
        DataNum += ret;
    }
    //停止ADS1256
    ret = ADS1256_Stop(DevHandle[0],ADS1256Channel);
    if(ret != ADS1256_SUCCESS){
        printf("Stop ADS1256 error!\n");
        return 0;
    }
    //延时一段时间后读取数据
    Sleep(5);
    ret = ADS1256_Read(DevHandle[0],ADS1256Channel,&DataBuffer[DataNum]);
    if(ret < 0){
        printf("Read ADS1256 error!\n");
        return 0;
    }else{
        DataNum += ret;
    }
    for(int i=0;i<DataNum;i+=2){
        printf("ADCDataBuffer[%d] = %f\t",i,(DataBuffer[i]*0.59604644775390625)/(double)((1<<(int)ADS1256Config.PGA)*(1000000)));
        printf("ADCDataBuffer[%d] = %f\r\n",i+1,(DataBuffer[i+1]*0.59604644775390625)/(double)((1<<(int)ADS1256Config.PGA)*(1000000)));
    }
    printf("ADS1256 Test End!\n");
    return 0;
}