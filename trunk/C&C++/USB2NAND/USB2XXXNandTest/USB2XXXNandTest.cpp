// USB2XXXNandTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "usb2nand.h"
#include "usb_device.h"
#include <time.h>  

#define	TEST_DATA_NUM	16*1024
#define TEST_BLOCK_ADDR	0x0

void NAND_Test(int devHandle)
{
	uint16_t index;
	int ret;
	int status = 0;
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
	NandMemInfo.BytesPerPage = 16384;
	NandMemInfo.PagePerBlock = 258;
	NandMemInfo.BlockPerPlane = 2048+60;
	NandMemInfo.PlanePerLUN = 2;
	NandMemInfo.LUNPerChip = 1;
	NandMemInfo.SpareAreaSize = 2048;

	NandTimeConfig.FSMC_HiZSetupTime = 10;	//比FSMC_SetupTime大一点即可
	NandTimeConfig.FSMC_HoldSetupTime = 10;	//ALE/CLE Hold time Min 3ns
	NandTimeConfig.FSMC_SetupTime = 10;		//ALE/CLE setup time Min 6ns
	NandTimeConfig.FSMC_WaitSetupTime = 10;	//WE/RE pulse width Min 8ns
	ret = NAND_Init(devHandle,0,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("nand init success!\n");
	}else{
		printf("nand init error!\n");
		return;
	}
	for(int k=0;k<1;k++){
		//读ID
		//AD 3A 18 A3 61 25
		uint8_t NandID[6];
		ret = NAND_ReadID(devHandle,0,0x00,NandID,6);
		if(ret == NAND_SUCCESS){
			printf("get nand id success!\n");
			printf("ID : ");
			for(int i=0;i<6;i++){
				printf("0x%02X ",NandID[i]);
			}
			printf("\n");
		}else{
			printf("get nand id error!\n");
			return;
		}
  
		if((NandID[0]!=0xAD)||(NandID[1]!=0x3A)||(NandID[2]!=0x18)||(NandID[3]!=0xA3)||(NandID[4]!=0x61)||(NandID[5]!=0x25)){
			printf("Get id error\n");
			return;
		}
	}
	//
	uint8_t cmd[]={0x36,0x16};
	uint8_t addr=0x70;
	uint8_t data = 0x00;
	//NAND_WritePage(devHandle,0,cmd,sizeof(cmd),&addr,1,&data,1,1000);
	addr=0x71;
	data = 0x01;
	//NAND_WritePage(devHandle,0,cmd,sizeof(cmd),&addr,1,&data,1,1000);

    //块擦除
    uint8_t BlockEraseCmd[]={0x60,0xD0};
    uint8_t EraseAddrs[]={0x00,TEST_BLOCK_ADDR,0x00};
    //status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),EraseAddrs,sizeof(EraseAddrs),1000);
    printf("erase status = 0x%08X\n",status);
	
	Sleep(100);
    //SLC页写数据
    uint8_t WriteTestData[3][TEST_DATA_NUM];
	uint8_t EmptyData[TEST_DATA_NUM];
	memset(EmptyData,0xFF,sizeof(EmptyData));
    srand((unsigned)time(NULL));
#if 1
	for(int j=0;j<3;j++){
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			WriteTestData[j][i] = rand();//((j<<4)|i)&0xFF;
		}
	}
#else
	for(int i=0;i<sizeof(WriteTestData[0]);i++){
		WriteTestData[1][i] = 0xFE;
	}
	for(int i=0;i<sizeof(WriteTestData[1]);i++){
		WriteTestData[2][i] = 0xEF;
	}
	for(int i=0;i<sizeof(WriteTestData[2]);i++){
		WriteTestData[0][i] = 0x7F;
	}
