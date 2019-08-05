/**
  ******************************************************************************
  * @file    usb2nand.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   usb2nand相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.usbxyz.com">http://www.usbxyz.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB2NAND_H_
#define __USB2NAND_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

//定义NAND的地址
typedef struct _NAND_ADDRESS
{
  unsigned short Block;     //块地址
  unsigned short Page;      //页地址
  unsigned short Column;    //在每个Page中的偏移地址
}NAND_ADDRESS,*PNAND_ADDRESS;

//定义NAND存储器组织形式
typedef struct _NAND_MEMORY_INFO
{
  unsigned int LUNPerChip;        //LUN number(LUN)，芯片上的LUN数
  unsigned int PlanePerLUN;       //Plane number(Plane)，每个LUN上的Plane数
  unsigned int BlockPerPlane;     //Plane size(Block),每个Plane上的Block数
  unsigned int PagePerBlock;      //Block size(Page)，每个Block上的Page数
  unsigned int BytesPerPage;      //Page size(Byte),每个Page上的字节数
  unsigned int SpareAreaSize;     //Spare area size(Byte)，每个Page上Spare Area字节数
}NAND_MEMORY_INFO,*PNAND_MEMORY_INFO;

//定义NAND Flash器件配置参数数据类型
typedef struct _NAND_FLASH_CONFIG{
    unsigned char CMD_WritePage[2];
    unsigned char CMD_EraseBlock[2];
    unsigned char CMD_Reset;
    unsigned char CMD_ReadID;
    unsigned char CMD_ReadIDAddr;
    unsigned char CMD_ReadPage[2];
    unsigned char CMD_ReadStatus;
    unsigned char ID[16];
    unsigned char ID_Length;
}NAND_FLASH_CONFIG,*PNAND_FLASH_CONFIG;

//定义NAND操作时序参数
typedef struct _NAND_TIMING_COMFIG{
  uint32_t FSMC_SetupTime;
  uint32_t FSMC_WaitSetupTime;
  uint32_t FSMC_HoldSetupTime;
  uint32_t FSMC_HiZSetupTime;
}NAND_TIMING_COMFIG,*PNAND_TIMING_COMFIG;
/* NAND memory status */
#define NAND_STATUS_VALID_ADDRESS         ((uint32_t)0x00000100)
#define NAND_STATUS_INVALID_ADDRESS       ((uint32_t)0x00000200)
#define NAND_STATUS_TIMEOUT_ERROR         ((uint32_t)0x00000400)
#define NAND_STATUS_BUSY                  ((uint32_t)0x00000000)
#define NAND_STATUS_ERROR                 ((uint32_t)0x00000001)
#define NAND_STATUS_READY                 ((uint32_t)0x00000040)

//定义函数返回错误代码
#define NAND_SUCCESS             (0)   //函数执行成功
#define NAND_ERR_NOT_SUPPORT     (-1)  //适配器不支持该函数
#define NAND_ERR_USB_WRITE_FAIL  (-2)  //USB写数据失败
#define NAND_ERR_USB_READ_FAIL   (-3)  //USB读数据失败
#define NAND_ERR_CMD_FAIL        (-4)  //命令执行失败

