  /*
  ******************************************************************************
  * @file     : USB2XXXSPI93CXX.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI 93CXX test demo
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
#include "usb_device.h"
#include "usb2spi.h"
//选择对应的芯片
#define AT93C46     0
#define AT93C56     0
#define AT93C66     1
#define AT93C76     0
#define AT93C86     0

#define DATA_BITS           16//数据组织方式，16-16bit读写方式，8-8bit读写方式

#if AT93C46
    #define DATA_SIZE_OF_BYTE   128    //总的字节数
    #define ADDR_BITS           7-((DATA_BITS>>4)&0x01)
#elif AT93C56
    #define DATA_SIZE_OF_BYTE   256    //总的字节数
    #define ADDR_BITS           9-((DATA_BITS>>4)&0x01)
#elif AT93C66
    #define DATA_SIZE_OF_BYTE   512    //总的字节数
    #define ADDR_BITS           9-((DATA_BITS>>4)&0x01)
#elif AT93C76
    #define DATA_SIZE_OF_BYTE   1024    //总的字节数
    #define ADDR_BITS           11-((DATA_BITS>>4)&0x01)
#elif AT93C86
    #define DATA_SIZE_OF_BYTE   2048    //总的字节数
    #define ADDR_BITS           11-((DATA_BITS>>4)&0x01)
#endif

#if DATA_BITS==16
    #define DATA_SIZE           (DATA_SIZE_OF_BYTE>>1)
#else
    #define DATA_SIZE           (DATA_SIZE_OF_BYTE)
#endif
//主函数
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevIndex = 0;
    int SPIIndex = SPI1_CS0;
    bool state;
    int ret,k=0;
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
    char FuncStr[128]={0};
    state = USB_GetDeviceInfo(DevIndex,&DevInfo,FuncStr);
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
    //配置SPI总线相关参数
    SPI_CONFIG SPIConfig;
    SPIConfig.Mode = SPI_MODE_SOFT_HDX;
    SPIConfig.ClockSpeedHz = 200000;
    SPIConfig.CPHA = 0;
    SPIConfig.CPOL = 0;
    SPIConfig.LSBFirst = SPI_MSB;
    SPIConfig.Master = SPI_MASTER;
    SPIConfig.SelPolarity = SPI_SEL_HIGH;
    ret = SPI_Init(DevIndex,SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    printf("ADDR_BITS = %d\n",ADDR_BITS);
    //写数据到芯片
    printf("Start write data to chip...\n");
    char WriteBitsStr[256]={0};
    k=0;
    //合成EWEN的二进制数据
    WriteBitsStr[k++]='1';//START BIT
    WriteBitsStr[k++]='0';//OP1
    WriteBitsStr[k++]='0';//OP2
    WriteBitsStr[k++]='1';
    WriteBitsStr[k++]='1';
    for(int j=0;j<(ADDR_BITS-2);j++){
        WriteBitsStr[k++] = '0';
    }
    WriteBitsStr[k++] = '\0';//字符串结束符，必须加上
    ret = SPI_WriteBits(DevIndex,SPIIndex,WriteBitsStr);
    if(ret != SPI_SUCCESS){
        printf("Write bits errror!\n");
        return -1;
    }
    Sleep(1);
#if DATA_BITS == 16
    unsigned short WriteDataBuffer[DATA_SIZE];
    for(int i=0;i<DATA_SIZE;i++){
        WriteDataBuffer[i] = (i<<8)|(~i&0xFF);
    }
#else
    unsigned char WriteDataBuffer[DATA_SIZE];
    for(int i=0;i<DATA_SIZE;i++){
        WriteDataBuffer[i] = i&0xFF;
    }
#endif
    unsigned int Addr = 0;
    //循环将数据写入芯片
    for(int i=0;i<DATA_SIZE;i++,Addr++){
        char WriteBitsStr[256]={0};
        k = 0;
        //合成WRITE的二进制数据
        WriteBitsStr[k++]='1';
        WriteBitsStr[k++]='0';
        WriteBitsStr[k++]='1';
        //合成地址二进制字符串
        for(int j=0;j<ADDR_BITS;j++){
            if(Addr&(1<<((ADDR_BITS-1)-j))){
                WriteBitsStr[k++] = '1';
            }else{
                WriteBitsStr[k++] = '0';
            }
        }
        //合成数据二进制字符串
        for(int j=0;j<DATA_BITS;j++){
            if(WriteDataBuffer[i]&(1<<((DATA_BITS-1)-j))){
                WriteBitsStr[k++] = '1';
            }else{
                WriteBitsStr[k++] = '0';
            }
        }
        WriteBitsStr[k++] = '\0';//字符串结束符，必须加上
        //WRITE CYCLE (WRITE)
        ret = SPI_WriteBits(DevIndex,SPIIndex,WriteBitsStr);
        if(ret != SPI_SUCCESS){
            printf("Write bits errror!\n");
            return -1;
        }
        //打印显示数据
#if DATA_BITS == 16
        printf("%04X ",WriteDataBuffer[i]);
#else
        printf("%02X ",WriteDataBuffer[i]);
#endif
#if DATA_BITS == 16
        if(((i+1)%16)==0){
#else
        if(((i+1)%32)==0){
#endif
            printf("\n");
        }
        Sleep(5);
    }
    //合成EWDS的二进制数据,该指令必须加，否则会出现数据出错的情况
    k = 0;
    WriteBitsStr[k++]='1';//START BIT
    WriteBitsStr[k++]='0';//OP1
    WriteBitsStr[k++]='0';//OP2
    WriteBitsStr[k++]='0';
    WriteBitsStr[k++]='0';
    for(int j=0;j<(ADDR_BITS-2);j++){
        WriteBitsStr[k++] = '0';
    }
    WriteBitsStr[k++] = '\0';//字符串结束符，必须加上
    //EWDS
    ret = SPI_WriteBits(DevIndex,SPIIndex,WriteBitsStr);
    if(ret != SPI_SUCCESS){
        printf("Write bits errror!\n");
        return -1;
    }
    //循环读数据
    printf("Start read data from chip...\n");
#if DATA_BITS == 16
    unsigned short ReadDataBuffer[DATA_SIZE];
#else
    unsigned char ReadDataBuffer[DATA_SIZE];
#endif
    Addr = 0;
    for(int i=0;i<DATA_SIZE;i++,Addr++){
        static int BreakNum = 0;
        char WriteBitsStr[256]={0};
        char ReadBitsStr[256]={0};
        k = 0;
        //合成READ命令的二进制字符串
        WriteBitsStr[k++]='1';
        WriteBitsStr[k++]='1';
        WriteBitsStr[k++]='0';
        //合成地址二进制字符串
        for(int j=0;j<ADDR_BITS;j++){
            if(Addr&(1<<((ADDR_BITS-1)-j))){
                WriteBitsStr[k++] = '1';
            }else{
                WriteBitsStr[k++] = '0';
            }
        }
        WriteBitsStr[k++] = '\0';
        ret = SPI_WriteReadBits(DevIndex,SPIIndex,WriteBitsStr,ReadBitsStr,DATA_BITS);
        if(ret != SPI_SUCCESS){
            printf("WriteRead bits errror!\n");
            return -1;
        }else{
            //将读出的二进制字符串还原为十六进制数据
            uint16_t ReadData = 0;
            for(int j=0;j<DATA_BITS;j++){
                ReadData <<= 1;
                if(ReadBitsStr[j]=='1'){
                    ReadData |= 1;
                }
            }
            //将数据存放到数据缓冲区
            ReadDataBuffer[i] = ReadData;
            //打印显示数据
#if DATA_BITS == 16
            printf("%04X ",ReadDataBuffer[i]);
#else
            printf("%02X ",ReadDataBuffer[i]);
#endif
#if DATA_BITS == 16
            if(((i+1)%16)==0){
#else
            if(((i+1)%32)==0){
#endif
                printf("\n");
            }
        }
    }
    return 0;
}

