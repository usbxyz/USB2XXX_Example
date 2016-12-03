  /*
  ******************************************************************************
  * @file     : USB2XXXSPITest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI test demo
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
#include "usb2spi.h"

int SPI_SlaveReadData(int DevIndex,int SPIIndex,unsigned char *pData,int DataNum)
{
    FILE *pFile=fopen("data.txt","a"); //获取文件的指针
    if(pFile == NULL){
        printf("Open file faild\n");
        return 0;
    }
    printf("Read Data Count: %d\n",DataNum);
    for(int i=0;i<DataNum;i++){
        fprintf(pFile,"%02X ",pData[i]);
        printf("%02X ",pData[i]);
        if(((i+1)%32)==0){
            fprintf(pFile,"\n");
            printf("\n");
        }
    }
    printf("\n");
    fflush(pFile);
    return 0;
}

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    SPI_CONFIG SPIConfig;
    int DevIndex = 0;
    int SPIIndex = 0;
    bool state;
    int ret;
    unsigned char WriteBuffer[64];
    unsigned char ReadBuffer[20480];
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
    /*
    //获取固件信息
    state = USB_GetDeviceInfo(DevIndex,&DevInfo);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }

    //配置SPI总线相关参数
    
    SPIConfig.Mode = 0;
    SPIConfig.ClockSpeed = 390625;
    SPIConfig.CPHA = 0;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = 0;
    SPIConfig.Master = 1;
    SPIConfig.SelPolarity = 0;
    ret = SPI_Init(DevIndex,SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //SPI发送数据
    
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    ret = SPI_WriteBytes(DevIndex,SPIIndex,WriteBuffer,sizeof(WriteBuffer));
    if(ret != SPI_SUCCESS){
        printf("SPI write data error!\n");
        return 0;
    }
    //SPI异步发送数据，调用该函数后会立即返回，但是SPI数据不一定发送完毕，但是在下一次发送数据之前会保证数据发送完毕
    for(int i=0;i<64;i++){
        ret = SPI_AsyncWriteBytes(DevIndex,SPIIndex,WriteBuffer,sizeof(WriteBuffer));
        if(ret != SPI_SUCCESS){
            printf("SPI async write data error!\n");
            return 0;
        }
    }
    //SPI接收数据
    
    ret = SPI_ReadBytes(DevIndex,SPIIndex,ReadBuffer,sizeof(ReadBuffer));
    if(ret != SPI_SUCCESS){
        printf("SPI read data error!\n");
        return 0;
    }else{
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
            if(((i+1)%16)==0){
                printf("\n");
            }
        }
        printf("\n");
    }
    //SPI先发送数据，再接收数据，整个过程片选信号一直有效
    int IntervalTime = 10;//发送和接收数据之间的时间间隔，单位为us
    ret = SPI_WriteReadBytes(DevIndex,SPIIndex,WriteBuffer,sizeof(WriteBuffer),ReadBuffer,sizeof(ReadBuffer),IntervalTime);
    if(ret != SPI_SUCCESS){
        printf("SPI write read data error!\n");
        return 0;
    }else{
        printf("Read Data:\n");
        for(int i=0;i<sizeof(ReadBuffer);i++){
            printf("%02X ",ReadBuffer[i]);
            if(((i+1)%16)==0){
                printf("\n");
            }
        }
        printf("\n");
    }
    */
    int DataTemp;
    printf("Please input SPI channel(0 or 1):");
    scanf("%d",&SPIIndex);
    printf("Please input CPHA(0 or 1):");
    scanf("%d",&DataTemp);
    SPIConfig.CPHA = DataTemp;
    printf("Please input CPOL(0 or 1):");
    scanf("%d",&DataTemp);
    SPIConfig.CPOL = DataTemp;
    printf("SPIConfig.CPHA = %d\n",SPIConfig.CPHA);
    printf("SPIConfig.CPOL = %d\n",SPIConfig.CPOL);
    //配置SPI总线相关参数(配置为从机模式)
    SPIConfig.Mode = 1;
    SPIConfig.ClockSpeedHz = 50000000;
    //SPIConfig.CPHA = 0;
    //SPIConfig.CPOL = 1;
    SPIConfig.LSBFirst = 0;
    SPIConfig.Master = 0;
    SPIConfig.SelPolarity = 0;
    ret = SPI_Init(DevIndex,SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    /*
    while(1){
        ret = SPI_SlaveWriteBytes(DevIndex,SPIIndex,WriteBuffer,16,5000);
        if(ret != SPI_SUCCESS){
            //printf("SPI slave write data error!\n");
            //return 0;
        }
        //getchar();
    }
    */
    printf("Start receive data to file,the file name is data.txt\n");
    printf("Press any key to exit the data reception!\n");
    SPI_SlaveContinueRead(DevIndex,SPIIndex,SPI_SlaveReadData);
    getchar();
    getchar();
    SPI_SlaveContinueReadStop(DevIndex,SPIIndex);
    printf("Test SPI_SUCCESS!\n");
    return 0;
}

