// USB2XXXCNTPulseCounter.cpp : Defines the entry point for the console application.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usb_device.h"
#include "usb2cnt.h"


int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    bool state;
    int ret;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        getchar();
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        getchar();
        return 0;
    }
    //获取固件信息
    char FuncStr[256]={0};
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        getchar();
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
    //配置计数器参数
    CNT_CONFIG CntConfig[4];
    unsigned char ChannelMask = 0xF;//开启所有通道
    for(int i=0;i<4;i++){
        CntConfig[i].CounterBitWide = COUNTER_BITS32;       //32位计数
        CntConfig[i].CounterMode = COUNTER_MODE_UP;         //加计数
        CntConfig[i].CounterPolarity = COUNTER_POL_RISING;  //上升沿计数
        CntConfig[i].CounterPinMode = COUNTER_PIN_UP;       //引脚内部上拉
    }
    ret = CNT_Init(DevHandle[0],ChannelMask,CntConfig);
    if(ret != CNT_SUCCESS){
        printf("Config counter error!\n");
        getchar();
        return 0;
    }
    //将计数器初始值设置为0
    unsigned int CntValueTemp[4] = {0};
    ret = CNT_SetValue(DevHandle[0],ChannelMask,CntValueTemp);
    if(ret != CNT_SUCCESS){
        printf("Set counter values error!\n");
        getchar();
        return 0;
    }
    //启动计数器
    ret = CNT_Start(DevHandle[0],ChannelMask);
    if(ret != CNT_SUCCESS){
        printf("Start counter error!\n");
        getchar();
        return 0;
    }
    //循环获取计数器的值
    while(1)
    {
        unsigned int CntValueTemp[4];
        ret = CNT_GetValue(DevHandle[0],ChannelMask,CntValueTemp);
        if(ret != CNT_SUCCESS){
            printf("Get counter value error!\n");
            break;;
        }
        system("cls");
        printf("Counter 1----Counter 2----Counter 3----Counter 4\n");
        printf("%-13.1u%-13.1u%-13.1u%-13.1u\n",CntValueTemp[0],CntValueTemp[1],CntValueTemp[2],CntValueTemp[3]);
        Sleep(100);
    }
    //停止计数器
    ret = CNT_Stop(DevHandle[0],ChannelMask);
    if(ret != CNT_SUCCESS){
        printf("Stop counter error!\n");
        getchar();
        return 0;
    }
	return 0;
}

