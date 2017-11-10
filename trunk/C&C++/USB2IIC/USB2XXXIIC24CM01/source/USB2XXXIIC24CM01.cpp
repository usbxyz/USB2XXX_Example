  /*
  ******************************************************************************
  * @file     : USB2XXXIICDHT8X.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX IIC DHT8X demo
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
#include "usb2iic.h"
//注意，使用这个程序必须烧指定的固件才行，可以联系我（QQ:188298598）提供
int main(int argc, const char* argv[])
{
    const int pageSize = 256;
    const int pageNum = 0x200;
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int IICIndex = 0;//注意要跟硬件连接匹配
    bool state;
    int ret;
    unsigned int PinValue;
    short Buffer[40960];
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
    //初始化配置I2C
    IIC_CONFIG IIC_Config;
    IIC_Config.EnablePu = 1;            //使能内部上拉电阻
    IIC_Config.AddrBits = 7;            //7bit地址模式
    IIC_Config.ClockSpeedHz = 400000;     //时钟频率400KHz
    IIC_Config.Master = 1;              //主机模式
    ret = IIC_Init(DevHandle[0],IICIndex,&IIC_Config);
    if(ret != IIC_SUCCESS){
        printf("Config IIC failed!\n");
        getchar();
        return 0;
    }else{
        printf("Config IIC IIC_SUCCESS!\n");
    }
    //整片写数据测试
    printf("Start write data...\n");
    uint8_t write_data_buffer[pageSize*pageNum];
    for(int i=0;i<(pageSize*pageNum);i++){
        write_data_buffer[i] = rand();//用随机数来验证
    }
    for(int p=0;p<pageNum;p++){
        int sub_addr = p*pageSize;
        uint16_t slave_addr = 0x50;
        uint8_t write_buffer[pageSize+2];       //多2字节用来存储子地址
        write_buffer[0] = (sub_addr>>8)&0xFF;   //子地址高字节
        write_buffer[1] = sub_addr&0xFF;        //子地址低字节
        slave_addr |= (sub_addr>>16)&0xFF;      //页选择
        memcpy(&write_buffer[2],&write_data_buffer[p*pageSize],pageSize);
        ret = IIC_WriteBytes(DevHandle[0],IICIndex,slave_addr,write_buffer,pageSize+2,100);
        if(ret != IIC_SUCCESS){
            printf("Write IIC failed!\n");
            printf("Error Code:%d\n",ret);
            getchar();
            return 0;
        }
#ifndef OS_UNIX
        Sleep(10);
#else
        usleep(10*1000);
#endif
    }
    //整片读数据测试
    printf("Start read data...\n");
    uint8_t read_data_buffer[pageSize*pageNum];
    for(int p=0;p<pageNum;p++){
        int sub_addr = p*pageSize;
        uint16_t slave_addr = 0x50;
        uint8_t write_buffer[2];                //2字节用来存储子地址
        write_buffer[0] = (sub_addr>>8)&0xFF;   //子地址高字节
        write_buffer[1] = sub_addr&0xFF;        //子地址低字节
        slave_addr |= (sub_addr>>16)&0xFF;      //页选择
        ret = IIC_WriteReadBytes(DevHandle[0],IICIndex,slave_addr,write_buffer,2,&read_data_buffer[p*pageSize],pageSize,100);
        if(ret != IIC_SUCCESS){
            printf("Read IIC failed!\n");
            printf("Error Code:%d\n",ret);
            getchar();
            return 0;
        }
        Sleep(10);
    }
    //对比数据
    int error_num = 0;
    for(int i=0;i<(pageSize*pageNum);i++){
        if(write_data_buffer[i] != read_data_buffer[i]){
            error_num++;
        }
    }
    printf("Write read success,error data num = %d byte\n",error_num);

    //关闭设备
    USB_CloseDevice(DevHandle[0]);
	return 0;
}

