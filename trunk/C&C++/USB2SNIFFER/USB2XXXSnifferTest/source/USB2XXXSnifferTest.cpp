  /*
  ******************************************************************************
  * @file     : USB2XXXSnifferTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX Sniffer test demo
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
#include "usb2sniffer.h"

#define SNIFFER_WRITE_TEST  1
#define SNIFFER_READ_TEST   0

#if SNIFFER_READ_TEST
//此处为接收数据回调函数，在这里处理接收到的数据
int WINAPI SnifferGetData(int DeviceHandle,unsigned char *pData,int DataNum)
{
    printf("Get %d Byte Data\n",DataNum);
	return 0;
}
#endif

int main(int argc, const char* argv[])
{
	DEVICE_INFO DevInfo;
	int DevHandles[10];
    bool state;
    int ret;
#if SNIFFER_READ_TEST
    unsigned char ReadDataBuffer[8];
#endif
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
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandles[0],&DevInfo,NULL);
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
#if SNIFFER_READ_TEST
	//配置sniffer
	ret = SNIFFER_Init(DevHandles[0],0,5000000,SNIFFER_SAMPLE_MODE_8CH);
	if(ret != SNIFFER_SUCCESS){
		printf("Init sniffer error!\n");
		return 0;
	}
	getchar();
	//单次读取数据
	ret = SNIFFER_ReadData(DevHandles[0],ReadDataBuffer,sizeof(ReadDataBuffer));
	if(ret != SNIFFER_SUCCESS){
		printf("Read sniffer error!\n");
		return 0;
	}else{
		printf("Read sniffer success!,Data is:\n");
		for(int i=0;i<sizeof(ReadDataBuffer);i++){
			printf("%02X ",ReadDataBuffer[i]);
			if(((i+1)%16)==0){
				printf("\n");
			}
		}
		printf("\n");
	}
	getchar();
	//启动sniffer并注册回调函数
	ret = SNIFFER_StartRead(DevHandles[0],SnifferGetData);
	if(ret != SNIFFER_SUCCESS){
		printf("Start sniffer error!\n");
		return 0;
	}else{
		printf("Start sniffer SNIFFER_SUCCESS!\n");
	}
    //接收10秒钟的数据
#ifndef OS_UNIX
    Sleep(3000);
#else
    usleep(3000*1000);
#endif
    //停止sniffer
    ret = SNIFFER_StopRead(DevHandles[0]);
    if(ret != SNIFFER_SUCCESS){
        printf("Stop sniffer error!\n");
        return 0;
    }else{
        printf("Stop sniffer SNIFFER_SUCCESS!\n");
    }
#endif
#if SNIFFER_WRITE_TEST
    unsigned char WriteBuffer[8];
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    //配置sniffer
    ret = SNIFFER_Init(DevHandles[0],1,5000000,SNIFFER_SAMPLE_MODE_8CH);
    if(ret != SNIFFER_SUCCESS){
        printf("Init sniffer error!\n");
        return 0;
    }
	getchar();
    //并口写数据
    ret = SNIFFER_WriteData(DevHandles[0],WriteBuffer,sizeof(WriteBuffer));
	if(ret != SNIFFER_SUCCESS){
		printf("Sniffer write error! %d\n",ret);
		return 0;
	}else{
        printf("Sniffer write SNIFFER_SUCCESS!\n");
    }
    //并口写数据
    ret = SNIFFER_WriteData(DevHandles[0],WriteBuffer,sizeof(WriteBuffer));
	if(ret != SNIFFER_SUCCESS){
		printf("Sniffer write error! %d\n",ret);
		return 0;
	}else{
        printf("Sniffer write SNIFFER_SUCCESS!\n");
    }
	getchar();
    //并口连续写数据，并口将缓冲区中的数据循环发送
    ret = SNIFFER_ContinueWriteData(DevHandles[0],WriteBuffer,sizeof(WriteBuffer));
	if(ret != SNIFFER_SUCCESS){
		printf("Sniffer continue write error!\n");
		return 0;
	}else{
        printf("Sniffer continue write SNIFFER_SUCCESS!\n");
    }
	for(int i=0;i<sizeof(WriteBuffer);i++){
		WriteBuffer[i] = (i<<4)|i;
    }
    ret = SNIFFER_ChangeWriteData(DevHandles[0],WriteBuffer,sizeof(WriteBuffer));
	if(ret != SNIFFER_SUCCESS){
		printf("Sniffer Change write error!\n");
		return 0;
	}else{
        printf("Sniffer Change write SNIFFER_SUCCESS!\n");
    }
    //写5秒钟的数据
#ifndef OS_UNIX
    Sleep(5000);
#else
    usleep(5000*1000);
#endif
    //停止循环发送数据
    ret = SNIFFER_StopContinueWrite(DevHandles[0]);
    if(ret != SNIFFER_SUCCESS){
        printf("Stop continue write error!\n");
        return 0;
    }else{
        printf("Stop continue write SNIFFER_SUCCESS!\n");
    }
#endif
    //关闭设备
    USB_CloseDevice(DevHandles[0]);
    printf("Sniffer Test Success\n");
    getchar();
    return 0;
}