#endif
#if 0
    uint8_t PageWriteCmd[]={0xA2,0x80,0x10};
    uint8_t PageWriteAddrs[]={0x00,0x00,0x00,TEST_BLOCK_ADDR,0x00};
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
#elif 0
	//Order0
    uint8_t PageWriteCmd[]={0x09,0x01,0x80,0x1A};
    uint8_t PageWriteAddrs[]={0x00,0x00,0x00,TEST_BLOCK_ADDR,0x00};
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x00;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order1
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x01;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order2
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x00;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order3
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x02;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order4
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x01;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order5
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x00;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order6
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x03;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order7
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x02;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order8
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x01;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order9
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x04;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order10
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x03;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order11
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	PageWriteAddrs[2] = 0x02;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,PageWriteAddrs,sizeof(PageWriteAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
#endif
    Sleep(100);
	//SLC页读数据
    uint8_t ReadTestData[3][TEST_DATA_NUM+8];
    uint8_t PageReadCmd[]={0x17,0x04,0x19,0xDA,0x00,0x30};
    uint8_t PageReadAddrs[]={0x00,0x00,0x20,0x02,0x00};
	PageReadCmd[0] = 0x01;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),PageReadAddrs,sizeof(PageReadAddrs),ReadTestData[0],sizeof(ReadTestData[0]),1000);
    printf("page read status = 0x%08X\n",status);
	return;
	PageReadCmd[0] = 0x02;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),PageReadAddrs,sizeof(PageReadAddrs),ReadTestData[1],sizeof(ReadTestData[1]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x03;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),PageReadAddrs,sizeof(PageReadAddrs),ReadTestData[2],sizeof(ReadTestData[2]),1000);
    printf("page read status = 0x%08X\n",status);
    //对比数据
    int ErrorDataNum = 0;
	for(int j=0;j<3;j++){
		ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			if(WriteTestData[j][i] != ReadTestData[j][i]){
				//printf("%02X ",ReadTestData[j][i]);
				//printf("%d ",i);
				ErrorDataNum++;
			}
		}
		//printf("\n");
		printf("ErrorDataNum[%d] = %d\n",j,ErrorDataNum);
	}
	PageReadAddrs[2] = 0x01;
	PageReadCmd[0] = 0x01;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),PageReadAddrs,sizeof(PageReadAddrs),ReadTestData[0],sizeof(ReadTestData[0]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x02;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),PageReadAddrs,sizeof(PageReadAddrs),ReadTestData[1],sizeof(ReadTestData[1]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x03;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),PageReadAddrs,sizeof(PageReadAddrs),ReadTestData[2],sizeof(ReadTestData[2]),1000);
    printf("page read status = 0x%08X\n",status);
	for(int j=0;j<3;j++){
		ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			if(WriteTestData[j][i] != ReadTestData[j][i]){
				//printf("%02X ",ReadTestData[j][i]);
				ErrorDataNum++;
			}
		}
		//printf("\n");
		printf("ErrorDataNum[%d] = %d\n",j,ErrorDataNum);
	}
}

void NAND_TF_SLC_Test(int devHandle)
{
	int ret;
	int status = 0;
	NAND_TIMING_COMFIG NandTimeConfig;
	uint8_t TestAddrs[]={0x00,0x00,0x00,0x00,0x00};
	//初始化配置NAND
	NandTimeConfig.FSMC_HiZSetupTime = 4;	//比FSMC_SetupTime大一点即可
	NandTimeConfig.FSMC_HoldSetupTime = 4;	//ALE/CLE Hold time Min 3ns
	NandTimeConfig.FSMC_SetupTime = 4;		//ALE/CLE setup time Min 6ns
	NandTimeConfig.FSMC_WaitSetupTime = 6;	//WE/RE pulse width Min 8ns
	ret = NAND_Init(devHandle,0,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("nand init success!\n");
	}else{
		printf("nand init error!\n");
		return;
	}
	uint8_t reset[]={0xFF};
	NAND_SendCmd(devHandle,0,reset,1,0);
	//读ID，读取ID的时候会自动发送复位命令，复位后会默认为TLC模式，所以需要在进行其他操作时发送进入SLC模式的命令
	//AD 3A 18 A3 61 25
	uint8_t NandID[6];
	ret = NAND_ReadID(devHandle,0,0x00,NandID,6);
	if(ret == NAND_SUCCESS){
		printf("get nand id success!\n");
		printf("ID : ");
		for(int i=0;i<6;i++){
			printf("0x%02X ",NandID[i]);
		}
		printf("\n");
	}else{
		printf("get nand id error!\n");
		return;
	}
	if((NandID[0]!=0x45)||(NandID[1]!=0x4C)||(NandID[2]!=0x98)||(NandID[3]!=0xA3)||(NandID[4]!=0x76)||(NandID[5]!=0x51)){
		printf("ID error\n");
		return;
	}
	//发送进入SLC模式命令
	uint8_t cmd[16];
    //块擦除
    uint8_t BlockEraseCmd[]={0xA2,0x60,0xD0};//0xA2是进入SLC模式命令，所有操作都必须加这个命令才能正常读写
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("erase status = 0x%08X\n",status);
	TestAddrs[3] = 1;
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("erase status = 0x%08X\n",status);
	TestAddrs[3] = 2;
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("erase status = 0x%08X\n",status);
	srand((unsigned)time(NULL));
	for(int p=0;p<256;p++){
		//SLC页写数据
		uint8_t WriteTestData[TEST_DATA_NUM];
		for(int i=0;i<sizeof(WriteTestData);i++){
			WriteTestData[i] = rand();//((j<<4)|i)&0xFF;
		}
		uint8_t PageWriteCmd[]={0xA2,0x80,0x10};
		TestAddrs[2] = p%128;
		TestAddrs[3] = p/128;
		status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData,sizeof(WriteTestData),1000);
		//printf("TF SLC page write status = 0x%08X\n",status);
		//Sleep(10);
		//SLC页读数据
		uint8_t ReadTestData[TEST_DATA_NUM];
		uint8_t PageReadCmd[]={0xA2,0x00,0x30};
		NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,0,1000);
		PageReadCmd[0]=0x05;
		PageReadCmd[1]=0xE0;
		NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd)-1,TestAddrs,sizeof(TestAddrs),ReadTestData,sizeof(ReadTestData),1000);
		//printf("TF SLC page read status = 0x%08X\n",status);
		//对比数据
		int ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData);i++){
			if(WriteTestData[i] != ReadTestData[i]){
				ErrorDataNum++;
			}
		}
		if(ErrorDataNum > 100){
			PageReadCmd[0]=0xA2;
			PageReadCmd[1]=0x00;
			PageReadCmd[2]=0x30;
			NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,0,1000);
			PageReadCmd[0]=0x05;
			PageReadCmd[1]=0xE0;
			NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd)-1,TestAddrs,sizeof(TestAddrs),ReadTestData,sizeof(ReadTestData),1000);
			//printf("TF SLC page read status = 0x%08X\n",status);
			//对比数据
			ErrorDataNum = 0;
			for(int i=0;i<sizeof(WriteTestData);i++){
				if(WriteTestData[i] != ReadTestData[i]){
					ErrorDataNum++;
				}
			}
			printf("read secend\n");
		}
		printf("TF SLC ErrorDataNum[%02X%02X%02X%02X%02X][%02X] = %d\n",TestAddrs[4],TestAddrs[3],TestAddrs[2],TestAddrs[1],TestAddrs[0],status,ErrorDataNum);
	}
	return;
}

