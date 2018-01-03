// USB2XXXNandTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "usb2nand.h"
#include "usb_device.h"

void NAND_Test(int devHandle)
{
	uint16_t index;
	int ret;
	NAND_TIMING_COMFIG NandTimeConfig;
	NAND_MEMORY_INFO NandMemInfo;
	NAND_FLASH_CONFIG NandConfig;
	static uint16_t i;
	static uint32_t WriteEcc[10],ReadEcc[10];
	static uint8_t BadBlockFlag[1024];
	static uint8_t WriteBuffer[8192];
	static uint8_t ReadBuffer[8192];
	static uint32_t Status;
	uint8_t Channel = 0;
	//  NAND_MEMORY_INFO NandMemInfo;
	NAND_ADDRESS WriteReadAddr;
	//ÉèÖÃ´æ´¢Æ÷ÐÅÏ¢
	NandMemInfo.BytesPerPage = 8192;
	NandMemInfo.PagePerBlock = 256;
	NandMemInfo.BlockPerPlane = 2048;
	NandMemInfo.PlanePerLUN = 2;
	NandMemInfo.LUNPerChip = 1;
	NandMemInfo.SpareAreaSize = 448;
	//Erase
	NandConfig.CMD_EraseBlock[0] = 0x60;
	NandConfig.CMD_EraseBlock[1] = 0xD0;
	//Page Program
	NandConfig.CMD_WritePage[0] = 0x80;
	NandConfig.CMD_WritePage[1] = 0x10;
	//Page Read
	NandConfig.CMD_ReadPage[0] = 0x00;
	NandConfig.CMD_ReadPage[1] = 0x30;
	//Read Status
	NandConfig.CMD_ReadStatus = 0x70;
	//FSMC_NANDDeInit(FSMC_Bank2_NAND);
	//FSMC_NAND_Init(Channel,NandMemInfo[Channel].BytesPerPage,10);
  
	NandConfig.CMD_Reset = 0xFF;

	NandConfig.CMD_ReadID = 0x90;
	NandConfig.CMD_ReadIDAddr = 0x00;
	NandConfig.ID_Length = 6;

	NandTimeConfig.FSMC_HiZSetupTime = 10;
	NandTimeConfig.FSMC_HoldSetupTime = 10;
	NandTimeConfig.FSMC_SetupTime = 10;
	NandTimeConfig.FSMC_WaitSetupTime = 10;
	ret = NAND_Init(devHandle,0,NandMemInfo.BytesPerPage,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("nand init success!\n");
	}else{
		printf("nand init error!\n");
		return;
	}
	ret = NAND_SetChipInfo(devHandle,0,&NandConfig,&NandMemInfo);
	if(ret == NAND_SUCCESS){
		printf("set nand info success!\n");
	}else{
		printf("set nand info error!\n");
		return;
	}
	uint8_t NandID[6];
	ret = NAND_ReadID(devHandle,0,NandID);
	if(ret == NAND_SUCCESS){
		printf("get nand id success!\n");
		printf("ID : ");
		for(int i=0;i<NandConfig.ID_Length;i++){
			printf("0x%02X ",NandID[i]);
		}
		printf("\n");
	}else{
		printf("get nand id error!\n");
		return;
	}
  

//    
//    //FSMC_NAND_WritePage(Channel,WriteReadAddr,WriteBuffer,NandMemInfo[Channel].BytesPerPage,WriteEcc);
//    
//    //FSMC_NAND_ReadPage (Channel, WriteReadAddr,ReadBuffer,NandMemInfo[Channel].BytesPerPage,ReadEcc);
//    
//    for(i=0;i<NandMemInfo[Channel].BytesPerPage;i++){
//      if(WriteBuffer[i] != ReadBuffer[i]){
//        while(1);
//      }
//    }
//    //FSMC_NAND_WriteSpareArea(Channel,WriteReadAddr,WriteBuffer,256);
//    //FSMC_NAND_ReadSpareArea(Channel,WriteReadAddr,ReadBuffer,256);
//  }
//  while(1);



  /* Write data to FSMC NOR memory */
  /* Fill the buffer to send */
//  srand(56);
//  for (index = 0; index < NAND_PAGE_SIZE; index++ ){
//     TxBuffer[index] = rand();
//  }

//  FSMC_NAND_WritePage(TxBuffer, WriteReadAddr, 1,&ecc);
//  printf("\r\nWritten to the number of:\r\n");
//  for(j = 0; j < 128; j++){
//    printf("%02X ",TxBuffer[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }

//  /* Read back the written data */
//  FSMC_NAND_ReadPage (RxBuffer, WriteReadAddr, 1,&ecc);
//  printf("\r\nRead several:\r\n");
//  for(j = 0; j < 128; j++){
//    printf("%02X ",RxBuffer[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }  
//  WriteReadAddr.Block = 2;
//  WriteReadAddr.Page = 0;
//  WriteReadAddr.Plane = 0;
//  FSMC_NAND_WriteSpareArea(TxBuffer, WriteReadAddr, 2);
//  printf("\r\nWritten To The Spare Area:\r\n");
//  for(j = 0; j < NAND_SPARE_AREA_SIZE*2; j++){
//    printf("%02X ",TxBuffer[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }
//  /* Read back the written data */
//  FSMC_NAND_ReadSpareArea (RxBuffer, WriteReadAddr, 2);
//  printf("\r\nRead Spare Area:\r\n");
//  for(j = 0; j < NAND_SPARE_AREA_SIZE*2; j++){
//    printf("%02X ",RxBuffer[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }
//  printf("\r\nCheck Bad Block:\r\n");
//  FSMC_NAND_CheckBadBlock(BadBlockFlag, WriteReadAddr, 1024);
//  printf("\r\nBad Blocks(Which block flag is 1):\r\n");
//  for(j=0;j<1024;j++){
//    printf("%d ",BadBlockFlag[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }
  
//  printf("\r\nGet Bad Block:\r\n");
//  WriteReadAddr.Block = 0;
//  WriteReadAddr.Page = 0;
//  FSMC_NAND_GetBadBlock(BadBlockFlag,WriteReadAddr,1024);
//  for(j=0;j<1024;j++){
//    printf("%d ",BadBlockFlag[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }
//  FSMC_NAND_WriteSpareArea(TxBuffer, WriteReadAddr, 1);
//  printf("\r\nWritten To The Spare Area:\r\n");
//  for(j = 0; j < NAND_SPARE_AREA_SIZE; j++){
//    printf("%02X ",TxBuffer[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }
//  /* Read back the written data */
//  FSMC_NAND_ReadSpareArea (RxBuffer, WriteReadAddr, 1);
//  printf("\r\nRead Spare Area:\r\n");
//  for(j = 0; j < NAND_SPARE_AREA_SIZE; j++){
//    printf("%02X ",RxBuffer[j]);
//    if((((j+1)%32)==0)){
//      printf("\n\r");
//    }
//  }    
}

int _tmain(int argc, _TCHAR* argv[])
{
	int DeviceHandle[20];
    int DevIndex = 0;
    int ChipIndex = 0;
    bool state;
    int ret;
    unsigned char WriteBuffer[10*2*1024];
    unsigned char ReadBuffer[10*2*1024];
    //扫描查找设备
    ret = USB_ScanDevice(DeviceHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //打开设备
    state = USB_OpenDevice(DeviceHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    Sleep(100);
	NAND_Test(DeviceHandle[0]);
	return 0;
}

