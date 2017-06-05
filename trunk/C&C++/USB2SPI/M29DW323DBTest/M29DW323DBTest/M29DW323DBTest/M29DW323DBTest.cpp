  /*
  ******************************************************************************
  * @file     : M29DW323DBTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2017/05/27 17:15
  * @brief    : M29DW323DB test demo
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
#include <math.h>
#include <time.h>  
#include "usb_device.h"
#include "usb2m29dw.h"

typedef struct
{
    uint32_t BlockStartAddr;//块起始地址
    uint32_t BlockSize;//块大小(Word)
}BlockInfo;

//主函数
int main(int argc, const char* argv[])
{
    int DevHandle[10];
    BlockInfo ChipBlockInfo_M29DW323DT[71];
    BlockInfo ChipBlockInfo_M29DW323DB[71];
    uint16_t WriteDataBuffer[20480];
    uint16_t ReadDataBuffer[20480];
    int TestDataNum = 4096;//每次写入的数据Word数
    DEVICE_INFO DevInfo;
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
    //获取固件信息
    char FuncStr[128]={0};
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("Firmware Functions:%s\n",FuncStr);
    }
    //生成M29DW323DT块信息
    for(int i=0;i<63;i++){
        ChipBlockInfo_M29DW323DT[i].BlockStartAddr = i*0x008000;
        ChipBlockInfo_M29DW323DT[i].BlockSize = 0x008000;
    }
    for(int i=63;i<71;i++){
        ChipBlockInfo_M29DW323DT[i].BlockStartAddr = 63*0x008000+(i-63)*0x001000;
        ChipBlockInfo_M29DW323DT[i].BlockSize = 0x001000;
    }
    //for(int i=0;i<71;i++){
    //    printf("%02d   %06X   %06X\n",i,ChipBlockInfo_M29DW323DT[i].BlockStartAddr,ChipBlockInfo_M29DW323DT[i].BlockSize);
    //}
    //生成M29DW323DB块信息
    for(int i=0;i<8;i++){
        ChipBlockInfo_M29DW323DB[i].BlockStartAddr = i*0x001000;
        ChipBlockInfo_M29DW323DB[i].BlockSize = 0x001000;
    }
    for(int i=8;i<71;i++){
        ChipBlockInfo_M29DW323DB[i].BlockStartAddr = 8*0x001000+(i-8)*0x008000;
        ChipBlockInfo_M29DW323DB[i].BlockSize = 0x008000;
    }
    //BlockErase(ChipBlockInfo_M29DW323DB[1].BlockStartAddr);
    //for(int i=0;i<71;i++){
    //    printf("%02d   %06X   %06X\n",i,ChipBlockInfo_M29DW323DT[i].BlockStartAddr,ChipBlockInfo_M29DW323DT[i].BlockSize);
    //}
    //初始化
    ret = M29DW_Init(DevHandle[0]);
    if(ret != M29DW_SUCCESS){
        printf("Init error!\n");
        return 0;
    }
    //获取ID
    printf("Read ID...\n");
    //M29DW323DT:0020225E,M29DW323DB:0020225F
    unsigned short ManufacturerCode,DeviceCode;
    ret = M29DW_ReadID(DevHandle[0],&ManufacturerCode,&DeviceCode);
    if(ret != M29DW_SUCCESS){
        printf("Read ID error!\n");
        return 0;
    }else{
        printf("ManufacturerCode = %04X\n",ManufacturerCode);
        printf("DeviceCode = %04X\n",DeviceCode);
    }
    //循环写入数据
    for(int b=0;b<(sizeof(ChipBlockInfo_M29DW323DB)/sizeof(BlockInfo));b++){
        srand((unsigned int)time(NULL));
        printf("Erase block(%d)...\n",b);
        ret = M29DW_BlockErase(DevHandle[0],ChipBlockInfo_M29DW323DB[b].BlockStartAddr);
        if(ret != M29DW_SUCCESS){
            printf("Block Erase error!\n");
            return 0;
        }
        printf("Write&Read block(%d) data...\n",b);
        for(int k=0;k<(ChipBlockInfo_M29DW323DB[b].BlockSize/TestDataNum);k++){
            //准备数据
            for(int i=0;i<TestDataNum;i++){
                WriteDataBuffer[i] = rand();
            }
            //写入数据
            ret = M29DW_WriteData(DevHandle[0],ChipBlockInfo_M29DW323DB[b].BlockStartAddr+k*TestDataNum,WriteDataBuffer,TestDataNum);
            if(ret != M29DW_SUCCESS){
                printf("Write data error!\n");
                return 0;
            }
            //读出数据
            ret = M29DW_ReadData(DevHandle[0],ChipBlockInfo_M29DW323DB[b].BlockStartAddr+k*TestDataNum,ReadDataBuffer,TestDataNum);
            if(ret != M29DW_SUCCESS){
                printf("Read data error!\n");
                return 0;
            }
            //对比数据
            int ErrorData = 0;
            for(int i=0;i<TestDataNum;i++){
                if(WriteDataBuffer[i] != ReadDataBuffer[i]){
                    ErrorData++;
                }
            }
            printf("Write&Read block(%d.%d) error data = %d\n",b,k,ErrorData);
        }
    }
    printf("\nM29DW323DB Test OK\n");
    getchar();
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    return 0;
}