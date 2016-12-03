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

#define SNIFFER_WRITE_TEST  0
#define SNIFFER_READ_TEST   1

#if SNIFFER_READ_TEST
static int ErrorData = 0;
static long long AllDataNum = 0;
static unsigned char MaxData=0,MinData=255;
//此处为接收数据回调函数，在这里处理接收到的数据
void SnifferGetData(int DeviceIndex,unsigned char *pData,int DataNum)
{
    AllDataNum += DataNum;
    for(int i=0;i<DataNum;i++){
        if(pData[i]>MaxData){
            MaxData = pData[i];
        }
        if(pData[i]<MinData){
            MinData = pData[i];
        }
        if((pData[i] > 155)||(pData[i] < 128)){
            ErrorData++;
        }
    }
    //printf("获取到%d字节数据\n",DataNum);
    //printf("Data = %f\n",(pData[0]*2.0)/255.0/*(((pData[0]*2.0)/255.0)-1)*5*/);
}
#endif

int main(int argc, const char* argv[])
{
	DEVICE_INFO DevInfo;
	int DevIndex = 0;
    bool state;
    int ret;
    //可根据实际要测试的次数修改
    for(int k=0;k<50;k++){
#if SNIFFER_READ_TEST
        ErrorData = 0;
        AllDataNum = 0;
        MaxData=0;
        MinData=255;
#endif
        //扫描查找设备
        ret = USB_ScanDevice(true);
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
#if SNIFFER_READ_TEST
	    //配置sniffer
	    ret = SNIFFER_Init(DevIndex,0,30000000);
	    if(ret != SNIFFER_SUCCESS){
		    printf("Init sniffer error!\n");
		    return 0;
	    }
	    //启动sniffer并注册回调函数
	    ret = SNIFFER_StartRead(DevIndex,SnifferGetData);
	    if(ret != SNIFFER_SUCCESS){
		    printf("Start sniffer error!\n");
		    return 0;
	    }else{
		    printf("Start sniffer SNIFFER_SUCCESS!\n");
	    }
        //接收10秒钟的数据
#ifndef OS_UNIX
        Sleep(10000);
#else
        usleep(10000*1000);
#endif
        printf("ErrorData = %d\n",ErrorData);
        printf("MaxData = %f\n",MaxData*2.0/255.0);
        printf("MinData = %f\n",MinData*2.0/255.0);
        printf("Error = %f%%\n",(ErrorData*100.0)/AllDataNum);
        //停止sniffer
        ret = SNIFFER_StopRead(DevIndex);
        if(ret != SNIFFER_SUCCESS){
            printf("Stop sniffer error!\n");
            return 0;
        }else{
            printf("Stop sniffer SNIFFER_SUCCESS!\n");
        }
#endif
#if SNIFFER_WRITE_TEST
        unsigned char WriteBuffer[10240];
        for(int i=0;i<sizeof(WriteBuffer);i++){
            WriteBuffer[i] = i;
        }
        //配置sniffer
        ret = SNIFFER_Init(DevIndex,1,30000000);
        if(ret != SNIFFER_SUCCESS){
            printf("Init sniffer error!\n");
            return 0;
        }
        //并口写数据
        ret = SNIFFER_WriteData(DevIndex,WriteBuffer,sizeof(WriteBuffer));
	    if(ret != SNIFFER_SUCCESS){
		    printf("Sniffer write error!\n");
		    return 0;
	    }else{
            printf("Sniffer write SNIFFER_SUCCESS!\n");
        }
        //并口连续写数据，并口将缓冲区中的数据循环发送
        ret = SNIFFER_ContinueWriteData(DevIndex,WriteBuffer,sizeof(WriteBuffer));
	    if(ret != SNIFFER_SUCCESS){
		    printf("Sniffer continue write error!\n");
		    return 0;
	    }else{
            printf("Sniffer continue write SNIFFER_SUCCESS!\n");
        }
        //写5秒钟的数据
#ifndef OS_UNIX
        Sleep(5000);
#else
        usleep(5000*1000);
#endif
        //停止循环发送数据
        ret = SNIFFER_StopContinueWrite(DevIndex);
        if(ret != SNIFFER_SUCCESS){
            printf("Stop continue write error!\n");
            return 0;
        }else{
            printf("Stop continue write SNIFFER_SUCCESS!\n");
        }
#endif
        //关闭设备
        USB_CloseDevice(DevIndex);
        printf("Test SNIFFER_SUCCESS %d\n\n",k);
    }
    getchar();
    return 0;
}