#ifdef __cplusplus
extern "C"
{
#endif
/**
  * @brief  初始化配置NAND接口，该函数必须调用
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  PageSize NAND Flash每页字节数，不包括备用区字节数
  * @param  pTimeConfig 对NAND进行读写操作的时间参数
  * @retval 函数执行状态，小于0函数执行出错
  */
int WINAPI NAND_Init(int DevHandle,int ChipIndex,PNAND_TIMING_COMFIG pTimeConfig);

/**
  * @brief  初始化配置NAND接口，该函数必须调用
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pNandConfig 配置NAND Flash相关操作命令
  * @param  pNandMemInfo 配置NAND Flash容量相关信息
  * @retval 函数执行状态，小于0函数执行出错
  */
int WINAPI NAND_SetChipInfo(int DevHandle,int ChipIndex,PNAND_FLASH_CONFIG pNandConfig,PNAND_MEMORY_INFO pNandMemInfo);

/**
  * @brief  获取NAND Flash芯片ID信息
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pNandID 芯片ID存储缓冲区首地址
  * @retval 函数执行状态，小于0函数执行出错
  */
int WINAPI NAND_ReadID(int DevHandle,int ChipIndex,unsigned char IDAddr,unsigned char *pID,unsigned char IDLen);

/**
  * @brief  向NAND Flash写入数据，建议按照整页写，然后再将该函数返回的ECC写入备用区，以便以后读取的时候将读出数据的ECC和写入ECC做对比，实现对数据的校验
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pStartAddr 写数据地址
  * @param  pWriteData 待写入NAND Flash芯片的数据缓冲区首地址
  * @param  NumByteToWrite 待写入NAND Flash芯片的数据字节数
  * @param  pECC 写入NAND的数据计算出的ECC值，每页数据会生成一个ECC，若写入了N页数据，那么会返回N个ECC数据
  * @param  TimeOutMs 等待写入数据完毕的超时参数，单位为毫秒，该参数应尽量大于数据写入Flash的时间
  * @retval 函数执行状态，小于0函数执行出错
  */
//int WINAPI NAND_WritePage(int DevHandle,int ChipIndex,PNAND_ADDRESS pStartAddr,unsigned char *pWriteData, int NumByteToWrite,unsigned int *pECC,int TimeOutMs);
int WINAPI NAND_WritePage(int DevHandle,int ChipIndex,unsigned char *pCmds,unsigned char CmdsLen,unsigned char *pAddrs,unsigned char AddrLen,unsigned char *pWriteData,int NumByteToWrite,int TimeOutMs);
/**
  * @brief  从NAND Flash读出数据，建议按照整页读，读数据的时候会返回每页的ECC值，可以和备用区的ECC值进行校验对比
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pStartAddr 读数据地址
  * @param  pReadData 存储读出数据的缓冲区首地址
  * @param  NumByteToRead 待读出的数据字节数
  * @param  pECC 读出NAND的数据计算出的ECC值，每页数据会生成一个ECC，若读出了N页数据，那么会返回N个ECC数据
  * @param  TimeOutMs 等待读出数据完毕的超时参数，单位为毫秒，该参数应尽量大于数据读出Flash的时间
  * @retval 函数执行状态，小于0函数执行出错
  */
//int WINAPI NAND_ReadPage (int DevHandle,int ChipIndex, PNAND_ADDRESS pStartAddr,unsigned char *pReadData, int NumByteToRead,unsigned int *pECC,int TimeOutMs);
int WINAPI NAND_ReadPage (int DevHandle,int ChipIndex, unsigned char *pCmds,unsigned char CmdsLen,unsigned char *pAddrs,unsigned char AddrLen,unsigned char *pReadData, int NumByteToRead,int TimeOutMs);
/**
  * @brief  向NAND Flash备用区写入数据，比如写入每页的ECC值，以及坏块标志数据，该函数最多只能操作一页的备用区
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pStartAddr 写数据地址
  * @param  pWriteSpareAreaData 待写入备用区的数据缓冲区首地址
  * @param  NumByteToWrite 待写入备用区的数据字节数
  * @param  TimeOutMs 写入备用区数据超时参数，单位为毫秒
  * @retval 函数执行状态，小于0函数执行出错
  */
int WINAPI NAND_WriteSpareArea(int DevHandle,int ChipIndex, PNAND_ADDRESS pStartAddr,unsigned char *pWriteSpareAreaData, int NumByteToWrite,int TimeOutMs);

/**
  * @brief  从NAND Flash备用区读出数据，比如每页的ECC值，坏块标志值，该函数最多只能操作一页的备用区
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pStartAddr 读数据地址
  * @param  pReadSpareAreaData 读出备用区数据存储缓冲区首地址
  * @param  NumByteToRead 读出数据字节数
  * @param  TimeOutMs 读备用区数据超时参数，单位为毫秒
  * @retval 函数执行状态，小于0函数执行出错
  */
int WINAPI NAND_ReadSpareArea(int DevHandle,int ChipIndex, PNAND_ADDRESS pStartAddr,unsigned char *pReadSpareAreaData, int NumByteToRead,int TimeOutMs);

/**
  * @brief  按块擦出NAND Flash数据，擦出后所有数据变成0xFF
  * @param  DevHandle 设备索引号
  * @param  ChipIndex NAND芯片片选号，取值0或者1
  * @param  pStartAddr 擦出块起始地址
  * @param  NumBlockToErase 准备擦出的块数
  * @param  TimeOutMs 擦出数据超时参数，单位为毫秒
  * @retval 函数执行状态，小于0函数执行出错
  */
//int WINAPI NAND_EraseBlock(int DevHandle,int ChipIndex,PNAND_ADDRESS pStartAddr,int NumBlockToErase,int TimeOutMs);
int WINAPI NAND_EraseBlock(int DevHandle,int ChipIndex,unsigned char *pCmds,unsigned char CmdsLen,unsigned char *pAddrs,unsigned char AddrLen,int TimeOutMs);


int WINAPI NAND_SendCmd(int DevHandle,int ChipIndex,unsigned char *pCmds,unsigned char CmdsLen, unsigned char CEPolarity);
int WINAPI NAND_SendAddr(int DevHandle,int ChipIndex,unsigned char *pAddrs,unsigned char AddrsLen, unsigned char CEPolarity);
int WINAPI NAND_WriteData(int DevHandle,int ChipIndex,unsigned char *pWriteData,unsigned int WriteDataLen, unsigned char CEPolarity);
int WINAPI NAND_ReadData(int DevHandle,int ChipIndex,unsigned char *pReadData,unsigned int ReadDataLen, unsigned char CEPolarity);
int WINAPI NAND_WaitReady(int DevHandle,int ChipIndex,unsigned int WaitTimeoutUs);

#ifdef __cplusplus
}
#endif

#endif
