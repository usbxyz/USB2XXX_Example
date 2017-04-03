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
#define AUTO_GET_SLAVE_ADDR     1
#define SLAVE_WRITE_TEST    0
#define SLAVE_READ_TEST    0

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevIndex = 0;
    int IICIndex = 0;
    bool state;
    int ret;
    unsigned int PinValue;
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
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //获取固件信息
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,FunctionStr);
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
#if SLAVE_WRITE_TEST||SLAVE_READ_TEST
    IIC_Config.Master = 0;              //从机模式
    IIC_Config.OwnAddr = 0x71;          //从机地址
#else
    IIC_Config.Master = 1;              //主机模式
#endif
    ret = IIC_Init(DevIndex,IICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config IIC failed!\n");
        return 0;
    }else{
        printf("Config IIC IIC_SUCCESS!\n");
    }
#if AUTO_GET_SLAVE_ADDR
    //获取总线上能应答的从机地址
    short SlaveAddrs[128];  //地址存储缓冲区
    int SlaveAddrNum = 0;   //返回应答的地址个数
    SlaveAddrNum = IIC_GetSlaveAddr(DevIndex,IICIndex,SlaveAddrs);
    if(SlaveAddrNum >= 0){
        printf("Get %d slave address!\n",SlaveAddrNum);
        for(int i=0;i<SlaveAddrNum;i++){
            printf("%02X ",SlaveAddrs[i]);
        }
        printf("\n");
    }else{
        printf("Get slave address error!\n");
        return 0;
    }
#endif
#if SLAVE_WRITE_TEST
    //IIC 从机模式写数据
    unsigned char SlaveWriteBuffer[256];
    for(int i=0;i<sizeof(SlaveWriteBuffer);i++){
        SlaveWriteBuffer[i] = i;
    }
    for(int i=0;i<(sizeof(SlaveWriteBuffer)/8);i++){
        ret = IIC_SlaveWriteBytes(DevIndex,IICIndex,&SlaveWriteBuffer[i*8],8,10000);
        if(ret < 0){
            printf("Slave write data error!\n");
            break;
        }else if(ret == 0){
            printf("Slave write data IIC_SUCCESS!\n");
        }else if(ret > 0){
            printf("Slave write data IIC_SUCCESS! have %d byte data remain\n",ret);
        }
    }
    printf("Slave write end!\n");
    return 0;
#endif
#if SLAVE_READ_TEST
    //IIC 从机模式读数据
    unsigned char SlaveReadBuffer[256];
    for(int i=0;i<16;i++){
        ret = IIC_SlaveReadBytes(DevIndex,IICIndex,SlaveReadBuffer,10000);
        if(ret < 0){
            printf("Slave read data error!\n");
            break;
        }else if(ret == 0){
            printf("Slave read data IIC_SUCCESS! but no data\n");
        }else if(ret > 0){
            printf("Slave read data IIC_SUCCESS! have %d byte data have read\n",ret);
            for(int i=0;i<ret;i++){
                printf("%02X ",SlaveReadBuffer[i]);
            }
            printf("\n");
        }
    }
    printf("Slave read end!\n");
    return 0;
#endif
    //IIC 写数据
    unsigned char WriteBuffer[8]={0x10,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    ret = IIC_WriteBytes(DevIndex,IICIndex,0x50,WriteBuffer,sizeof(WriteBuffer),1000);
    if(ret != IIC_SUCCESS){
        printf("Write IIC failed!\n");
        printf("Error Code:%d\n",ret);
        return 0;
    }else{
        printf("Write IIC IIC_SUCCESS!\n");
    }
    //延时
#ifndef OS_UNIX
    Sleep(10);
#else
    usleep(10*1000);
#endif
    //IIC 读数据
    unsigned char ReadBuffer[8];
    ret = IIC_ReadBytes(DevIndex,IICIndex,0x50,ReadBuffer,sizeof(ReadBuffer),1000);
    if(ret != IIC_SUCCESS){
        printf("Read IIC failed!\n");
        printf("Error Code:%d\n",ret);
        return 0;
    }else{
        printf("Read IIC IIC_SUCCESS!\n");
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
        }
        printf("\n");
    }
    //延时
#ifndef OS_UNIX
    Sleep(10);
#else
    usleep(10*1000);
#endif
    //IIC 写读数据。也就是先发送数据，然后再次产生START信号，再读数据
    //注意：对于每个I2C设备，必须按照设备要求正确读写，否则可能会导致设备工作不正常，从而导致无法读写数据
    WriteBuffer[0] = 0x08;
    ret = IIC_WriteReadBytes(DevIndex,IICIndex,0x50,WriteBuffer,1,ReadBuffer,sizeof(ReadBuffer),1000);
    if(ret != IIC_SUCCESS){
        printf("WriteRead IIC failed!\n");
        printf("Error Code:%d\n",ret);
        return 0;
    }else{
        printf("WriteRead IIC IIC_SUCCESS!\n");
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
        }
        printf("\n");
    }
    //关闭设备
    USB_CloseDevice(DevIndex);
	return 0;
}

