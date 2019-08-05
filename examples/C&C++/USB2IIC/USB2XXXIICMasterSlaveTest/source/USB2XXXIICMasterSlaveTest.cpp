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
#define SLAVE_WRITE_TEST    1
#define SLAVE_READ_TEST    1

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int MasterIICIndex = 0;
    int SlaveIICIndex = 1;
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
        getchar();
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
    //初始化配置I2C主机
    IIC_CONFIG IIC_Config;
    IIC_Config.EnablePu = 1;            //使能内部上拉电阻
    IIC_Config.AddrBits = 7;            //7bit地址模式
    IIC_Config.ClockSpeedHz = 100000;     //时钟频率400KHz
    IIC_Config.Master = 1;              //主机模式
    IIC_Config.OwnAddr = 0x51;          //从机地址
    ret = IIC_Init(DevHandle[0],MasterIICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config Master IIC Failed!\n");
        getchar();
        return 0;
    }else{
        printf("Config Master IIC Success!\n");
    }

    //初始化配置I2C
    IIC_Config.EnablePu = 1;            //使能内部上拉电阻
    IIC_Config.AddrBits = 7;            //7bit地址模式
    IIC_Config.ClockSpeedHz = 100000;     //时钟频率400KHz
    IIC_Config.Master = 0;              //从机模式
    IIC_Config.OwnAddr = 0x71;          //从机地址
    ret = IIC_Init(DevHandle[0],SlaveIICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config Slave IIC Failed!\n");
        getchar();
        return 0;
    }else{
        printf("Config Slave IIC Success!\n");
    }

#if AUTO_GET_SLAVE_ADDR
    //获取总线上能应答的从机地址
    short SlaveAddrs[128];  //地址存储缓冲区
    int SlaveAddrNum = 0;   //返回应答的地址个数
    SlaveAddrNum = IIC_GetSlaveAddr(DevHandle[0],MasterIICIndex,SlaveAddrs);
    if(SlaveAddrNum >= 0){
        printf("Get %d slave address!\n",SlaveAddrNum);
        for(int i=0;i<SlaveAddrNum;i++){
            printf("%02X ",SlaveAddrs[i]);
        }
        printf("\n");
    }else{
        printf("Get slave address error!\n");
        getchar();
        return 0;
    }
#endif
    //IIC 从机模式写数据
    unsigned char SlaveWriteBuffer[256];
    for(int i=0;i<sizeof(SlaveWriteBuffer);i++){
        SlaveWriteBuffer[i] = i;
    }
    ret = IIC_SlaveWriteBytes(DevHandle[0],SlaveIICIndex,SlaveWriteBuffer,sizeof(SlaveWriteBuffer),100);
    if(ret < 0){
        printf("Slave write data error!\n");
        printf("Error Code:%d\n",ret);
        getchar();
        return 0;
    }else if(ret == 0){
        printf("Slave write data success!\n");
    }else if(ret > 0){
        printf("Slave write data success! have %d byte data remain\n",ret);
    }
    //主机模式读数据
    unsigned char ReadBuffer[256];
    ret = IIC_ReadBytes(DevHandle[0],MasterIICIndex,0x71,ReadBuffer,sizeof(ReadBuffer),1000);
    if(ret != IIC_SUCCESS){
        printf("Master Read IIC failed!\n");
        printf("Error Code:%d\n",ret);
        getchar();
        return 0;
    }else{
        printf("Master Read IIC success!\n");
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
            if(((i+1)%16)==0){
                printf("\n");
            }
        }
        printf("\n");
    }
    //主机模式写数据
    unsigned char WriteBuffer[8]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
    ret = IIC_WriteBytes(DevHandle[0],MasterIICIndex,0x71,WriteBuffer,sizeof(WriteBuffer),1000);
    if(ret != IIC_SUCCESS){
        printf("Master Write IIC failed!\n");
        printf("Error Code:%d\n",ret);
        getchar();
        return 0;
    }else{
        printf("Master Write IIC success!\n");
    }
    //IIC 从机模式读数据
    unsigned char SlaveReadBuffer[256];
    ret = IIC_SlaveReadBytes(DevHandle[0],SlaveIICIndex,SlaveReadBuffer,10000);
    if(ret < 0){
        printf("Slave read data error!\n");
        printf("Error Code:%d\n",ret);
        getchar();
        return 0;
    }else if(ret == 0){
        printf("Slave read data IIC_SUCCESS! but no data\n");
    }else if(ret > 0){
        printf("Slave read data IIC_SUCCESS! have %d byte data have read\n",ret);
        for(int i=0;i<ret;i++){
            printf("%02X ",SlaveReadBuffer[i]);
            if(((i+1)%16)==0){
                printf("\n");
            }
        }
        printf("\n");
    }
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    printf("Test end\n");
    getchar();
	return 0;
}

