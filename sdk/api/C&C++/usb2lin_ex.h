/**
  ******************************************************************************
  * @file    usb2lin_ex.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   usb2lin_ex相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.toomoss.com">http://www.toomoss.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB2LIN_EX_H_
#define __USB2LIN_EX_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif
//注意：使用这些函数需要1.5.30及以上的固件才支持
//定义函数返回错误代码
#define LIN_EX_SUCCESS             (0)   //函数执行成功
#define LIN_EX_ERR_NOT_SUPPORT     (-1)  //适配器不支持该函数
#define LIN_EX_ERR_USB_WRITE_FAIL  (-2)  //USB写数据失败
#define LIN_EX_ERR_USB_READ_FAIL   (-3)  //USB读数据失败
#define LIN_EX_ERR_CMD_FAIL        (-4)  //命令执行失败
#define LIN_EX_ERR_CH_NO_INIT      (-5)  //该通道未初始化
#define LIN_EX_ERR_READ_DATA       (-6)  //LIN读数据失败
#define LIN_EX_ERR_PARAMETER       (-7)  //函数参数传入有误
//校验类型
#define LIN_EX_CHECK_STD     0  //标准校验，不含PID
#define LIN_EX_CHECK_EXT     1  //增强校验，包含PID
#define LIN_EX_CHECK_USER    2  //自定义校验类型，需要用户自己计算并传入Check，不进行自动校验
#define LIN_EX_CHECK_NONE    3  //不计算校验数据
#define LIN_EX_CHECK_ERROR   4  //接收数据校验错误
//主从机定义
#define LIN_EX_MASTER        1  //主机
#define LIN_EX_SLAVE         0  //从机
//帧类型
#define LIN_EX_MSG_TYPE_UN      0  //未知类型
#define LIN_EX_MSG_TYPE_MW      1  //主机向从机发送数据
#define LIN_EX_MSG_TYPE_MR      2  //主机从从机读取数据
#define LIN_EX_MSG_TYPE_SW      3  //从机发送数据
#define LIN_EX_MSG_TYPE_SR      4  //从机接收数据
#define LIN_EX_MSG_TYPE_BK      5  //只发送BREAK信号，若是反馈回来的数据，表明只检测到BREAK信号
#define LIN_EX_MSG_TYPE_SY      6  //表明检测到了BREAK，SYNC信号
#define LIN_EX_MSG_TYPE_ID      7  //表明检测到了BREAK，SYNC，PID信号
#define LIN_EX_MSG_TYPE_DT      8  //表明检测到了BREAK，SYNC，PID,DATA信号
#define LIN_EX_MSG_TYPE_CK      9  //表明检测到了BREAK，SYNC，PID,DATA,CHECK信号

typedef struct _LIN_EX_MSG{
    unsigned int  Timestamp;    //从机接收数据时代表时间戳，单位为0.1ms;主机读写数据时，表示数据读写后的延时时间，单位为ms
    unsigned char MsgType;      //帧类型
    unsigned char CheckType;    //校验类型
    unsigned char DataLen;      //LIN数据段有效数据字节数
    unsigned char Sync;         //固定值，0x55
    unsigned char PID;          //帧ID
    unsigned char Data[8];      //数据
    unsigned char Check;        //校验,只有校验数据类型为LIN_EX_CHECK_USER的时候才需要用户传入数据
    unsigned char BreakBits;    //该帧的BRAK信号位数，有效值为10到26，若设置为其他值则默认为13位
    unsigned char Reserve1;
}LIN_EX_MSG,*PLIN_EX_MSG;

#ifdef __cplusplus
extern "C"
{
#endif

int WINAPI  LIN_EX_Init(int DevHandle,unsigned char LINIndex,unsigned int BaudRate,unsigned char MasterMode);
int WINAPI  LIN_EX_MasterSync(int DevHandle,unsigned char LINIndex,LIN_EX_MSG *pInMsg,LIN_EX_MSG *pOutMsg,unsigned int MsgLen);
int WINAPI  LIN_EX_MasterWrite(int DevHandle,unsigned char LINIndex,unsigned char PID,unsigned char *pData,unsigned char DataLen,unsigned char CheckType);
int WINAPI  LIN_EX_MasterRead(int DevHandle,unsigned char LINIndex,unsigned char PID,unsigned char *pData);
int WINAPI  LIN_EX_SlaveSetIDMode(int DevHandle,unsigned char LINIndex,LIN_EX_MSG *pLINMsg,unsigned int MsgLen);
int WINAPI  LIN_EX_SlaveGetIDMode(int DevHandle,unsigned char LINIndex,LIN_EX_MSG *pLINMsg);
int WINAPI  LIN_EX_SlaveGetData(int DevHandle,unsigned char LINIndex,LIN_EX_MSG *pLINMsg);
int WINAPI  LIN_EX_CtrlPowerOut(int DevHandle,unsigned char State);
int WINAPI  LIN_EX_GetVbatValue(int DevHandle,unsigned short *pBatValue);

int WINAPI  LIN_EX_MasterStartSch(int DevHandle,unsigned char LINIndex,LIN_EX_MSG *pLINMsg,unsigned int MsgLen);
int WINAPI  LIN_EX_MasterStopSch(int DevHandle,unsigned char LINIndex);
int WINAPI  LIN_EX_MasterGetSch(int DevHandle,unsigned char LINIndex,LIN_EX_MSG *pLINMsg);

int WINAPI  LIN_EX_DecodeListFile(char *pFileName,char CheckType,int BaudRate,char *pReadDataList,char ReadDataListLen,char *pCheckTypeList,char CheckTypeListLen);
int WINAPI  LIN_EX_GetListFileMsg(int MsgIndex,int MsgLen,LIN_EX_MSG *pLINMsg);

#ifdef __cplusplus
}
#endif
#endif

