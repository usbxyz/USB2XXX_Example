  /*
  ******************************************************************************
  * @file     : USB2XXXI2CSnifferTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX I2C Sniffer test demo
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
#include "i2c_sniffer.h"

int WINAPI GetI2CData(int DeviceIndex,int Channel,I2C_DATA *pI2CData,int I2CDataNum)
{
	int i=0;
	for(i=0;i<I2CDataNum;i++){
		if(pI2CData[i].DataType == START){
			printf("S");
		}else if(pI2CData[i].DataType == ADDR){
			printf("<%02X>",pI2CData[i].Addr);
			if(pI2CData[i].WriteRead&0x01){
				printf("<R>");
			}else{
				printf("<W>");
			}
		}else if(pI2CData[i].DataType == DATA){
			printf("%02X ",pI2CData[i].Data);
		}else if(pI2CData[i].DataType == STOP){
			printf("P\n");
		}
	}
    return 0;
}

int main(int argc, const char* argv[])
{
	DEVICE_INFO DevInfo;
	int DevHandles[20];
    bool state;
    int ret;
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
    char FunctionStr[256]={0};
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandles[0],&DevInfo,FunctionStr);
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
	//启动I2C Sniffer
	ret = I2C_SnifferStart(DevHandles[0],0,5000000,GetI2CData);
    if(ret != SUCCESS){
        printf("Start i2c sniffer faild!\r\n");
        return 0;
    }
	getchar();
	getchar();
	I2C_SnifferStop(DevHandles[0]);
	USB_CloseDevice(DevHandles[0]);
}