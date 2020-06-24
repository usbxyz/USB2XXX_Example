  /*
  ******************************************************************************
  * @file     : USB2XXXSPIW25Q64.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX SPI W25Q64 test demo
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
#include "usb2spi.h"

#define USE_SPI_FLASH_API   1

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    int SPIIndex = 0x00;
    bool state;
    int ret,i;
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
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,NULL);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
	    printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%08X\n",DevInfo.Functions);
    }
#if !USE_SPI_FLASH_API
    //配置SPI总线相关参数
    SPI_CONFIG SPIConfig;
    SPIConfig.Mode = 1;
    SPIConfig.ClockSpeedHz = 50000000>>2;
    SPIConfig.CPHA = 1;
    SPIConfig.CPOL = 1;
    SPIConfig.LSBFirst = 0;
    SPIConfig.Master = 1;
    SPIConfig.SelPolarity = 0;
    ret = SPI_Init(DevHandle[0],SPIIndex,&SPIConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize device error!\n");
        return 0;
    }
    // JEDEC ID
    unsigned char WriteDataTemp[20*1024];
    unsigned char ReadDataTemp[20*1024];
    WriteDataTemp[0] = 0x9F;
    ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,3,0);
    if(ret != SPI_SUCCESS){
	    printf("Read flash ID error :%d\n",ret);
	    return ret;
    }else{
	    printf("Flash ID = 0x%06X\n",(ReadDataTemp[0]<<16)|(ReadDataTemp[1]<<8)|(ReadDataTemp[2]));
    }
    //return 0;
    // Write Enable
    //WriteDataTemp[0] = 0x06;
    //ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,1);


    WriteDataTemp[0] = 0x05;// Read Status Register-1
	ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,100);
    printf("STATUS1 = %02X \n",ReadDataTemp[0]);
    /*WriteDataTemp[0] = 0x35;// Read Status Register-1
	ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,100);
    printf("STATUS2 = %02X \n",ReadDataTemp[0]);*/
   /* return 0;
    WriteDataTemp[0] = 0x01;// Read Status Register-1
    WriteDataTemp[1] = 0x00;
    WriteDataTemp[2] = 0x00;
	//ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,3);
    Sleep(1000);

    WriteDataTemp[0] = 0x05;// Read Status Register-1
	ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,100);
    printf("STATUS1 = %02X \n",ReadDataTemp[0]);
    WriteDataTemp[0] = 0x35;// Read Status Register-1
	ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,100);
    printf("STATUS2 = %02X \n",ReadDataTemp[0]);
    // Write Enable
    WriteDataTemp[0] = 0x04;
    ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,1);*/
    //return 0;
    // Write Enable
    WriteDataTemp[0] = 0x06;
    ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,1);
    if(ret != SPI_SUCCESS){
	    printf("Flash write enable error :%d\n",ret);
	    return ret;
    }else{
	    printf("Write enable SPI_SUCCESS!\n");
    }
    // Sector Erase (4KB)
    WriteDataTemp[0] = 0x20;
    WriteDataTemp[1] = 0x00;
    WriteDataTemp[2] = 0x00;
    WriteDataTemp[3] = 0x00;
    ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,4);
    if(ret != SPI_SUCCESS){
	    printf("Sector Erase start error :%d\n",ret);
	    return ret;
    }else{
	    printf("Sector erase start SPI_SUCCESS!\n");
    }
    // Check the operation to complete
    do{
	    WriteDataTemp[0] = 0x05;// Read Status Register-1
	    ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,0);
        Sleep(500);
        printf("%02X ",ReadDataTemp[0]);
    }while((ReadDataTemp[0]&0x01)&&(ret == SPI_SUCCESS));
    if(ret != SPI_SUCCESS){
	    printf("Sector Erase error :%d\n",ret);
	    return ret;
    }else{
	    printf("Sector erase SPI_SUCCESS!\n");
    }
    // Write Enable
    WriteDataTemp[0] = 0x06;
    ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,1);
    if(ret != SPI_SUCCESS){
	    printf("Flash write enable error :%d\n",ret);
	    return ret;
    }else{
	    printf("Write enable SPI_SUCCESS!\n");
    }
    WriteDataTemp[0] = 0x05;// Read Status Register-1
	ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,100);
    printf("STATUS1 = %02X \n",ReadDataTemp[0]);
    // Page Program
    WriteDataTemp[0] = 0x02;// Page Program command
    WriteDataTemp[1] = 0x00;// Address
    WriteDataTemp[2] = 0x01;
    WriteDataTemp[3] = 0x00;
    for(i=4;i<(256+4);i++){
	    WriteDataTemp[i] = i-4;
    }
    ret = SPI_WriteBytes(DevHandle[0],SPIIndex,WriteDataTemp,256+4);
    if(ret != SPI_SUCCESS){
	    printf("Page program start error :%d\n",ret);
	    return ret;
    }else{
	    printf("Page program start SPI_SUCCESS!\n");
    }
    // Check the operation to complete
    do{
	    WriteDataTemp[0] = 0x05;// Read Status Register-1
	    ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,1,ReadDataTemp,1,0);
        Sleep(500);
        printf("%02X ",ReadDataTemp[0]);
    }while((ReadDataTemp[0]&0x01)&&(ret == SPI_SUCCESS));
    if(ret != SPI_SUCCESS){
	    printf("Page program error :%d\n",ret);
	    return ret;
    }else{
	    printf("Page program SPI_SUCCESS!\n");
    }
    // Read Data
    WriteDataTemp[0] = 0x03;//Read Data command
    WriteDataTemp[1] = 0x00;//address
    WriteDataTemp[2] = 0x01;
    WriteDataTemp[3] = 0x00;
    ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,4,ReadDataTemp,256,0);
    if(ret != SPI_SUCCESS){
	    printf("Read Data error :%d\n",ret);
	    return ret;
    }else{
	    printf("Read Data SPI_SUCCESS\n");
    }
    printf("Read Data:");
    for(i=0;i<256;i++){
        if((i%16)==0){
            printf("\n");
        }
        printf("%02X ",ReadDataTemp[i]);
    }
    printf("\n\n");
    return 0;
    //读取所有数据，测试读取数据速度
    // Start Read Data Speed Test
    printf("Start Read Data Speed Test...\n");
    // Write Data Speed Test
    LARGE_INTEGER litmp;
    LONGLONG StartTime,EndTime;
    double dfFreq;
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    //循环读取W25Q64芯片的所有数据，每次读取10K
    int addr;
    for(addr = 0;addr < 0x800000;addr += 20480){
        WriteDataTemp[0] = 0x0B;//Fast Read
        WriteDataTemp[1] = addr>>16;//address
        WriteDataTemp[2] = addr>>8;
        WriteDataTemp[3] = addr>>0;
        WriteDataTemp[4] = 0x00;
        ret = SPI_WriteReadBytes(DevHandle[0],SPIIndex,WriteDataTemp,5,ReadDataTemp,20480,0);
        if(ret != SPI_SUCCESS){
            printf("Read Data error :%d\n",ret);
            break;
        }
    }
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
    // Print the write data speed information
    printf("Read Data Numbers: %d MBytes\n",0x800000/(1024*1024));
    printf("Read Data Elapsed Time: %f\n",(EndTime-StartTime)/dfFreq);
    printf("Read Data Speed: %f MByte/s\n",((0x800000/(1024*1024)))/((EndTime-StartTime)/dfFreq));
    return 0;
