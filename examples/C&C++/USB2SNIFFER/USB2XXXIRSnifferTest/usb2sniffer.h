/**
  ******************************************************************************
  * @file    usb2sniffer.h
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
#ifndef __USB2SNIFFER_H_
#define __USB2SNIFFER_H_

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
#define SNIFFER_SUCCESS             (0)   //函数执行成功
#define SNIFFER_ERR_NOT_SUPPORT     (-1)  //适配器不支持该函数
#define SNIFFER_ERR_USB_WRITE_FAIL  (-2)  //USB写数据失败
#define SNIFFER_ERR_USB_READ_FAIL   (-3)  //USB读数据失败
#define SNIFFER_ERR_CMD_FAIL        (-4)  //命令执行失败

#define SNIFFER_SAMPLE_MODE_1CH     (1)
#define SNIFFER_SAMPLE_MODE_2CH     (2)
#define SNIFFER_SAMPLE_MODE_4CH     (4)
#define SNIFFER_SAMPLE_MODE_8CH     (8)

//定义从机模式下连续读取数据的回调函数
typedef  int (WINAPI SNIFFER_GET_DATA_HANDLE)(int DevHandle,unsigned char *pData,int DataNum);//接收数据回掉函数

#ifdef __cplusplus
extern "C"
{
#endif
	int WINAPI SNIFFER_Init(int DevHandle,char WriteMode,unsigned int SampleRateHz,char SampleMode);
	int WINAPI SNIFFER_StartRead(int DevHandle,SNIFFER_GET_DATA_HANDLE *pGetDataHandle);
	int WINAPI SNIFFER_StopRead(int DevHandle);
    int WINAPI SNIFFER_GetData(int DevHandle,unsigned char *pDataBuffer,int BufferSize);
    int WINAPI SNIFFER_WriteData(int DevHandle,unsigned char *pWriteData,int WriteLen);
    int WINAPI SNIFFER_ContinueWriteData(int DevHandle,unsigned char *pWriteData,int WriteLen);
    int WINAPI SNIFFER_StopContinueWrite(int DevHandle);
#ifdef __cplusplus
}
#endif

#endif
