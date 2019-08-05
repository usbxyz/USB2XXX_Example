  /*
  ******************************************************************************
  * @file     : USB2XXXLINTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX LIN test demo
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
#include "usb2lin.h"

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int LINMasterIndex = 0;
    int LINSlaveIndex = 0;
    bool state;
    int ret;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }else if(ret < 2){
        printf("Must two device connected!\n");
        return 0;
	}
    //打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    state = USB_OpenDevice(DevHandle[1]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FunctionStr);
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
    //初始化配置LIN
    LIN_CONFIG LINConfig;
    LINConfig.BaudRate = 19200;
    LINConfig.BreakBits = LIN_BREAK_BITS_11;
    LINConfig.CheckMode = LIN_CHECK_MODE_EXT;
    LINConfig.MasterMode = LIN_MASTER;
    ret = LIN_Init(DevHandle[0],LINMasterIndex,&LINConfig);
    if(ret != LIN_SUCCESS){
        printf("Config LIN failed!\n");
        return 0;
    }else{
        printf("Config LIN Success!\n");
    }

    LINConfig.BaudRate = 19200;
    LINConfig.BreakBits = LIN_BREAK_BITS_11;
    LINConfig.CheckMode = LIN_CHECK_MODE_EXT;
    LINConfig.MasterMode = LIN_SLAVE;
    ret = LIN_Init(DevHandle[1],LINSlaveIndex,&LINConfig);
    if(ret != LIN_SUCCESS){
        printf("Config LIN failed!\n");
        return 0;
    }else{
        printf("Config LIN Success!\n");
    }
    //设置从机模式下所有ID都为从接收数据模式，这样就可以获取到主机发送过来的所有数据
    LIN_MSG LINSlaveData[64];
	for(int i=0;i<64;i++){
		LINSlaveData[i].DataLen = 9;//最大8Byte数据+1Byte和校验
		LINSlaveData[i].ID = i;		//ID值
	}
    ret = LIN_SlaveSetIDMode(DevHandle[1],LINSlaveIndex,LIN_SLAVE_READ,LINSlaveData,64);
    if(ret != LIN_SUCCESS){
        printf("Set LIN operation mode failed!\n");
        return 0;
    }else{
        printf("Set LIN operation mode success!\n");
    }
	
    //主机写数据
	for(int ID=0;ID<10;ID++){
		LIN_MSG msg;
		for(int i=0;i<8;i++){
			msg.Data[i] = ID+i;
		}
		msg.DataLen = 8;
		msg.ID = ID;
		ret = LIN_Write(DevHandle[0],LINMasterIndex,&msg,1);
		if(ret != LIN_SUCCESS){
			printf("LIN write data failed!\n");
			return 0;
		}else{
			printf("LIN write data success!\n");
		}
	}
    Sleep(10);
    //主机发送数据之后，可以读取从机接收到的数据
    LIN_MSG LINSlaveDataBuffer[1024];//为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
    ret = LIN_SlaveGetData(DevHandle[1],LINSlaveIndex,LINSlaveDataBuffer);
    if(ret < LIN_SUCCESS){
        printf("LIN slave read data error!\n");
        return 0;
    }else if(ret == 0){
        printf("LIN slave read no data!\n");
    }else{
		printf("LIN slave read data:\n");
        for(int i=0;i<ret;i++){
            printf("Data[%d]: ",i);
			printf("ID = 0x%02X ",LINSlaveDataBuffer[i].ID);
            printf("Data = ");
            for(int j=0;j<LINSlaveDataBuffer[i].DataLen;j++){
                printf("0x%02X ",LINSlaveDataBuffer[i].Data[j]);
            }
            printf("\n");
        }
    }
	//将前10个ID设置为从机发送数据模式
	for(int ID=0;ID<10;ID++){
		LINSlaveData[ID].DataLen = 6;//从机模式下该ID对应的数据长度
		LINSlaveData[ID].ID = ID;	 //ID值
		for(int i=0;i<LINSlaveData[ID].DataLen;i++){
			LINSlaveData[ID].Data[i] = ID+i;
		}
	}
    ret = LIN_SlaveSetIDMode(DevHandle[1],LINSlaveIndex,LIN_SLAVE_WRITE,LINSlaveData,10);
    if(ret != LIN_SUCCESS){
        printf("Set LIN operation mode failed!\n");
        return 0;
    }else{
        printf("Set LIN operation mode success!\n");
    }
    //主机读数据
	for(int ID=0;ID<10;ID++){
		LIN_MSG msg;
		msg.ID = ID;
		ret = LIN_Read(DevHandle[0],LINMasterIndex,&msg,1);
		if(ret < LIN_SUCCESS){
			printf("LIN read data failed!\n");
			return 0;
		}else{
			printf("Master LIN Read: ID = 0x%02X Data = ",ID);
			for(int i=0;i<msg.DataLen;i++){
				printf("0x%02X ",msg.Data[i]);
			}
			printf("\n");
		}
	}
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
	USB_CloseDevice(DevHandle[1]);
	return 0;
}

