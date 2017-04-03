  /*
  ******************************************************************************
  * @file     : USB2XXXGPIOSpeedTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX GPIO speed test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2014, usbxyz.com
  * http://www.usbxyz.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usb2gpio.h"
#include "usb_device.h"


int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevIndex = 0;
    bool state;
    int ret;
    unsigned int PinValue;
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
    char FuncStr[256]={0};
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Info:\n");
        printf("    Name:%s\n",DevInfo.FirmwareName);
        printf("    Build Date:%s\n",DevInfo.BuildDate);
        printf("    Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("    Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("    Functions:%08X\n",DevInfo.Functions);
        printf("    Functions:%s\n",FuncStr);
    }
    //输出测试——没上下拉
    GPIO_SetOutput(DevIndex,0xFFFF,1);
    //获取起始时间,主要用于测试发送速度
    LARGE_INTEGER litmp;
    LONGLONG StartTime,EndTime;
    double dfFreq;
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    int TestNum = 100000;
    CRITICAL_SECTION  _critical;
    /*初始化，最先调用的函数。没什么好说的，一般windows编程都有类似初始化的方法*/
    InitializeCriticalSection(&_critical);
    /*
    把代码保护起来。调用此函数后，他以后的资源其他线程就不能访问了。
    */
    EnterCriticalSection(& _critical);
    for(int i=0;i<TestNum;i++){
        GPIO_Write(DevIndex,0xFFFF,0xFFFF);
        GPIO_Write(DevIndex,0xFFFF,0x0000);
    }
    /*
    离开临界区，表示其他线程能够进来了。注意EnterCritical和LeaveCrticalSection必须是成对出现的!当然除非你是想故意死锁！
    */
    LeaveCriticalSection(&_critical);
    /*释放资源，确定不使用_critical时调用，一般在程序退出的时候调用。如果以后还要用_critical，则要重新调用InitializeCriticalSection
    */
    DeleteCriticalSection(&_critical); 
	//获取结束时间并打印输出耗时和速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
	// Print the write data speed information
	printf("-----------------------www.usbxyz.com-----------------------\n");
	printf("控制GPIO次数: %d  \n",TestNum*2);
	printf("消耗总时间: %f s\n",(EndTime-StartTime)/dfFreq);
	printf("控制GPIO的速度: %.3f 次/s\n",(TestNum*2)/((EndTime-StartTime)/dfFreq));
	printf("-----------------------www.usbxyz.com-----------------------\n");

    //关闭设备
    USB_CloseDevice(DevIndex);
    return 0;
}

