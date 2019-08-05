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
    int DevHandle[10];
    bool state;
    int ret;
    unsigned int PinValue;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //获取固件信息
    char FuncStr[256]={0};
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FuncStr);
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
    GPIO_SetOutput(DevHandle[0],0xFFFF,1);
    //获取起始时间,主要用于测试发送速度
    LARGE_INTEGER litmp;
    LONGLONG StartTime,EndTime;
    double dfFreq;
    int TestNum = 10000;
    CRITICAL_SECTION  _critical;
    /*初始化，最先调用的函数。没什么好说的，一般windows编程都有类似初始化的方法*/
    InitializeCriticalSection(&_critical);
    /*
    把代码保护起来。调用此函数后，他以后的资源其他线程就不能访问了。
    */
    EnterCriticalSection(& _critical);
    FILE *fp;
    if((fp=fopen("GPIOSpeed.csv","wt"))==NULL){
        printf("Cannot open file strike any key exit!\n");
        getchar();
        exit(1);
    }
    int max0=0;//控制时间小于1ms的次数
    int max1=0;//控制时间小于2ms，大于1ms的次数
    int max2=0;//控制时间小于3ms，大于2ms的次数
    int max3=0;//控制时间小于4ms，大于3ms的次数
    int max4=0;//控制大于4ms的次数
    for(int i=0;i<TestNum;i++){
        QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
        dfFreq = (double)litmp.QuadPart;
        QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
        StartTime = litmp.QuadPart;       // Start time
        GPIO_Write(DevHandle[0],0xFFFF,0xFFFF);
        QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
        EndTime = litmp.QuadPart; // Stop time
        float time = 1000*((EndTime-StartTime)/dfFreq);
        //GPIO_Write(DevHandle[0],0xFFFF,0x0000);
        fprintf(fp,"%f\n",time);
        if(time>=4){
            max4++;
        }else if(time>=3){
            max3++;
        }else if(time>=2){
            max2++;
        }else if(time>=1){
            max1++;
        }else{
            max0++;
        }
    }
    fclose(fp);
    printf("控制时间大于4ms的次数 = %d\n",max4);
    printf("控制时间大于3ms,小于4ms的次数 = %d\n",max3);
    printf("控制时间大于2ms,小于3ms的次数  = %d\n",max2);
    printf("控制时间大于1ms,小于2ms的次数  = %d\n",max1);
    printf("控制时间小于1ms的次数  = %d\n",max0);
    /*
    离开临界区，表示其他线程能够进来了。注意EnterCritical和LeaveCrticalSection必须是成对出现的!当然除非你是想故意死锁！
    */
    LeaveCriticalSection(&_critical);
    /*释放资源，确定不使用_critical时调用，一般在程序退出的时候调用。如果以后还要用_critical，则要重新调用InitializeCriticalSection
    */
    DeleteCriticalSection(&_critical); 

    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    return 0;
}

