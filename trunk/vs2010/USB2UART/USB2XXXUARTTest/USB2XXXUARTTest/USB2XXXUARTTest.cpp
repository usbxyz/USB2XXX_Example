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
    int DevIndex = 0;
    int UARTIndex = 0;
    bool state;
    int ret;
    unsigned char WriteBuffer[256];
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
    //获取固件信息
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,NULL);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
	    printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%08X\n",DevInfo.Functions);
    }

    //配置UART总线相关参数
    UARTConfig.BaudRate = 115200;
    UARTConfig.Parity = UART_PARITY_NO;
    UARTConfig.StopBits = UART_STOP_BITS_1;
    UARTConfig.WordLength = UART_WORD_LENGTH_8BIT;
    ret = UART_Init(DevIndex,1,&UARTConfig);
    ret = UART_Init(DevIndex,UARTIndex,&UARTConfig);
    if(ret != UART_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    //SPI发送数据
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    ret = UART_WriteBytes(DevIndex,UARTIndex,WriteBuffer,sizeof(WriteBuffer));
    if(ret != UART_SUCCESS){
        printf("UART write data error!\n");
        return 0;
    }
    ret = UART_WriteBytes(DevIndex,UARTIndex,WriteBuffer,sizeof(WriteBuffer));
    if(ret != UART_SUCCESS){
        printf("UART write data error!\n");
        return 0;
    }
    //SPI异步发送数据，调用该函数后会立即返回，但是SPI数据不一定发送完毕，但是在下一次发送数据之前会保证数据发送完毕
    for(int i=0;i<64;i++){
        ret = UART_WriteBytesAsync(DevIndex,UARTIndex,WriteBuffer,sizeof(WriteBuffer));
        if(ret != UART_SUCCESS){
            printf("UART async write data error!\n");
            return 0;
        }
    }
    //SPI接收数据
    printf("Read Bytes:\n");
    int ReadDataNum = 0;
    while(1){
        ret = UART_ReadBytes(DevIndex,UARTIndex,ReadBuffer,5000);
        if(ret > 0){
            ReadDataNum += ret;
            for(int i=0;i<ret;i++){
                printf("%02X ",ReadBuffer[i]);
                if(((i+1)%16)==0){
                    printf("\n");
                }
            }
            //printf("ret = %d\n",ret);
            printf("\nReadDataNum = %d\n",ReadDataNum);
        }else if(ret < 0){
            printf("ret = %d\n",ret);
        }
        //Sleep(500);
    }
    getchar();
    printf("Test UART_SUCCESS!\n");
    return 0;
}

