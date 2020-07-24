  /*
  ******************************************************************************
  * @file     : USB2XXXCANTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX IIC test demo
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
#include "usb2can.h"
#include "can_uds.h"

#define GET_FIRMWARE_INFO   1

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int CANIndex = 0;
    int DEVIndex = 0;
    bool state;
    int ret;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\r\n");
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DevHandle[DEVIndex]);
    if(!state){
        printf("Open device error!\r\n");
        return 0;
    }
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandle[DEVIndex],&DevInfo,FunctionStr);
    if(!state){
        printf("Get device infomation error!\r\n");
        return 0;
    }else{
        printf("Firmware Info:\r\n");
	    printf("Firmware Name:%s\r\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\r\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\r\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\r\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%s\r\n",FunctionStr);
    }
#endif
    //初始化配置CAN
    CAN_INIT_CONFIG CANConfig;
    CANConfig.CAN_Mode = 0x80;//正常模式，接入终端电阻
    CANConfig.CAN_ABOM = 0;//禁止自动离线
    CANConfig.CAN_NART = 0;//使能报文重传
    CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
    CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
    //配置波特率，配置为500K
    CANConfig.CAN_BRP = 4;
    CANConfig.CAN_BS1 = 16;
    CANConfig.CAN_BS2 = 4;
    CANConfig.CAN_SJW = 1;
    ret = CAN_Init(DevHandle[DEVIndex],CANIndex,&CANConfig);
    if(ret != CAN_SUCCESS){
        printf("Config CAN failed!\r\n");
        return 0;
    }else{
        printf("Config CAN Success!\r\n");
    }
    //获取发动机转速
    uint8_t req_data[]={0x01,0x0C};
    uint8_t res_data[4096]={0};
    CAN_UDS_ADDR UDSAddr;
    UDSAddr.ExternFlag = 0;//使用标准帧
    UDSAddr.AddrFormats = 0;
    UDSAddr.ReqID = 0x7DF;
    UDSAddr.ResID = 0x7E8;
    ret = CAN_UDS_Request(DevHandle[DEVIndex],CANIndex,UDSAddr,req_data,sizeof(req_data));
    if(ret != CAN_UDS_OK){
        printf("CAN UDS request failed! %d\r\n",ret);
    }else{
        printf("Request:");
        for(int i=0;i<sizeof(req_data);i++){
            printf(" %02X",req_data[i]);
        }
        printf("\r\n");
    }
    ret = CAN_UDS_Response(DevHandle[DEVIndex],CANIndex,UDSAddr,res_data,1000);
    if(ret <= CAN_UDS_OK){
        printf("CAN UDS response failed! %d\r\n",ret);
    }else{
        printf("Response:");
        for(int i=0;i<ret;i++){
            printf(" %02X",res_data[i]);
        }
        printf("\r\n");
        if((res_data[0]==(0x40+req_data[0]))&&(req_data[1]==res_data[1])){
            printf("转速 = %d Rpm\r\n",((res_data[2]<<8)|res_data[3])/4);
        }
    }
    //获取车速
    req_data[1] = 0x0D;
    ret = CAN_UDS_Request(DevHandle[DEVIndex],CANIndex,UDSAddr,req_data,sizeof(req_data));
    if(ret != CAN_UDS_OK){
        printf("CAN UDS request failed! %d\r\n",ret);
    }else{
        printf("Request:");
        for(int i=0;i<sizeof(req_data);i++){
            printf(" %02X",req_data[i]);
        }
        printf("\r\n");
    }
    ret = CAN_UDS_Response(DevHandle[DEVIndex],CANIndex,UDSAddr,res_data,1000);
    if(ret <= CAN_UDS_OK){
        printf("CAN UDS response failed! %d\r\n",ret);
    }else{
        printf("Response:");
        for(int i=0;i<ret;i++){
            printf(" %02X",res_data[i]);
        }
        printf("\r\n");
        if((res_data[0]==(0x40+req_data[0]))&&(req_data[1]==res_data[1])){
            printf("车速 = %d Km/h\r\n",res_data[2]);
        }
    }
	return 0;
}

