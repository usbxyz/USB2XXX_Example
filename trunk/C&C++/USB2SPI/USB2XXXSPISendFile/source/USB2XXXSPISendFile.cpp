  /*
  ******************************************************************************
  * @file     : USB2XXXSPISendFile.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI slave test demo
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

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    int SPIIndex = 0;
    bool state;
    int ret,i;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
	}else if (ret <= 1) {
		printf("Must connect two devices!\n");
		return 0;
	}
    //打开设备
    state = USB_OpenDevice(DevHandle[1]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //获取固件信息
    char FunctionStr[512]={0};
    state = DEV_GetDeviceInfo(DevHandle[1],&DevInfo,FunctionStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
	    printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%s\n",FunctionStr);
        printf("Firmware Serial Number:%08X%08X%08X\n",DevInfo.SerialNumber[0],DevInfo.SerialNumber[1],DevInfo.SerialNumber[2]);
    }
    //配置SPI总线相关参数
    SPI_CONFIG SPIConfig;
    SPIConfig.Mode = 1;
    SPIConfig.ClockSpeedHz = 25000000>>2;
    SPIConfig.CPHA = 0;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = 0;
    SPIConfig.Master = 1;
    SPIConfig.SelPolarity = 0;
    ret = SPI_Init(DevHandle[1],SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //输入文件名
    printf("Please input file name:");
    char FileName[512]={0};
    gets(FileName);
    //打开文件
    FILE *pReadFile=fopen(FileName,"rb"); //获取文件的指针
    if(pReadFile == NULL){
	    printf("Open file error!\n");
	    return -2;
    }
    unsigned char *pInputBuf;  //定义文件指针
    fseek(pReadFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    int InputFileLen=ftell(pReadFile); //获取文件长度
	pInputBuf = new unsigned char[InputFileLen]; //定义数组长度
	rewind(pReadFile); //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来会出错
	fread(pInputBuf,1,InputFileLen,pReadFile); //读文件
	fclose(pReadFile); // 关闭文件
    printf("FileSize = %d Byte\n",InputFileLen);
    //获取起始时间,主要用于测试发送速度
    LARGE_INTEGER litmp;
    LONGLONG StartTime,EndTime;
    double dfFreq;
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    //循环将数据通过SPI口发送出去
    int PackSize = 20*1024;//每次发送10K字节数据
    int PackIndex = 0;
    for(PackIndex = 0;PackIndex<(InputFileLen/PackSize);PackIndex++){
        ret = SPI_WriteBytes(DevHandle[1],SPIIndex,&pInputBuf[PackIndex*PackSize],PackSize);
        if(ret != SPI_SUCCESS){
            printf("SPI write data error!\n");
            return 0;
        }
        Sleep(1);
    }
    if(InputFileLen%PackSize){
        ret = SPI_WriteBytes(DevHandle[1],SPIIndex,&pInputBuf[PackIndex*PackSize],InputFileLen%PackSize);
        if(ret != SPI_SUCCESS){
            printf("SPI write data error!\n");
            return 0;
        }
    }
	//获取结束时间并打印输出耗时和速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
	// Print the write data speed information
	printf("-----------------------www.usbxyz.com-----------------------\n");
	printf("发送数据字节数: %.3f MBytes\n",InputFileLen/(1024*1024.0));
	printf("发送数据消耗时间: %f s\n",(EndTime-StartTime)/dfFreq);
	printf("发送数据速度: %.3f MByte/s\n",InputFileLen/((EndTime-StartTime)/dfFreq)/(1024*1024));
	printf("-----------------------www.usbxyz.com-----------------------\n");
    //关闭设备
    USB_CloseDevice(DevHandle[1]);
    printf("SPI send file test success!\n");
    return 0;
}