void NAND_SLC_Test(int devHandle)
{
	int ret;
	int status = 0;
	NAND_TIMING_COMFIG NandTimeConfig;
	uint8_t TestAddrs[]={0x00,0x00,0x00,TEST_BLOCK_ADDR,0x00};
	//初始化配置NAND
	NandTimeConfig.FSMC_HiZSetupTime = 1;	//比FSMC_SetupTime大一点即可
	NandTimeConfig.FSMC_HoldSetupTime = 1;	//ALE/CLE Hold time Min 3ns
	NandTimeConfig.FSMC_SetupTime = 1;		//ALE/CLE setup time Min 6ns
	NandTimeConfig.FSMC_WaitSetupTime = 5;	//WE/RE pulse width Min 8ns
	ret = NAND_Init(devHandle,0,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("nand init success!\n");
	}else{
		printf("nand init error!\n");
		return;
	}
	//发送进入SLC模式命令
	uint8_t cmd[16];
	cmd[0] = 0xFF;//进入SLC模式命令
	NAND_SendCmd(devHandle,0,cmd,1,0);
	cmd[0] = 0xBF;//进入SLC模式命令
	NAND_SendCmd(devHandle,0,cmd,1,0);
	//读ID，读取ID的时候会自动发送复位命令，复位后会默认为TLC模式，所以需要在进行其他操作时发送进入SLC模式的命令
	//AD 3A 18 A3 61 25
	uint8_t NandID[6];
	ret = NAND_ReadID(devHandle,0,0x00,NandID,6);
	if(ret == NAND_SUCCESS){
		printf("get nand id success!\n");
		printf("ID : ");
		for(int i=0;i<6;i++){
			printf("0x%02X ",NandID[i]);
		}
		printf("\n");
	}else{
		printf("get nand id error!\n");
		return;
	}
	if((NandID[0]!=0xAD)||(NandID[1]!=0x3A)||(NandID[2]!=0x18)||(NandID[3]!=0xA3)||(NandID[4]!=0x61)||(NandID[5]!=0x25)){
		printf("ID error\n");
		return;
	}
    //块擦除
    uint8_t BlockEraseCmd[]={0x60,0xD0};
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("erase status = 0x%08X\n",status);
	
    //SLC页写数据
    uint8_t WriteTestData[TEST_DATA_NUM*2];
    srand((unsigned)time(NULL));
	for(int i=0;i<sizeof(WriteTestData);i++){
		WriteTestData[i] = rand();//((j<<4)|i)&0xFF;
	}
    uint8_t PageWriteCmd[]={0x80,0x10};
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData,sizeof(WriteTestData)/2,1000);
    printf("SLC page write status = 0x%08X\n",status);
	TestAddrs[2]++;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),&WriteTestData[TEST_DATA_NUM],sizeof(WriteTestData)/2,1000);
    printf("SLC page write status = 0x%08X\n",status);
	//SLC页读数据
    uint8_t ReadTestData[TEST_DATA_NUM*2];
    uint8_t PageReadCmd[]={0x00,0x30};
	TestAddrs[2]--;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,sizeof(ReadTestData)/2,1000);
    printf("SLC page read status = 0x%08X\n",status);
	TestAddrs[2]++;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),&ReadTestData[TEST_DATA_NUM],sizeof(ReadTestData)/2,1000);
    printf("SLC page read status = 0x%08X\n",status);
    //对比数据
    int ErrorDataNum = 0;
	for(int i=0;i<sizeof(WriteTestData);i++){
		if(WriteTestData[i] != ReadTestData[i]){
			ErrorDataNum++;
		}
	}
	printf("SLC ErrorDataNum = %d\n",ErrorDataNum);
	return;
}

