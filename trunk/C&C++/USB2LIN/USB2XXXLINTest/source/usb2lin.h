/**
  ******************************************************************************
  * @file    usb2lin.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   usb2lin相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.usbxyz.com">http://www.usbxyz.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB2LIN_H_
#define __USB2LIN_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

//定义函数返回错误代码
#define LIN_SUCCESS             (0)   //函数执行成功
#define LIN_ERR_NOT_SUPPORT     (-1)  //适配器不支持该函数
#define LIN_ERR_USB_WRITE_FAIL  (-2)  //USB写数据失败
#define LIN_ERR_USB_READ_FAIL   (-3)  //USB读数据失败
#define LIN_ERR_CMD_FAIL        (-4)  //命令执行失败
#define LIN_ERR_CH_NO_INIT      (-5)  //该通道未初始化
#define LIN_ERR_READ_DATA       (-6)  //LIN读数据失败
//LIN和校验模式
#define LIN_CHECK_MODE_STD     0
#define LIN_CHECK_MODE_EXT     1
//BREAK位数定义
#define LIN_BREAK_BITS_10    0x00
#define LIN_BREAK_BITS_11    0x20
//定义主从模式
#define LIN_MASTER          1
#define LIN_SLAVE           0
//定义从机操作模式
#define LIN_SLAVE_WRITE     0
#define LIN_SLAVE_READ      1

//定义初始化LIN初始化数据类型
typedef struct _LIN_CONFIG{
  unsigned int  BaudRate;     //波特率,最大20K
  unsigned char CheckMode;    //校验模式，0-标准校验模式，1-增强校验模式（包含PID）
  unsigned char MasterMode;   //主从模式，0-从模式，1-主模式
  unsigned char BreakBits;    //Break长度，0x00-10bit,0x20-11bit
}LIN_CONFIG,*PLIN_CONFIG;
//设置从模式下ID操作模式
typedef struct _SLAVE_LIN_DATA{
  uint8_t DataLen;  //从模式下发送数据的长度或者从模式接收数据的长度，不含校验字节
  uint8_t WorkMode; //0-从模式发送数据，1-从模式接收数据
  uint8_t Data[9];  //从模式下接收到的数据字节或者待发送的数据字节
}SLAVE_LIN_DATA,*PSLAVE_LIN_DATA;

#ifdef __cplusplus
extern "C"
{
#endif

int LIN_Init(int DevHandle,unsigned char Channel,LIN_CONFIG *pConfig);
int LIN_WriteData(int DevHandle,unsigned char Channel,unsigned char ID,unsigned char *pWriteData,unsigned char WriteLen);
int LIN_ReadData(int DevHandle,unsigned char Channel,unsigned char ID,unsigned char *pReadData);
int LIN_SlaveSetIDOperation(int DevHandle,unsigned char Channel,unsigned char ID,SLAVE_LIN_DATA SlaveOperationData);
int LIN_SlaveGetData(int DevHandle,unsigned char Channel,SLAVE_LIN_DATA *pSlaveData);

#ifdef __cplusplus
}
#endif
#endif

