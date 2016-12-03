/**
  ******************************************************************************
  * @file    usb2uart.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   usb2uart相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.usbxyz.com">http://www.usbxyz.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB2UART_H_
#define __USB2UART_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

//定义初始化UART初始化数据类型
typedef struct _UART_CONFIG{
  unsigned int  BaudRate;     //波特率
  unsigned char WordLength;   //数据位宽，0-8bit,1-9bit
  unsigned char StopBits;     //停止位宽，0-1bit,1-0.5bit,2-2bit,3-1.5bit
  unsigned char Parity;       //奇偶校验，0-No,4-Even,6-Odd
  unsigned char TEPolarity;   //TE输出控制，0x80-输出TE信号，且低电平有效，0x81-输出TE信号，且高电平有效，0x00不输出TE信号
}UART_CONFIG,*PUART_CONFIG;

//定义函数返回错误代码
#define UART_SUCCESS             (0)   //函数执行成功
#define UART_ERR_NOT_SUPPORT     (-1)  //适配器不支持该函数
#define UART_ERR_USB_WRITE_FAIL  (-2)  //USB写数据失败
#define UART_ERR_USB_READ_FAIL   (-3)  //USB读数据失败
#define UART_ERR_CMD_FAIL        (-4)  //命令执行失败
//数据位宽
#define UART_WORD_LENGTH_8BIT     0
#define UART_WORD_LENGTH_9BIT     1
//停止位
#define UART_STOP_BITS_1          0
#define UART_STOP_BITS_05         1
#define UART_STOP_BITS_2          2
#define UART_STOP_BITS_15         3
//奇偶校验位
#define UART_PARITY_NO            0
#define UART_PARITY_EVEN          4
#define UART_PARITY_ODD           6
//TE控制信号输出
#define UART_TE_DISEN             0x00
#define UART_TE_EN_LOW            0x80
#define UART_TE_EN_HIGH           0x81

#ifdef __cplusplus
extern "C"
{
#endif

int WINAPI UART_Init(int DevIndex, unsigned char Channel, PUART_CONFIG pConfig);
int WINAPI UART_WriteBytes(int DevIndex,unsigned char Channel,unsigned char *pWriteData,int DataSize);
int WINAPI UART_WriteBytesAsync(int DevIndex,unsigned char Channel,unsigned char *pWriteData,int DataSize);
int WINAPI UART_ReadBytes(int DevIndex,unsigned char Channel,unsigned char *pReadData,int TimeOutMs);

#ifdef __cplusplus
}
#endif
#endif