void NAND_TLC_Test2(int devHandle)
{
	int ret;
	int status = 0;
	NAND_TIMING_COMFIG NandTimeConfig;
	uint8_t TestAddrs[]={0x00,0x00,0x00,TEST_BLOCK_ADDR,0x00};
	//初始化配置NAND
	NandTimeConfig.FSMC_HiZSetupTime = 1;	//比FSMC_SetupTime大一点即可
	NandTimeConfig.FSMC_HoldSetupTime = 1;	//ALE/CLE Hold time Min 3ns
	NandTimeConfig.FSMC_SetupTime = 1;		//ALE/CLE setup time Min 6ns
	NandTimeConfig.FSMC_WaitSetupTime = 5;	//WE/RE pulse width Min 8ns
	ret = NAND_Init(devHandle,0,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("nand init success!\n");
	}else{
		printf("nand init error!\n");
		return;
	}
	//发送进入SLC模式命令
	uint8_t cmd[16];
	cmd[0] = 0xFF;//进入TLC模式命令
	NAND_SendCmd(devHandle,0,cmd,1,0);
	//cmd[0] = 0xBF;//进入SLC模式命令
	//NAND_SendCmd(devHandle,0,cmd,1,0);
	//读ID，读取ID的时候会自动发送复位命令，复位后会默认为TLC模式，所以需要在进行其他操作时发送进入SLC模式的命令
	//AD 3A 18 A3 61 25
	uint8_t NandID[6];
	ret = NAND_ReadID(devHandle,0,0x00,NandID,6);
	if(ret == NAND_SUCCESS){
		printf("get nand id success!\n");
		printf("ID : ");
		for(int i=0;i<6;i++){
			printf("0x%02X ",NandID[i]);
		}
		printf("\n");
	}else{
		printf("get nand id error!\n");
		return;
	}
	if((NandID[0]!=0x98)||(NandID[1]!=0x3c)||(NandID[2]!=0x98)||(NandID[3]!=0xb3)||(NandID[4]!=0x76)||(NandID[5]!=0x71)){
		printf("ID error\n");
		//return;
	}
    //块擦除
    uint8_t BlockEraseCmd[]={0x60,0xD0};
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("erase status = 0x%08X\n",status);
	
    //SLC页写数据
    uint8_t WriteTestData[TEST_DATA_NUM*3];
    srand((unsigned)time(NULL));
	for(int i=0;i<sizeof(WriteTestData);i++){
		WriteTestData[i] = rand();//((j<<4)|i)&0xFF;
	}
    uint8_t PageWriteCmd[]={0x09,0x01,0x80,0x11};
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData,TEST_DATA_NUM,1000);
    printf("SLC page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0X0D;
	PageWriteCmd[1] = 0X02;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),&WriteTestData[TEST_DATA_NUM],TEST_DATA_NUM,1000);
    printf("SLC page write status = 0x%08X\n",status);
	PageWriteCmd[1] = 0X03;
	PageWriteCmd[3] = 0X10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),&WriteTestData[TEST_DATA_NUM*2],TEST_DATA_NUM,1000);
    printf("SLC page write status = 0x%08X\n",status);
	//TestAddrs[2]++;
    //status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),&WriteTestData[TEST_DATA_NUM],sizeof(WriteTestData)/2,1000);
    //printf("SLC page write status = 0x%08X\n",status);
	//SLC页读数据
    uint8_t ReadTestData[TEST_DATA_NUM*3];
    uint8_t PageReadCmd[]={0X01,0x00,0x30};
	status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,0,1000);
	PageReadCmd[0] = 0X05;
	PageReadCmd[1] = 0Xe0;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,2,TestAddrs,sizeof(TestAddrs),ReadTestData,TEST_DATA_NUM,1000);
    printf("SLC page read status = 0x%08X\n",status);

	PageReadCmd[0]=0x02;PageReadCmd[1]=0x00;PageReadCmd[2]=0x30;
	status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,0,1000);
	PageReadCmd[0] = 0X05;
	PageReadCmd[1] = 0Xe0;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,2,TestAddrs,sizeof(TestAddrs),&ReadTestData[TEST_DATA_NUM],TEST_DATA_NUM,1000);
    printf("SLC page read status = 0x%08X\n",status);

	PageReadCmd[0]=0x03;PageReadCmd[1]=0x00;PageReadCmd[2]=0x30;
	status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,0,1000);
	PageReadCmd[0] = 0X05;
	PageReadCmd[1] = 0Xe0;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,2,TestAddrs,sizeof(TestAddrs),&ReadTestData[TEST_DATA_NUM*2],TEST_DATA_NUM,1000);
    printf("SLC page read status = 0x%08X\n",status);
    //对比数据
    int ErrorDataNum = 0;
	for(int i=0;i<sizeof(WriteTestData);i++){
		if(WriteTestData[i] != ReadTestData[i]){
			ErrorDataNum++;
		}
	}
	printf("SLC ErrorDataNum = %d\n",ErrorDataNum);
	return;
}

