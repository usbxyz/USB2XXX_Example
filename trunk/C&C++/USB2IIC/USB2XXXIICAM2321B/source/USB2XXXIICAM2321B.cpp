  /*
  ******************************************************************************
  * @file     : USB2XXXIICTest.cpp
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

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int IICIndex = 3;
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
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FunctionStr);
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
    IIC_CONFIG IIC_Config;
    IIC_Config.EnablePu = 1;            //使能内部上拉电阻
    IIC_Config.AddrBits = 7;            //7bit地址模式
    IIC_Config.ClockSpeedHz = 100000;   //时钟频率400KHz
    IIC_Config.Master = 1;              //主机模式
    ret = IIC_Init(DevHandle[0],IICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config IIC failed!\n");
        return 0;
    }else{
        printf("Config IIC IIC_SUCCESS!\n");
    }
    while(true)
    {
        //唤醒传感器
        IIC_WriteBytes(DevHandle[0],IICIndex,0x5C,NULL,0,10);
        Sleep(10);
        //发送数据测量命令
        unsigned char WriteBuffer[]={0x03,0x00,0x04};
        ret = IIC_WriteBytes(DevHandle[0],IICIndex,0x5C,WriteBuffer,sizeof(WriteBuffer),10);
        if(ret != IIC_SUCCESS){
            printf("Write IIC failed!\n");
            printf("Error Code:%d\n",ret);
            return 0;
        }
        Sleep(10);
        //读取测量的数据
        unsigned char ReadBuffer[8];
        ret = IIC_ReadBytes(DevHandle[0],IICIndex,0x5C,ReadBuffer,sizeof(ReadBuffer),10);
        if (ret != IIC_SUCCESS){
            printf("Read IIC failed!\n");
            printf("Error Code:%d\n",ret);
            return 0;
        }else{
            double t = ((ReadBuffer[4] << 8) | ReadBuffer[5]) / 10.0;
            system("cls");
            printf("Temperature: %.1f\n",t);
            double h = ((ReadBuffer[2] << 8) | ReadBuffer[3]) / 10.0;
            printf("Huminity:%.1f\n",h);
        }
        //延时
#ifndef OS_UNIX
        Sleep(1000);
#else
        usleep(1000*1000);
#endif
    }
    return 0;
}