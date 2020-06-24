/**
  ******************************************************************************
  * @file    can_uds.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   can_uds相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.toomoss.com">http://www.toomoss.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __CAN_UDS_H_
#define __CAN_UDS_H_

#include <stdint.h>
#include "offline_type.h"
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

#define CAN_UDS_OK            0
#define CAN_UDS_TRAN_USB      -98
#define CAN_UDS_TRAN_CAN      -99
#define CAN_UDS_TIMEOUT_A     -100
#define CAN_UDS_TIMEOUT_Bs    -101
#define CAN_UDS_TIMEOUT_Cr    -102
#define CAN_UDS_WRONG_SN      -103
#define CAN_UDS_INVALID_FS    -104
#define CAN_UDS_UNEXP_PDU     -105
#define CAN_UDS_WFT_OVRN      -106
#define CAN_UDS_BUFFER_OVFLW  -107
#define CAN_UDS_ERROR         -108

//1.CAN UDS地址定义
typedef  struct  _CAN_UDS_ADDR
{
    unsigned int    ReqID;        //请求报文ID。
    unsigned int    ResID;        //应答报文ID。
    unsigned char   ExternFlag;   //是否是扩展帧
    unsigned char   AddrFormats;  //0-normal, 1-extended ,2-mixed
    unsigned char   AddrExt;      //当AddrFormats不为normal时，该数据放到CAN数据域第1字节
    unsigned char   __Res;
}CAN_UDS_ADDR;

#ifdef __cplusplus
extern "C"
{
#endif

    int WINAPI CAN_UDS_Request(int DevHandle,unsigned char CANIndex,CAN_UDS_ADDR UDSAddr,unsigned char *pReqData,int DataLen);
    int WINAPI CAN_UDS_Response(int DevHandle,unsigned char CANIndex,CAN_UDS_ADDR UDSAddr,unsigned char *pResData,int TimeOutMs);

#ifdef __cplusplus
}
#endif
#endif