void NAND_MLC_Test(int devHandle)
{
	int ret;
	int status = 0;
	NAND_TIMING_COMFIG NandTimeConfig;
	uint8_t TestAddrs[]={0x00,0x00,0x00,TEST_BLOCK_ADDR,0x00};
	//初始化配置NAND
	NandTimeConfig.FSMC_HiZSetupTime = 5;	//比FSMC_SetupTime大一点即可
	NandTimeConfig.FSMC_HoldSetupTime = 5;	//ALE/CLE Hold time Min 3ns
	NandTimeConfig.FSMC_SetupTime = 5;		//ALE/CLE setup time Min 6ns
	NandTimeConfig.FSMC_WaitSetupTime = 5;	//WE/RE pulse width Min 8ns
	ret = NAND_Init(devHandle,0,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("nand init success!\n");
	}else{
		printf("nand init error!\n");
		return;
	}
	//发送一些特定指令后才能正常操作
	uint8_t cmd[16];
	uint8_t addr[16];
	uint8_t data[16];
	uint8_t GetIDCmd[]={0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};
	for(int k=0;k<sizeof(GetIDCmd);k++){
		cmd[0] = 0xA3;
		NAND_SendCmd(devHandle,0,cmd,1,0);
		addr[0] = 0xAC;addr[1] = 0x41;addr[2] = 0xFB;addr[3] = 0x50;addr[4] = 0x7C;addr[5] = 0x54;addr[6] = 0x3B;addr[7] = 0xFE;
		NAND_SendAddr(devHandle,0,addr,8,0);
		data[0] = GetIDCmd[k];data[1] = GetIDCmd[k];data[2] = 0x60;
		ret = NAND_WriteData(devHandle,0,data,3,0);
		cmd[0] = 0x19;
		NAND_SendCmd(devHandle,0,cmd,1,0);
		data[0] = 0x00;
		NAND_WriteData(devHandle,0,data,1,0);
	
		cmd[0] = 0x00;
		NAND_SendCmd(devHandle,0,cmd,1,0);
		addr[0] = 0x00;addr[1] = 0x00;addr[2] = 0x00;addr[3] = 0x00;addr[4] = 0x00;
		NAND_SendAddr(devHandle,0,addr,5,0);
		cmd[0] = 0x5C;cmd[1] = 0xC5;cmd[2] = 0x55;
		NAND_SendCmd(devHandle,0,cmd,3,0);
		addr[0] = 0x00;
		NAND_SendAddr(devHandle,0,addr,1,0);
		data[0] = 0x01;
		NAND_WriteData(devHandle,0,data,1,0);
		cmd[0] = 0xFD;cmd[1] = 0x00;
		NAND_SendCmd(devHandle,0,cmd,2,0);
		addr[0] = 0x00;addr[1] = 0x00;addr[2] = 0x00;addr[3] = 0x00;addr[4] = 0x00;
		NAND_SendAddr(devHandle,0,addr,5,0);
		
		cmd[0] = 0x70;
		NAND_SendCmd(devHandle,0,cmd,1,0);
		NAND_ReadData(devHandle,0,data,1,0);
		printf("STATUS : 0x%02X\n",data[0]);
	
		cmd[0] = 0xFF;
		NAND_SendCmd(devHandle,0,cmd,1,0);
		cmd[0] = 0x90;
		NAND_SendCmd(devHandle,0,cmd,1,0);
		addr[0] = 0x00;
		NAND_SendAddr(devHandle,0,addr,1,0);
		NAND_ReadData(devHandle,0,data,14,0);
		printf("ID : ");
		for(int i=0;i<14;i++){
			printf("0x%02X ",data[i]);
		}
		printf("\n");
		if((data[0]==0x45)||(data[1]==0xDE)||(data[2]==0x94)||(data[3]==0x93)||(data[4]==0x76)||(data[5]==0x57)){
			break;
		}
		//Sleep(1);
	}
	//NAND_WritePage(devHandle,0,cmd,sizeof(cmd),&addr,1,&data,1,1000);
	//读ID，读取ID的时候会自动发送复位命令，复位后会默认为TLC模式，所以需要在进行其他操作时发送进入SLC模式的命令
	//E0 45 DE 94 93 76 57 09
	uint8_t NandID[6];
	ret = NAND_ReadID(devHandle,0,0x00,NandID,6);
	if(ret == NAND_SUCCESS){
		printf("get nand id success!\n");
		printf("ID : ");
		for(int i=0;i<6;i++){
			printf("0x%02X ",NandID[i]);
		}
		printf("\n");
	}else{
		printf("get nand id error!\n");
		return;
	}
	if((data[0]!=0x45)||(data[1]!=0xDE)||(data[2]!=0x94)||(data[3]!=0x93)||(data[4]!=0x76)||(data[5]!=0x57)){
		printf("ID error\n");
		return;
	}
	//发送进入SLC模式命令
	//cmd[0] = 0xBF;//进入SLC模式命令
	//NAND_SendCmd(devHandle,0,cmd,1);
    //块擦除
    uint8_t BlockEraseCmd[]={0x60,0xD0};
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("erase status = 0x%08X\n",status);
	/*uint64_t FileSize = 26024;
	int BlockBytes = 4194304;
	printf("test = %d\n",FileSize%BlockBytes);*/
    //SLC页写数据
    uint8_t WriteTestData[TEST_DATA_NUM*2];
    srand((unsigned)time(NULL));
	for(int i=0;i<sizeof(WriteTestData);i++){
		WriteTestData[i] = rand();//((j<<4)|i)&0xFF;
	}
    uint8_t PageWriteCmd[]={0x80,0x10};
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData,sizeof(WriteTestData)/2,1000);
    printf("MLC page write status = 0x%08X\n",status);
	TestAddrs[2]++;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),&WriteTestData[TEST_DATA_NUM],sizeof(WriteTestData)/2,1000);
    printf("MLC page write status = 0x%08X\n",status);
	//SLC页读数据
    uint8_t ReadTestData[TEST_DATA_NUM*2];
    uint8_t PageReadCmd[]={0x00,0x30};
	TestAddrs[2]--;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData,sizeof(ReadTestData)/2,1000);
    printf("MLC page read status = 0x%08X\n",status);
	TestAddrs[2]++;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),&ReadTestData[TEST_DATA_NUM],sizeof(ReadTestData)/2,1000);
    printf("MLC page read status = 0x%08X\n",status);
    //对比数据
    int ErrorDataNum = 0;
	for(int i=0;i<sizeof(WriteTestData);i++){
		if(WriteTestData[i] != ReadTestData[i]){
			ErrorDataNum++;
		}
	}
	printf("MLC ErrorDataNum = %d\n",ErrorDataNum);
	return;
}

