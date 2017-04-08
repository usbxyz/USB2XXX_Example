  /*
  ******************************************************************************
  * @file     : USB2XXXIICLM75A.cpp
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
    char IICChannel[8]={};
    int DevHandle[10];
    int IICIndex = 0;
    bool state;
    unsigned char WriteBuffer[8];
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
    IICIndex = 0;
    for(int i=0;i<8;i++){
        //初始化配置I2C
        IIC_CONFIG IIC_Config;
        IIC_Config.EnablePu = 1;            //使能内部上拉电阻
        IIC_Config.AddrBits = 7;            //7bit地址模式
        IIC_Config.ClockSpeedHz = 400000;   //时钟频率400KHz
        IIC_Config.Master = 1;              //主机模式
        ret = IIC_Init(DevHandle[0],i,&IIC_Config);
        if(ret != IIC_SUCCESS){
            printf("Config IIC failed!\n");
            return 0;
        }else{
            printf("Config IIC IIC_SUCCESS!\n");
        }
        //检测当前通道传感器是否在线
        //若传感器在线则配置传感器
        WriteBuffer[0] = 0x01;
        WriteBuffer[1] = 0x00;
        ret = IIC_WriteBytes(DevHandle[0],i,0x48,WriteBuffer,2,10);
        if(ret == IIC_SUCCESS){
            IICChannel[IICIndex++] = i;
        }
    }
    printf("检测到%d个传感器在线\n",IICIndex);
    int ErrorDataCnt = 0;
    int ErrorReadCnt[8]={0};
    int ReadDataCnt = 0;
    while(true)
    {
        system("cls");//清除之前显示信息
        double Temperature[8];
        for(int i=0;i<IICIndex;i++){
            //读取测量的数据
            unsigned char ReadBuffer[8];
            WriteBuffer[0] = 0x00;
            ret = IIC_WriteReadBytes(DevHandle[0],IICChannel[i],0x48,WriteBuffer,1,ReadBuffer,2,10);
            if (ret != IIC_SUCCESS){
                printf("Read IIC failed!  %d\n",ret);
                ErrorReadCnt[i]++;//读数据出错次数加一
            }else{
                short Temp = ((ReadBuffer[0] << 8) | ReadBuffer[1]);
                Temp >>= 5;
                Temperature[i] = Temp*0.125;
                printf("Temperature[I2C-%d]: %.1f\n",IICChannel[i],Temperature[i]);//显示温度值
            }
        }
        //计算读取到的温度值最大值和最小值之差
        double MaxValue = Temperature[0],MinValue = Temperature[0];
        for(int i=0;i<IICIndex;i++){
            if(Temperature[i] > MaxValue){
                MaxValue = Temperature[i];
            }
            if(Temperature[i] < MinValue){
                MinValue = Temperature[i];
            }
        }
        //最大最小值相差达到1度则判断数据有误
        if((MaxValue-MinValue)>1){
            ErrorDataCnt++;
        }
        //显示读数据出错次数
        for(int i=0;i<IICIndex;i++){
            printf("ErrorReadCnt[I2C-%d] = %d\n",IICChannel[i],ErrorReadCnt[i]);
        }
        //显示成功读数据但是数据有误的次数
        printf("ErrorDataCnt = %d\n",ErrorDataCnt);
        //读温度次数加一
        ReadDataCnt++;
        printf("ReadDataCnt = %d\n",ReadDataCnt);
        //延时
#ifndef OS_UNIX
        Sleep(50);
#else
        usleep(100*1000);
#endif
    }
    return 0;
}