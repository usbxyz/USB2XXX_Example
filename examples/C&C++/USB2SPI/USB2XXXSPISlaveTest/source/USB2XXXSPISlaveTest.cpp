  /*
  ******************************************************************************
  * @file     : USB2XXXSPISlaveTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI slave test demo
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
#include <time.h> 
#include "usb_device.h"
#include "usb2spi.h"

FILE *pBinFile;
int FileSize = 0;
int WINAPI SlaveGetData(int DevHandle,int SPIIndex,unsigned char *pData,int DataNum)
{
    printf("DevHandle = %08X,SPIIndex=%d,DataNum=%d\n",DevHandle,SPIIndex,DataNum);//测试用，正常接收数据到文件的时候需要注释掉
    fwrite(pData,1,DataNum,pBinFile);
    FileSize += DataNum;
    return 0;
}

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    SPI_CONFIG SPIConfig;
    int DevHandle[10];
    int SPIIndex = SPI1_CS0;
    bool state;
    int ret;
    int devNum=0;//若同时连接了多个设备，那么每个设备都会被启动数据接收
    unsigned char ReadBuffer[20480];
    unsigned char WriteBuffer[20480];
    //扫描查找设备
    devNum = USB_ScanDevice(DevHandle);
    if(devNum <= 0){
        printf("No device connected!\n");
        getchar();
        return 0;
    }
    devNum = 1;
    //打开设备
    for(int i=0;i<devNum;i++){
        state = USB_OpenDevice(DevHandle[i]);
        if(!state){
            printf("Open device error!\n");
            getchar();
            return 0;
        }
    }
    //获取固件信息
    for(int i=0;i<devNum;i++){
        char FunctionStr[512]={0};
        state = DEV_GetDeviceInfo(DevHandle[i],&DevInfo,FunctionStr);
        if(!state){
            printf("Get device infomation error!\n");
            getchar();
            return 0;
        }else{
            printf("Firmware Name:%s\n",DevInfo.FirmwareName);
            printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
            printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
            printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
            printf("Firmware Functions:%s\n",FunctionStr);
            printf("Firmware Serial Number:%08X%08X%08X\n",DevInfo.SerialNumber[0],DevInfo.SerialNumber[1],DevInfo.SerialNumber[2]);
        }
    }
    //输入SPI相关参数，必须和主机匹配
    int DataTemp;
    printf("Please input CPHA(0 or 1):");
    scanf("%d",&DataTemp);
    SPIConfig.CPHA = DataTemp;
    printf("Please input CPOL(0 or 1):");
    scanf("%d",&DataTemp);
    SPIConfig.CPOL = DataTemp;
    printf("SPIConfig.CPHA = %d\n",SPIConfig.CPHA);
    printf("SPIConfig.CPOL = %d\n",SPIConfig.CPOL);
    //配置SPI总线相关参数(配置为从机模式)
    SPIConfig.Mode = SPI_MODE_HARD_FDX;
    SPIConfig.ClockSpeedHz = 50000000;
    SPIConfig.LSBFirst = SPI_MSB;
    SPIConfig.Master = SPI_SLAVE;
    SPIConfig.SelPolarity = SPI_SEL_LOW;
    for(int i=0;i<devNum;i++){
        ret = SPI_Init(DevHandle[i],SPIIndex,&SPIConfig);
        if(ret != SPI_SUCCESS){
            printf("Initialize device error!\n");
            getchar();
            return 0;
        }
    }
    time_t timep;
    struct tm *tp;
    time(&timep);
    tp =localtime(&timep);
    //通过回调函数方式接收数据，并将数据写入文件中
    //输入文件名
    char BinFileName[256];
    sprintf(BinFileName,"SPIData_%02d%02d%02d%02d.bin",tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
    printf("Press any key to exit the data reception!\n");
    pBinFile=fopen(BinFileName,"wb"); //获取文件的指针
    if(pBinFile == NULL){
        printf("Open file faild\n");
        getchar();
        return 0;
    }
    for(int i=0;i<devNum;i++){
        ret = SPI_SlaveContinueRead(DevHandle[i],SPIIndex,NULL);
        if(ret != SPI_SUCCESS){
            printf("Start continue read faild\n");
            getchar();
            return 0;
        }
    }
    while(1){
        ret = SPI_SlaveGetBytes(DevHandle[0],SPIIndex,ReadBuffer,10);
        for(int i=0;i<ret;i++){
            printf("%02X ",ReadBuffer[i]);
        }
        //Sleep(100);
    }
    getchar();
    getchar();
    for(int i=0;i<devNum;i++){
        SPI_SlaveContinueWriteReadStop(DevHandle[i],SPIIndex);
    }
    fclose(pBinFile);
    //将二进制文件转换为十六进行显示的txt文件，方便数据查看
    printf("start convert file...\n");
    FILE *pTxtFile;
    char TextFileName[256];
    sprintf(TextFileName,"SPIData_%02d%02d%02d%02d.txt",tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
    pTxtFile=fopen(TextFileName,"wb"); //获取文件的指针
    if(pTxtFile == NULL){
        printf("Open file faild\n");
        getchar();
        return 0;
    }

    pBinFile = fopen(BinFileName,"rb");
    if(pBinFile == NULL){
        printf("Open file faild\n");
        getchar();
        return 0;
    }
    
    do{
        ret = fread(ReadBuffer,1,16,pBinFile);
        if(ret <= 0){
            break;
        }else{
            for(int i=0;i<ret;i++){
                fprintf(pTxtFile,"%02X ",ReadBuffer[i]);
            }
            fprintf(pTxtFile,"\r\n");
        }
    }while(ret<16);

    fclose(pBinFile);
    fclose(pTxtFile);
    for(int i=0;i<devNum;i++){
        USB_CloseDevice(DevHandle[i]);
    }
    
    printf("FileSize = %d Byte\n",FileSize);
	printf("Output bin file:%s\n",BinFileName);
	printf("Output txt file:%s\n",TextFileName);
    printf("SPI Slave Test Success!\n");
    getchar();
    return 0;
}
