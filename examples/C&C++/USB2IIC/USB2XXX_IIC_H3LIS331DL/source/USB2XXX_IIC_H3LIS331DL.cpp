  /*
  ******************************************************************************
  * @file     : USB2XXX_IIC_H3LIS331DL.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2018/04/16 9:33
  * @brief    : USB2XXX IIC H3LIS331DL demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2018, usbxyz.com
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
#include "H3LIS331DL.h"

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int IICIndex = 0;//注意要跟硬件连接匹配
    bool state;
    int ret;
    unsigned char Buffer[32];
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
    IIC_Config.ClockSpeedHz = 400000;     //时钟频率400KHz
    IIC_Config.Master = 1;              //主机模式
    ret = IIC_Init(DevHandle[0],IICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config IIC failed!\n");
        getchar();
        return 0;
    }else{
        printf("Config IIC IIC_SUCCESS!\n");
    }
	//初始化配置传感器
	H3LIS331DL_init(DevHandle[0],IICIndex,H3LIS331DL_ODR_100Hz, H3LIS331DL_NORMAL,H3LIS331DL_FULLSCALE_2);
#ifndef OS_UNIX
        Sleep(100);
#else
        usleep(100*1000);
#endif
    //循环获取数据
    for(int i=0;i<10;i++){
		AxesRaw_t AxesRawData;
        status_t response = H3LIS331DL_getAccAxesRaw(&AxesRawData);
        if(MEMS_SUCCESS == response){
			printf("\n");
			printf("AxesRawData.AXIS_X = %d\n",AxesRawData.AXIS_X);
			printf("AxesRawData.AXIS_Y = %d\n",AxesRawData.AXIS_Y);
			printf("AxesRawData.AXIS_Z = %d\n",AxesRawData.AXIS_Z);
			printf("\n");
        }else{
			printf("Read data failed!\n");
			getchar();
			return 0;
		}
#ifndef OS_UNIX
        Sleep(10);
#else
        usleep(10*1000);
#endif
    }

    //关闭设备
    USB_CloseDevice(DevHandle[0]);
	return 0;
}

