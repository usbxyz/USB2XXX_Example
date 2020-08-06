  /*
  ******************************************************************************
  * @file     : USB2XXX_CANFD_Test.cpp
  * @Copyright: TOOMOSS 
  * @Revision : ver 1.0
  * @Date     : 2019/12/19 9:33
  * @brief    : USB2XXX CANFD test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2019, TOOMOSS
  * http://www.toomoss.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usb_device.h"
#include "usb2canfd.h"
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    int CANIndex = 0;//CAN通道号
    bool state;
    int ret;
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
    //初始化配置CAN
    CANFD_INIT_CONFIG CANFDConfig;
    CANFDConfig.Mode = 1;        //0-正常模式，1-自发自收模式
    CANFDConfig.RetrySend = 1;   //使能自动重传
    CANFDConfig.ISOCRCEnable = 1;//使能ISOCRC
    CANFDConfig.ResEnable = 1;   //使能内部终端电阻（若总线上没有终端电阻，则必须使能终端电阻才能正常传输数据）
    //波特率参数可以用TCANLINPro软件里面的波特率计算工具计算
    //仲裁段波特率参数,波特率=40M/NBT_BRP*(1+NBT_SEG1+NBT_SEG2)
    CANFDConfig.NBT_BRP = 1;
    CANFDConfig.NBT_SEG1 = 63;
    CANFDConfig.NBT_SEG2 = 16;
    CANFDConfig.NBT_SJW = 16;
    //数据域波特率参数,波特率=40M/DBT_BRP*(1+DBT_SEG1+DBT_SEG2)
    CANFDConfig.DBT_BRP = 1;
    CANFDConfig.DBT_SEG1 = 15;
    CANFDConfig.DBT_SEG2 = 4;
    CANFDConfig.DBT_SJW = 4;
    ret = CANFD_Init(DevHandle[0],CANIndex,&CANFDConfig);
    if(ret != CANFD_SUCCESS){
        printf("CANFD Init Error!\n");
        return 0;
    }else{
        printf("CANFD Init Success!\n");
    }
    //配置过滤器，若不配置过滤器，默认是接收所有数据
    CANFD_FILTER_CONFIG CANFDFilter;
    CANFDFilter.Index = 0;//取值范围为：0~31
    CANFDFilter.Enable = 1;//0-禁止，1-使能
    //配置为只接收扩展帧数据
    CANFDFilter.ID_Accept = 0x80000000;
    CANFDFilter.ID_Mask = 0x80000000;
    ret = CANFD_SetFilter(DevHandle[0], CANIndex, &CANFDFilter, 1);
    if (ret != CANFD_SUCCESS){
        printf("Config filter failed!\n");
        return 0;
    }else{
        printf("Config filter success!\n");
    }
    //启动CAN数据接收
    ret = CANFD_StartGetMsg(DevHandle[0], CANIndex);
    if (ret != CANFD_SUCCESS){
        printf("Start receive CANFD failed!\n");
        return 0;
    }else{
        printf("Start receive CANFD Success!\n");
    }
    //发送CAN数据
    CANFD_MSG CanMsg[5];
    for (int i = 0; i < 5; i++){
        CanMsg[i].Flags = CANFD_MSG_FLAG_FDF;//bit[0]-BRS,bit[1]-ESI,bit[2]-FDF,bit[6..5]-Channel,bit[7]-RXD
        CanMsg[i].DLC = 16;
        CanMsg[i].ID = i|CANFD_MSG_FLAG_IDE;
        for (int j = 0; j < CanMsg[i].DLC; j++){
            CanMsg[i].Data[j] = j;
        }
    }
    int SendedMsgNum = CANFD_SendMsg(DevHandle[0], CANIndex, CanMsg, 5);
    if (SendedMsgNum >= 0){
        printf("Success send frames:%d\n", SendedMsgNum);
    }else{
        printf("Send CAN data failed!\n");
        return 0;
    }
    //接收数据
    //延时
#ifndef OS_UNIX
    Sleep(50);
#else
    usleep(50*1000);
#endif
    //读取接收数据缓冲中的数据
    CANFD_MSG CanMsgBuffer[1024];
    int GetMsgNum = CANFD_GetMsg(DevHandle[0], CANIndex, CanMsgBuffer, 1024);
    if (GetMsgNum > 0){
        for (int i = 0; i < GetMsgNum; i++){
            printf("CanMsg[%d].ID = 0x%08X\n", i, CanMsgBuffer[i].ID & CANFD_MSG_FLAG_ID_MASK);
            printf("CanMsg[%d].TimeStamp = %d\n", i, CanMsgBuffer[i].TimeStamp);
            printf("CanMsg[%d].Data = ", i);
            for (int j = 0; j < CanMsgBuffer[i].DLC; j++)
            {
                printf("0x%02X ", CanMsgBuffer[i].Data[j]);
            }
            printf("\n");
        }
    }else if (GetMsgNum < 0){
        printf("Get CAN data error!\n");
    }
    //停止接收数据
    ret = CANFD_StopGetMsg(DevHandle[0], CANIndex);
    if (ret != CANFD_SUCCESS){
        printf("Stop receive CANFD failed!\n");
        return 0;
    }else{
        printf("Stop receive CANFD Success!\n");
    }
    return 0;
}