void NAND_TLC_Test(int devHandle)
{
	int ret;
	int status = 0;
	uint8_t TestAddrs[]={0x00,0x00,0x00,0x00,0x00};

	NAND_TIMING_COMFIG NandTimeConfig;
	NandTimeConfig.FSMC_HiZSetupTime = 4;	//比FSMC_SetupTime大一点即可
	NandTimeConfig.FSMC_HoldSetupTime = 4;	//ALE/CLE Hold time Min 3ns
	NandTimeConfig.FSMC_SetupTime = 4;		//ALE/CLE setup time Min 6ns
	NandTimeConfig.FSMC_WaitSetupTime = 8;	//WE/RE pulse width Min 8ns
	ret = NAND_Init(devHandle,0,&NandTimeConfig);
	if(ret == NAND_SUCCESS){
		printf("TLC nand init success!\n");
	}else{
		printf("TLC nand init error!\n");
		return;
	}
#if 1
	//复位
	uint8_t cmd[16];
	uint8_t addr[16];
	uint8_t write_data[16*1024];
	uint8_t read_data[16*1024];
	uint8_t ReadyFlag = 0;
	//进入TLC模式
	cmd[0] = 0xFF;
	NAND_SendCmd(devHandle,0,cmd,1,0);
#endif
	//读ID
	//AD 3A 18 A3 61 25
	uint8_t NandID[6];
	ret = NAND_ReadID(devHandle,0,0x00,NandID,6);
	if(ret == NAND_SUCCESS){
		printf("get nand id success!\n");
		printf("ID : ");
		for(int i=0;i<6;i++){
			printf("0x%02X ",NandID[i]);
		}
		printf("\n");
	}else{
		printf("get nand id error!\n");
		return;
	}
  
	if((NandID[0]!=0xAD)||(NandID[1]!=0x3A)||(NandID[2]!=0x18)||(NandID[3]!=0xA3)||(NandID[4]!=0x61)||(NandID[5]!=0x25)){
		printf("ID error\n");
		//return;
	}

    //块擦除
    uint8_t BlockEraseCmd[]={0x60,0xD0};
    status = NAND_EraseBlock(devHandle,0,BlockEraseCmd,sizeof(BlockEraseCmd),&TestAddrs[2],3,1000);
    printf("TLC erase status = 0x%08X\n",status);
	
    //TLC页写数据
    uint8_t WriteTestData[3][TEST_DATA_NUM];
    srand((unsigned)time(NULL));
#if 1
	for(int j=0;j<3;j++){
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			WriteTestData[j][i] = rand();//((j<<4)|i)&0xFF;
		}
	}
