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


int __stdcall IRGetDataHandle(int DeviceIndex,int ChannelIndex,IR_RESULTS *pIRResults)
{
    printf("pIRResults->decode_type = %d\n",pIRResults->decode_type);
    printf("pIRResults->value = 0x%08X\n",pIRResults->value);
    printf("pIRResults->rawlen = %d\n",pIRResults->rawlen);
    printf("\n");
    return 0;
}

int main(int argc, const char* argv[])
{
	DEVICE_INFO DevInfo;
	int DevIndex = 0;
    bool state;
    int ret;
    int DevHandle[20];
    //可根据实际要测试的次数修改
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    printf("DevHandle[%d] = %08X\n",DevIndex,DevHandle[DevIndex]);
    //打开设备
    state = USB_OpenDevice(DevHandle[DevIndex]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandle[DevIndex],&DevInfo,NULL);
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
    ret = IR_SnifferStart(DevHandle[DevIndex],0x01,IRGetDataHandle);
    printf("ret = %d\n",ret);
    getchar();
    getchar();
    IR_SnifferStop(DevHandle[DevIndex]);
    //关闭设备
    USB_CloseDevice(DevHandle[DevIndex]);
    return 0;
}

