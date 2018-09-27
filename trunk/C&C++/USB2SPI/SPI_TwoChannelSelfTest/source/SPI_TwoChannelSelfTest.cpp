     /*
  ******************************************************************************
  * @file     : SPI_TwoChannelSelfTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : SPI两个通道对接，一个配置为主，一个配置为从，实现数据收发
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
/*
硬件连接说明（同一个适配器的两个SPI通道对接）
P13(SPI1_CS0)	<-->	P4(SPI2_CS0)
P12(SPI1_SCK)	<-->	P5(SPI2_SCK)
P11(SPI1_MISO)	<-->	P6(SPI2_MISO)
P10(SPI1_MOSI)	<-->	P7(SPI2_MOSI)
*/

//主函数
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    SPI_CONFIG SPIConfig;
    int DevHandles[10];
    int SPIMasterIndex = SPI1_CS0;
	int SPISlaveIndex = SPI2_CS0;
    bool state;
    int ret;
    unsigned char WriteBuffer[1024];
    unsigned char ReadBuffer[1024];
    //扫描查找设备
    ret = USB_ScanDevice(DevHandles);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevHandles[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    char FunctionStr[512]={0};
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandles[0],&DevInfo,FunctionStr);
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
	//初始化SPI主机通道
    SPIConfig.Mode = SPI_MODE_HARD_HDX;
    SPIConfig.ClockSpeedHz = 25000000;
    SPIConfig.CPHA = 1;
    SPIConfig.CPOL = 1;
    SPIConfig.LSBFirst = SPI_MSB;
    SPIConfig.Master = SPI_MASTER;
    SPIConfig.SelPolarity = SPI_SEL_LOW;
    ret = SPI_Init(DevHandles[0],SPIMasterIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
	//初始化SPI从机通道
	SPIConfig.Master = SPI_SLAVE;
	ret = SPI_Init(DevHandles[0],SPISlaveIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
	//启动从机连续接收数据,回调函数设置为NULL，通过调用SPI_SlaveGetBytes函数来获取缓冲区中的数据
	ret = SPI_SlaveContinueRead(DevHandles[0],SPISlaveIndex,NULL);
    if(ret != SPI_SUCCESS){
        printf("Start continue read error!\n");
        return 0;
    }
	//主机发送数据，从机接收数据测试
	for(int TestNum = 0;TestNum<10;TestNum++){
		//主机发送数据
		for(int i=0;i<sizeof(WriteBuffer);i++){
			WriteBuffer[i] = rand();
		}
		ret = SPI_WriteBytes(DevHandles[0],SPIMasterIndex,WriteBuffer,sizeof(WriteBuffer));
		if(ret != SPI_SUCCESS){
			printf("Master write data error!\n");
			return 0;
		}
		//读取从机数据接收缓冲区中的数据
		ret = SPI_SlaveGetBytes(DevHandles[0],SPISlaveIndex,ReadBuffer,100);
		if(ret != sizeof(WriteBuffer)){
			printf("Master send data num not equal to slave read data num\n");
		}else{
			int ErrorDataNum = 0;
			for(int i=0;i<sizeof(WriteBuffer);i++){
				if(WriteBuffer[i] != ReadBuffer[i]){
					ErrorDataNum++;
				}
			}
			printf("Error data num[%d] = %d\n",TestNum,ErrorDataNum);
		}
	}
	//停止从机连续读数据模式
	ret = SPI_SlaveContinueWriteReadStop(DevHandles[0],SPISlaveIndex);
    if(ret != SPI_SUCCESS){
        printf("Slave stop continue read data error!\n");
        return 0;
    }
	//从机发送数据，主机读取数据测试
	for(int TestNum = 0;TestNum<10;TestNum++){
		//从机连续模式写数据
		for(int i=0;i<sizeof(WriteBuffer);i++){
			WriteBuffer[i] = rand();
		}
		//程序第二次调用该函数可以实现修改缓冲区中的数据，主机第一次读取数据，从机返回的第0和第1字节可能会出错（还是上一次的数据），主机再一次读后就会恢复正常
		ret = SPI_SlaveContinueWrite(DevHandles[0],SPISlaveIndex,WriteBuffer,sizeof(WriteBuffer));
		if(ret != SPI_SUCCESS){
			printf("Slave continue write data error!\n");
			return 0;
		}
		//主机第一次读取数据，第一次读取数据从机返回的第0，第1字节数据可能会是错的（还是上一次缓冲区中的数据）
		ret = SPI_ReadBytes(DevHandles[0],SPIMasterIndex,ReadBuffer,sizeof(WriteBuffer));
		if(ret != SPI_SUCCESS){
			printf("Master read data error\n");
		}else{
			int ErrorDataNum = 0;
			for(int i=0;i<sizeof(WriteBuffer);i++){
				if(WriteBuffer[i] != ReadBuffer[i]){
					ErrorDataNum++;
				}
			}
			printf("Error data num[%d][0] = %d\n",TestNum,ErrorDataNum);
		}
		//主机第二次读取数据，返回的是缓冲区中的数据，不会出错，后面再连续多次调用都不会出错
		ret = SPI_ReadBytes(DevHandles[0],SPIMasterIndex,ReadBuffer,sizeof(WriteBuffer));
		if(ret != SPI_SUCCESS){
			printf("Master read data error\n");
		}else{
			int ErrorDataNum = 0;
			for(int i=0;i<sizeof(WriteBuffer);i++){
				if(WriteBuffer[i] != ReadBuffer[i]){
					ErrorDataNum++;
				}
			}
			printf("Error data num[%d][1] = %d\n",TestNum,ErrorDataNum);
		}
	}
}
