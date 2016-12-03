/**
  ******************************************************************************
  * @file    usb_device.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   USB设置操作相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.usbxzy.com">http://www.usbxzy.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB_DEVICE_H_
#define __USB_DEVICE_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

typedef struct _DEVICE_INFO
{
    char    FirmwareName[32];   //固件名称字符串
    char    BuildDate[32];      //固件编译时间字符串
    int     HardwareVersion;    //硬件版本号
    int     FirmwareVersion;    //固件版本号
    int     SerialNumber[3];    //适配器序列号
    int     Functions;          //适配器当前具备的功能
}DEVICE_INFO,*PDEVICE_INFO;

#ifdef __cplusplus
extern "C"
{
#endif

    int  WINAPI USB_ScanDevice(int *pDevNum);
    bool WINAPI USB_OpenDevice(int DevIndex);
    bool WINAPI USB_CloseDevice(int DevIndex);
    bool WINAPI USB_GetDeviceInfo(int DevIndex,PDEVICE_INFO pDevInfo,char *pFunctionStr);

#ifdef __cplusplus
}
#endif

#endif