#else
	for(int i=0;i<sizeof(WriteTestData[0]);i++){
		WriteTestData[0][i] = rand();//((j<<4)|i)&0xFF;
	}
	for(int j=0;j<2;j++){
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			WriteTestData[j+1][i] = WriteTestData[0][i];//((j<<4)|i)&0xFF;
		}
	}
#endif
	for(int p=0;p<258;p++){
		uint8_t order_cmds1[3]={0x09,0x0D,0x00};//最后一个命令不发
		uint8_t order_cmds2[3]={0x1A,0x1A,0x10};//最后一个命令不发
		uint8_t PageWriteCmd[]={0x09,0x01,0x80,0x1A};
		if(p==0){
			TestAddrs[2] = 0;
			for(int k=0;k<3;k++){
				PageWriteCmd[0] = 0x09;
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				//printf("page write status = 0x%08X\n",status);
			}
		}else if(p==1){
			TestAddrs[2] = 1;
			for(int k=0;k<3;k++){
				PageWriteCmd[0] = 0x09;
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				//printf("page write status = 0x%08X\n",status);
			}
		}else if(p==2){
			TestAddrs[2] = 0;
			for(int k=0;k<3;k++){
				PageWriteCmd[0]=0x0D;
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				//printf("page write status = 0x%08X\n",status);
			}
		}else if(p==255){
			TestAddrs[2] = 85;
			for(int k=0;k<3;k++){
				PageWriteCmd[0]=0x0D;
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				//printf("page write status = 0x%08X\n",status);
			}
		}else if(p==256){
			TestAddrs[2] = 84;
			for(int k=0;k<3;k++){
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				//printf("page write status = 0x%08X\n",status);
			}
		}else if(p==257){
			TestAddrs[2] = 85;
			for(int k=0;k<3;k++){
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				//printf("page write status = 0x%08X\n",status);
			}
		}else{
			static int e = 0;
			if((p%3)==0){
				e = ((p*4)/3)+1;
			}
			TestAddrs[2] = e-p;
			printf("Write Addr = 0x%02X%02X%02X%02X%02X\n",TestAddrs[4],TestAddrs[3],TestAddrs[2],TestAddrs[1],TestAddrs[0]);
			for(int k=0;k<3;k++){
				PageWriteCmd[0] = order_cmds1[p%3];
				PageWriteCmd[1] = k+1;
				PageWriteCmd[2] = 0x80;
				PageWriteCmd[3] = order_cmds2[k];
				if(PageWriteCmd[0] == 0x00){
					status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				}else{
					status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[k],sizeof(WriteTestData[k]),1000);
				}
				//printf("page write status = 0x%08X\n",status);
			}
		}
	}
