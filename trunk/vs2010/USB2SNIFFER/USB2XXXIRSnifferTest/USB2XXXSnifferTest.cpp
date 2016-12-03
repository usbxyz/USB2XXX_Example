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
#include "ir_sniffer.h"


int __stdcall IRGetDataHandle(int DeviceIndex,int Channel,PIRTimeSeries pIRData,int IRDataNum)
{
    int DataNum = 0;
    for(int i=0;i<IRDataNum;i++){
        /*if(pIRData[i].DataTime < 10000000){
            DataNum++;
        }else{
            printf("DataNum = %d\n",DataNum);
            DataNum = 0;
        }*/
        printf("IRData[%d].DataType = %d,IRData[%d].DataTime = %d\n",i,pIRData[i].DataType,i,pIRData[i].DataTime);
    }
    return 0;
}

int main(int argc, const char* argv[])
{
	DEVICE_INFO DevInfo;
	int DevIndex = 0;
    bool state;
    int ret;
    //可根据实际要测试的次数修改
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
    //
    ret = IR_SnifferStart(DevIndex,0x00,10000000,IRGetDataHandle);
    printf("ret = %d\n",ret);
    getchar();
    IR_SnifferStop(DevIndex);
    //关闭设备
    USB_CloseDevice(DevIndex);
    return 0;
}

