// USB2XXXNandTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "../../USB2XXX/source/usb2xxx/usb2nand.h"
#include "../../USB2XXX/source/usb_device.h"

int NAND_GetMemoryInfo(NAND_ID* pNandId,NAND_MEMORY_INFO *pNandMemInfo)
{
  int i=0;
  pNandMemInfo->BytesPerPage = 1;
  for(i=0;i<(pNandId->FourthID&0x03);i++){
    pNandMemInfo->BytesPerPage *= 2;
  }
  pNandMemInfo->BytesPerPage *= 1024;
  
  pNandMemInfo->PagePerBlock = 64;
  for(i=0;i<((pNandId->FourthID>>4)&0x03);i++){
    pNandMemInfo->PagePerBlock *= 2;
  }
  pNandMemInfo->PagePerBlock *= 1024;
  pNandMemInfo->PagePerBlock /= pNandMemInfo->BytesPerPage;
  
  pNandMemInfo->SpareAreaSize = (pNandMemInfo->BytesPerPage/512)*((((pNandId->FourthID>>2)&0x01)>0?16:8));
  
  pNandMemInfo->BlockPerPlane = 8;
  for(i=0;i<((pNandId->FiveID>>4)&0x07);i++){
    pNandMemInfo->BlockPerPlane *= 2;
  }
  pNandMemInfo->BlockPerPlane *= 1024;
  pNandMemInfo->BlockPerPlane /= (pNandMemInfo->BytesPerPage/1024)*pNandMemInfo->PagePerBlock;
  
  pNandMemInfo->PlanePerChip = 1;
  for(i=0;i<((pNandId->FiveID>>3)&0x03);i++){
    pNandMemInfo->PlanePerChip *= 2;
  }
  return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    DEVICE_INFO DevInfo;
    NAND_ID NandId;
    NAND_ADDRESS NAN_Address;
    int DevIndex = 0;
    int ChipIndex = 1;
    bool state;
    int ret;
    unsigned char WriteBuffer[10*2*1024];
    unsigned char ReadBuffer[10*2*1024];
    //扫描查找设备
    ret = USB_ScanDevice(true);
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
    state = USB_GetDeviceInfo(DevIndex,&DevInfo);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }
    Sleep(100);
    //初始化配置NAND控制器
    ret = NAND_Init(DevIndex,ChipIndex,2048,10);
    if(ret != SUCCESS){
        printf("Init error!\n");
        return 0;
    }
    //Sleep(1000);
    //获取设备ID
    ret = NAND_ReadID(DevIndex,ChipIndex,&NandId);
    if(ret != SUCCESS){
        printf("Get id error!\n");
        return 0;
    }else{
        printf("Get Chip ID:\n");
        printf("MakerID = EC = %02X\n",NandId.MakerID);
        printf("DeviceID = F1 = %02X\n",NandId.DeviceID);
        printf("ThirdID = 00 = %02X\n",NandId.ThirdID);
        printf("FourthID = 95 = %02X\n",NandId.FourthID);
        printf("FiveID = 40 = %02X\n",NandId.FiveID);
        printf("\n");
    }
    /*if((NandId.MakerID != 0xEC)||(NandId.DeviceID != 0xF1)||(NandId.ThirdID != 0x00)||(NandId.FourthID != 0x95)||(NandId.FiveID != 0x40)){
        return 0;
    }*/
    //通过ID获取存储信息
    NAND_MEMORY_INFO NandMemInfo;
    NAND_GetMemoryInfo(&NandId,&NandMemInfo);
    printf("NandMemInfo.PlanePerChip = %d\n\r",NandMemInfo.PlanePerChip);
     printf("NandMemInfo.BlockPerPlane = %d\n\r",NandMemInfo.BlockPerPlane);
    printf("NandMemInfo.PagePerBlock = %d\n\r",NandMemInfo.PagePerBlock);
    printf("NandMemInfo.BytesPerPage = %d\n\r",NandMemInfo.BytesPerPage);
    printf("NandMemInfo.SpareAreaSize = %d\n\r",NandMemInfo.SpareAreaSize);
    //设置存储信息
    ret = NAND_SetMemoryInfo(DevIndex,ChipIndex,&NandMemInfo);
    if(ret != SUCCESS){
        printf("Set mem info error!\n");
        return 0;
    }
    //return 0;
    //擦出芯片
    NAN_Address.Block = 0;
    NAN_Address.Page = 5;
    NAN_Address.Plane = 0;
    ret = NAND_EraseBlock(DevIndex,ChipIndex,&NAN_Address,1);
    if(ret != SUCCESS){
        printf("Erase block error!\n");
        return 0;
    }
    //写数据
    int PageNum = 10;
    unsigned int ecc[10];
   for(int i = 0;i<sizeof(WriteBuffer);i++){
        WriteBuffer[i]=i;
    }
    for(int i=0;i<PageNum;i+=1){
        ret = NAND_WritePage(DevIndex,ChipIndex,&NAN_Address,WriteBuffer,1,&ecc[i]);
        if(ret != SUCCESS){
            printf("Write page error!\n");
            return 0;
        }
        NAN_Address.Page += 1;
        //Sleep(10);
    }
    printf("WRITE ECC = ");
    for(int i=0;i<10;i++){
        printf("%08X ",ecc[i]);
    }
    printf("\n");
    //读数据
    NAN_Address.Page = 5;
    for(int i=0;i<PageNum;i+=1){
        ret = NAND_ReadPage(DevIndex,ChipIndex,&NAN_Address,ReadBuffer,1,&ecc[i]);
        if(ret != SUCCESS){
            printf("Read page error!\n");
            return 0;
        }
        NAN_Address.Page += 1;
        //Sleep(10);
    }
    printf("READ  ECC = ");
    for(int i=0;i<10;i++){
        printf("%08X ",ecc[i]);
    }
    printf("\n");
	return 0;
}