#else
    //根据W25Q64配置相关参数
    SPI_FLASH_CONFIG SPIFlashConfig;
    SPIFlashConfig.CMD_EraseSector = 0x20;
    SPIFlashConfig.CMD_ReadData = 0x03;
    SPIFlashConfig.CMD_ReadFast = 0x0B;
    SPIFlashConfig.CMD_ReadID = 0x9F;
    SPIFlashConfig.CMD_ReadStatus = 0x05;
    SPIFlashConfig.CMD_WriteEnable = 0x06;
    SPIFlashConfig.CMD_WritePage = 0x02;
    SPIFlashConfig.CMD_EraseChip = 0xC7;
    SPIFlashConfig.EraseSectorAddressBytes = 3;
    SPIFlashConfig.ID_Length = 3;
    SPIFlashConfig.NumPages = 32768;
    SPIFlashConfig.PageSize = 256;
    SPIFlashConfig.ReadDataAddressBytes = 3;
    SPIFlashConfig.ReadFastAddressBytes = 3;
    SPIFlashConfig.SectorSize = 4096;
    SPIFlashConfig.WritePageAddressBytes = 3;
    ret = SPI_FlashInit(DevHandle[0],SPIIndex,50000000>>2,&SPIFlashConfig);
    if(ret != SPI_SUCCESS){
        printf("Initialize Device Error!\n");
        return 0;
    }
    //读取芯片ID
    ret = SPI_FlashReadID(DevHandle[0],SPIIndex,SPIFlashConfig.ID);
    if(ret != SPI_SUCCESS){
        printf("Get Device ID Error!\n");
        return 0;
    }else{
        printf("ID = ");
        for(int i=0;i<SPIFlashConfig.ID_Length;i++){
            printf("%02X",SPIFlashConfig.ID[i]);
        }
        printf("\n\n");
        if((SPIFlashConfig.ID[0] == 0xFF)&&(SPIFlashConfig.ID[1] == 0xFF)){
            return 0;
        }
    }
    //扇区擦除
    ret = SPI_FlashEraseSector(DevHandle[0],SPIIndex,0,1);
    if(ret != SPI_SUCCESS){
        printf("Erase Sector Error!\n");
        return 0;
    }

    uint8_t TestBuffer[20*1024];
    for(int i=0;i<200;i++){
        TestBuffer[i] = i;
    }
    ret = SPI_FlashWrite(DevHandle[0],SPIIndex,10,TestBuffer,200);
    if(ret != SPI_SUCCESS){
        printf("Flash Write Error!\n");
        return 0;
    }
    ret = SPI_FlashReadFast(DevHandle[0],SPIIndex,0,TestBuffer,256);
    if(ret != SPI_SUCCESS){
        printf("Flash Read Error!\n");
        return 0;
    }
    for(int i=0;i<256;i++){
        if((i%16)==0){
            printf("\n");
        }
        printf("%02X ",TestBuffer[i]);
    }
    printf("\n\n");
    //return 0;
    ret = SPI_FlashEraseSector(DevHandle[0],SPIIndex,0,1);
    if(ret != SPI_SUCCESS){
        printf("Erase Sector Error!\n");
        return 0;
    }
    for(int i=0;i<256;i++){
        TestBuffer[i] = i;
    }
    ret = SPI_FlashWrite(DevHandle[0],SPIIndex,20,TestBuffer,256);
    if(ret != SPI_SUCCESS){
        printf("Flash Write Error!\n");
        return 0;
    }
    ret = SPI_FlashReadFast(DevHandle[0],SPIIndex,0,TestBuffer,512);
    if(ret != SPI_SUCCESS){
        printf("Flash Read Error!\n");
        return 0;
    }
    for(int i=0;i<512;i++){
        if((i%16)==0){
            printf("\n");
        }
        printf("%02X ",TestBuffer[i]);
    }
    printf("\n\n");

   ret = SPI_FlashEraseSector(DevHandle[0],SPIIndex,0,1);
    if(ret != SPI_SUCCESS){
        printf("Erase Sector Error!\n");
        return 0;
    }
    for(int i=0;i<270;i++){
        TestBuffer[i] = i;
    }
    ret = SPI_FlashWrite(DevHandle[0],SPIIndex,250,TestBuffer,270);
    if(ret != SPI_SUCCESS){
        printf("Flash Write Error!\n");
        return 0;
    }
    ret = SPI_FlashReadFast(DevHandle[0],SPIIndex,0,TestBuffer,256*3);
    if(ret != SPI_SUCCESS){
        printf("Flash Read Error!\n");
        return 0;
    }
    for(int i=0;i<256*3;i++){
        if((i%16)==0){
            printf("\n");
        }
        printf("%02X ",TestBuffer[i]);
    }
    printf("\n\n");
    //准备计算时间
    LARGE_INTEGER litmp;
    LONGLONG StartTime,EndTime;
    double dfFreq;
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    //循环擦除整片数据，速度较慢
    /*for(int i=0;i<((SPIFlashConfig.NumPages*SPIFlashConfig.PageSize)/SPIFlashConfig.SectorSize);i++){
        ret = SPI_FlashEraseSector(DevHandle[0],SPIIndex,i,1);
        if(ret != SPI_SUCCESS){
            printf("Erase Sector Error!\n");
            return 0;
        }
    }*/
    //擦出整片数据，该函数擦出速度最快
    ret = SPI_FlashEraseChip(DevHandle[0],SPIIndex,30000);//超时参数要大于整片擦除时间
    if(ret != SPI_SUCCESS){
        printf("Erase Chip Error!\n");
        return 0;
    }
    //计算消耗的时间并算出速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
    // Print the write data speed information
    printf("擦出的扇区数： %d \n",(SPIFlashConfig.NumPages*SPIFlashConfig.PageSize)/SPIFlashConfig.SectorSize);
    printf("消耗时间： %.2f 秒\n",(EndTime-StartTime)/dfFreq);
    printf("\n");

    //循环写整片数据,每次写20KByte
    uint8_t WriteBuffer[20*1024];
    for(int i=0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i] = i;
    }
    //准备计算时间
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    //开始写数据
    for(int i=0;i<((SPIFlashConfig.NumPages*SPIFlashConfig.PageSize)/sizeof(WriteBuffer));i++){
        ret = SPI_FlashWrite(DevHandle[0],SPIIndex,i*sizeof(WriteBuffer),WriteBuffer,sizeof(WriteBuffer));
        if(ret != SPI_SUCCESS){
            printf("Flash Write Error!\n");
            return 0;
        }
    }
    //计算消耗的时间并算出速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
    // Print the write data speed information
    printf("写数据字节数：%d MBytes\n",(SPIFlashConfig.NumPages*SPIFlashConfig.PageSize)/(1024*1024));
    printf("写数据消耗时间： %.2f 秒\n",(EndTime-StartTime)/dfFreq);
    printf("写数据速度： %f MByte/s\n",((SPIFlashConfig.NumPages*SPIFlashConfig.PageSize/(1024*1024)))/((EndTime-StartTime)/dfFreq));
    printf("\n");
    //循环读取整片数据，每次读20KByte
    uint8_t ReadBuffer[20*1024];
    //准备计算时间
    QueryPerformanceFrequency(&litmp);// Get the performance counter frequency, in n/s
    dfFreq = (double)litmp.QuadPart;
    QueryPerformanceCounter(&litmp);  // Get the current value of the performance counter
    StartTime = litmp.QuadPart;       // Start time
    for(int i=0;i<((SPIFlashConfig.NumPages*SPIFlashConfig.PageSize)/sizeof(ReadBuffer));i++){
        ret = SPI_FlashReadFast(DevHandle[0],SPIIndex,i*sizeof(ReadBuffer),ReadBuffer,sizeof(ReadBuffer));
        if(ret != SPI_SUCCESS){
            printf("Flash Read Error!\n");
            return 0;
        }
    }
   //计算消耗的时间并算出速度
    QueryPerformanceCounter(&litmp);// Get the current value of the performance counter
    EndTime = litmp.QuadPart; // Stop time
    // Print the write data speed information
    printf("读数据字节数： %d MBytes\n",(SPIFlashConfig.NumPages*SPIFlashConfig.PageSize)/(1024*1024));
    printf("读数据消耗时间： %.2f 秒\n",(EndTime-StartTime)/dfFreq);
    printf("读数据速度： %f MByte/s\n",((SPIFlashConfig.NumPages*SPIFlashConfig.PageSize/(1024*1024)))/((EndTime-StartTime)/dfFreq));
    printf("\n");
    return 0;
#endif
}

