  /*
  ******************************************************************************
  * @file     : USB2XXXIICDHT8X.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX IIC DHT8X demo
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
//注意，使用这个程序必须烧指定的固件才行，可以联系我（QQ:188298598）提供
int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int IICIndex = 2;//I2C通道号必须大于等于2
    bool state;
    int ret;
    unsigned int PinValue;
    short Buffer[40960];
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        getchar();
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        getchar();
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
    IIC_Config.ClockSpeedHz = 100000;     //时钟频率400KHz
    IIC_Config.Master = 1;              //主机模式
    ret = IIC_Init(DevHandle[0],IICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config IIC failed!\n");
        getchar();
        return 0;
    }else{
        printf("Config IIC IIC_SUCCESS!\n");
    }
    while(true)
    {
        float t;
        //IIC 读数据
        unsigned char ReadBuffer[3];
        ret = IIC_ReadBytes(DevHandle[0],IICIndex,0x03,ReadBuffer,sizeof(ReadBuffer),1000);//读取温度值
        if(ret != IIC_SUCCESS){
            printf("Read IIC failed!\n");
            printf("Error Code:%d\n",ret);
            getchar();
            return 0;
        }else{
            t = -39.6 + 0.01*((ReadBuffer[0]<<8)|(ReadBuffer[1]));
            printf("T_RAW: %d\n",(ReadBuffer[0]<<8)|(ReadBuffer[1]));
            printf("T: %f\n",t);
        }
        ret = IIC_ReadBytes(DevHandle[0],IICIndex,0x05,ReadBuffer,sizeof(ReadBuffer),1000);//读取湿度值
        if(ret != IIC_SUCCESS){
            printf("Read IIC failed!\n");
            printf("Error Code:%d\n",ret);
            getchar();
            return 0;
        }else{
            printf("H_RAW: %d\n",(ReadBuffer[0]<<8)|(ReadBuffer[1]));
            float RHlinear = -2.0468 + 0.0367*((ReadBuffer[0]<<8)|(ReadBuffer[1]))+((ReadBuffer[0]<<8)|(ReadBuffer[1]))*((ReadBuffer[0]<<8)|(ReadBuffer[1]))*(-0.0000015955);
            printf("RHlinear: %f\n",RHlinear);
            float RHtrue = (t-25)*(0.01+0.00008*((ReadBuffer[0]<<8)|(ReadBuffer[1])))+RHlinear;
            printf("RHtrue: %f\n",RHtrue);
        }
    //延时
#ifndef OS_UNIX
        Sleep(5000);
#else
        usleep(5000*1000);
#endif
    }

    //关闭设备
    USB_CloseDevice(DevHandle[0]);
	return 0;
}

