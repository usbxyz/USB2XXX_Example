  /*
  ******************************************************************************
  * @file     : USB2XXXUARTTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX UART test demo
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
#include "usb2uart.h"


int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    UART_CONFIG UARTConfig;
    int DevHandle[10];
    int UARTIndex = 0;
    bool state;
    int ret;
    const int PackSize = 20*1024;
    const int PackNum = 100;
    unsigned char WriteBuffer[PackSize]={0};
    unsigned char ReadBuffer[PackSize];
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
    char FuncStr[256]={0};
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

    //配置UART总线相关参数
    UARTConfig.BaudRate = 6000000;
    UARTConfig.Parity = UART_PARITY_NO;
    UARTConfig.StopBits = UART_STOP_BITS_1;
    UARTConfig.WordLength = UART_WORD_LENGTH_8BIT;
    ret = UART_Init(DevHandle[0],1,&UARTConfig);
    ret = UART_Init(DevHandle[0],UARTIndex,&UARTConfig);
    if(ret != UART_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    printf("开始测试...\n");
    printf("传输方式：同步模式发送数据\n");
    printf("UART波特率：%d \n",UARTConfig.BaudRate);
    //获取起始时间,主要用于测试发送速度
    LARGE_INTEGER litmp;
    LONGLONG StartTime,EndTime;
    double dfFreq;
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    for(int i=0;i<PackNum;i++){
        ret = UART_WriteBytes(DevHandle[0],UARTIndex,WriteBuffer,PackSize);
        if(ret != UART_SUCCESS){
            printf("UART write data error!\n");
            getchar();
            return 0;
        }
    }
	//获取结束时间并打印输出耗时和速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
	// Print the write data speed information
	printf("-----------------------www.usbxyz.com-----------------------\n");
	printf("发送数据字节数: %.3f MBytes\n",PackSize*PackNum/(1024*1024.0));
	printf("发送数据消耗时间: %f s\n",(EndTime-StartTime)/dfFreq);
	printf("发送数据速度: %.3f MByte/s\n",PackSize*PackNum/((EndTime-StartTime)/dfFreq)/(1024*1024));
	printf("-----------------------www.usbxyz.com-----------------------\n");

    printf("\n开始测试...\n");
    printf("传输方式：异步发送数据\n");
    printf("UART波特率：%d \n",UARTConfig.BaudRate);
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    for(int i=0;i<PackNum;i++){
        ret = UART_WriteBytesAsync(DevHandle[0],UARTIndex,WriteBuffer,PackSize);
        if(ret != UART_SUCCESS){
            printf("UART write data error!\n");
            getchar();
            return 0;
        }
    }
	//获取结束时间并打印输出耗时和速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
	// Print the write data speed information
	printf("-----------------------www.usbxyz.com-----------------------\n");
	printf("发送数据字节数: %.3f MBytes\n",PackSize*PackNum/(1024*1024.0));
	printf("发送数据消耗时间: %f s\n",(EndTime-StartTime)/dfFreq);
	printf("发送数据速度: %.3f MByte/s\n",PackSize*PackNum/((EndTime-StartTime)/dfFreq)/(1024*1024));
	printf("-----------------------www.usbxyz.com-----------------------\n");
    getchar();
    return 0;
}

