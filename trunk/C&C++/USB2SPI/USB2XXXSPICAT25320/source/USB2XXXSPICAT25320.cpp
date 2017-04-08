  /*
  ******************************************************************************
  * @file     : USB2XXXSPICAT25320.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI CAT25320 test demo
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
#include "usb2spi.h"

#define DATA_SIZE   256    //总的字节数

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    unsigned char WriteBuffer[8]={0};
    unsigned char ReadDataBuffer[8];
    int DevHandle[10];
    int SPIIndex = SPI1_CS0;
    bool state;
    int ret,k=0;
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
    //配置SPI总线相关参数
    SPI_CONFIG SPIConfig;
    SPIConfig.Mode = SPI_MODE_SOFT_HDX;//该芯片支持的时钟频率较低，建议使用软件模式
    SPIConfig.ClockSpeedHz = 100000;
    SPIConfig.CPHA = 0;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = SPI_MSB;
    SPIConfig.Master = SPI_MASTER;
    SPIConfig.SelPolarity = SPI_SEL_LOW;
    ret = SPI_Init(DevHandle[0],SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //写数据到芯片
    printf("Start write data to chip...\n");
    //循环将数据写入芯片
    for(int i=0,addr=0;i<DATA_SIZE;addr++){
        //发送WREN命令
        WriteBuffer[0]=0x06;
        ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteBuffer,1);
        if(ret != SPI_SUCCESS){
            printf("Write byte error!\n");
            return -1;
        }
        k = 0;
        //写数据
        WriteBuffer[k++] = 0x02;//WRITE命令
        WriteBuffer[k++] = addr>>8;//地址高8位
        WriteBuffer[k++] = addr&0xFF;//地址低8位
        WriteBuffer[k++] = i&0xFF;//即将写入该地址的数据
        ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteBuffer,4);
        if(ret != SPI_SUCCESS){
            printf("Write byte error!\n");
            return -1;
        }
        //读取状态
        k = 0;
        //读取状态,一直等到RDY位为0为止
        do{
            WriteBuffer[k++] = 0x05;//RDSR命令
            ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteBuffer,1,ReadDataBuffer,1,0);
            if(ret != SPI_SUCCESS){
                printf("WriteRead byte error!\n");
                return -1;
            }
        }while((ReadDataBuffer[0]&0x01)!=0x00);
        //显示数据
        printf("%02X ",i&0xFF);
        i++;
        if((i%16)==0){
            printf("\n");
        }
    }
    //循环读数据
    printf("Start read data from chip...\n");
    for(int i=0,addr=0;i<DATA_SIZE;addr++){
        k = 0;
        //合成READ数据
        WriteBuffer[k++] = 0x03;//READ命令
        WriteBuffer[k++] = addr>>8;//地址高8位
        WriteBuffer[k++] = addr&0xFF;//地址低8位
        ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteBuffer,3,ReadDataBuffer,1,0);
        if(ret != SPI_SUCCESS){
            printf("WriteRead byte error!\n");
            return -1;
        }else{
            //打印显示数据
            printf("%02X ",ReadDataBuffer[0]);
            i++;
            if((i%16)==0){
                printf("\n");
            }
        }
        Sleep(1);
    }
    return 0;
}