#if 0
	//0
    uint8_t PageWriteCmd[]={0x09,0x01,0x80,0x1A};
	uint8_t StartAddr = TestAddrs[2];
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	//1
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	//2
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);

	//3
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	TestAddrs[2] = 1;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	//4
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	//5
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	
	//Order2
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	TestAddrs[2] = 0;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);

	//Order3
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	TestAddrs[2] = 2;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);

	//Order4
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	TestAddrs[2] = 1;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order5
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	TestAddrs[2] = 0;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order6
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	StartAddr += 3;
	TestAddrs[2] = StartAddr;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order7
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	StartAddr -= 1;
	TestAddrs[2] = StartAddr;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order8
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	StartAddr -= 1;
	TestAddrs[2] = StartAddr;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order9
	PageWriteCmd[0] = 0x09;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	StartAddr += 3;
	TestAddrs[2] = StartAddr;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x09;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order10
	PageWriteCmd[0] = 0x0D;
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	StartAddr -= 1;
	TestAddrs[2] = StartAddr;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
	PageWriteCmd[0] = 0x0D;
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,PageWriteCmd,sizeof(PageWriteCmd),TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
	//Order11
	PageWriteCmd[1] = 0x01;
	PageWriteCmd[3] = 0x1A;
	StartAddr -= 1;
	TestAddrs[2] = StartAddr;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[0],sizeof(WriteTestData[0]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x02;
	PageWriteCmd[3] = 0x1A;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[1],sizeof(WriteTestData[1]),1000);
    printf("page write status = 0x%08X\n",status);
    PageWriteCmd[1] = 0x03;
    PageWriteCmd[3] = 0x10;
    status = NAND_WritePage(devHandle,0,&PageWriteCmd[1],sizeof(PageWriteCmd)-1,TestAddrs,sizeof(TestAddrs),WriteTestData[2],sizeof(WriteTestData[2]),1000);
    printf("page write status = 0x%08X\n",status);
#endif
    Sleep(100);
	//SLC页读数据
    uint8_t ReadTestData[3][TEST_DATA_NUM];
    uint8_t PageReadCmd[]={0x01,0x00,0x30};
	PageReadCmd[0] = 0x01;
	TestAddrs[2] = 0x00;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[0],sizeof(ReadTestData[0]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x02;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[1],sizeof(ReadTestData[1]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x03;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[2],sizeof(ReadTestData[2]),1000);
    printf("page read status = 0x%08X\n",status);
    //对比数据
    int ErrorDataNum = 0;
	for(int j=0;j<3;j++){
		ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			if(WriteTestData[j][i] != ReadTestData[j][i]){
				//printf("%02X ",ReadTestData[j][i]);
				//printf("%d ",i);
				ErrorDataNum++;
			}
		}
		//printf("\n");
		printf("ErrorDataNum[%d] = %d\n",j,ErrorDataNum);
	}
	TestAddrs[2] = 0x01;
	PageReadCmd[0] = 0x01;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[0],sizeof(ReadTestData[0]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x02;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[1],sizeof(ReadTestData[1]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x03;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[2],sizeof(ReadTestData[2]),1000);
    printf("page read status = 0x%08X\n",status);
	for(int j=0;j<3;j++){
		ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			if(WriteTestData[j][i] != ReadTestData[j][i]){
				//printf("%02X ",ReadTestData[j][i]);
				ErrorDataNum++;
			}
		}
		//printf("\n");
		printf("ErrorDataNum[%d] = %d\n",j,ErrorDataNum);
	}
	TestAddrs[2] = 0x02;
	PageReadCmd[0] = 0x01;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[0],sizeof(ReadTestData[0]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x02;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[1],sizeof(ReadTestData[1]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x03;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[2],sizeof(ReadTestData[2]),1000);
    printf("page read status = 0x%08X\n",status);
	for(int j=0;j<3;j++){
		ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			if(WriteTestData[j][i] != ReadTestData[j][i]){
				//printf("%02X ",ReadTestData[j][i]);
				ErrorDataNum++;
			}
		}
		//printf("\n");
		printf("ErrorDataNum[%d] = %d\n",j,ErrorDataNum);
	}
	TestAddrs[2] = 0x03;
	PageReadCmd[0] = 0x01;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[0],sizeof(ReadTestData[0]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x02;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[1],sizeof(ReadTestData[1]),1000);
    printf("page read status = 0x%08X\n",status);
	PageReadCmd[0] = 0x03;
    status = NAND_ReadPage(devHandle,0,PageReadCmd,sizeof(PageReadCmd),TestAddrs,sizeof(TestAddrs),ReadTestData[2],sizeof(ReadTestData[2]),1000);
    printf("page read status = 0x%08X\n",status);
	for(int j=0;j<3;j++){
		ErrorDataNum = 0;
		for(int i=0;i<sizeof(WriteTestData[j]);i++){
			if(WriteTestData[j][i] != ReadTestData[j][i]){
				//printf("%02X ",ReadTestData[j][i]);
				ErrorDataNum++;
			}
		}
		//printf("\n");
		printf("ErrorDataNum[%d] = %d\n",j,ErrorDataNum);
	}
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
	//NAND_Test2(DeviceHandle[0]);
	//NAND_SLC_Test(DeviceHandle[0]);
	//NAND_TLC_Test(DeviceHandle[0]);
	//NAND_MLC_Test(DeviceHandle[0]);
	//NAND_TF_SLC_Test(DeviceHandle[0]);
	NAND_TLC_Test2(DeviceHandle[0]);
	return 0;
}

