/**
  ******************************************************************************
  * @file    usb2pwm.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   usb2pwm相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.usbxyz.com">http://www.usbxyz.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB2PWM_H_
#define __USB2PWM_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

//定义初始化PWM的数据类型
typedef struct _PWM_CONFIG{
  unsigned short Prescaler[8];  //预分频器
  unsigned short Precision[8];  //占空比调节精度
  unsigned short Pulse[8];      //占空比，实际占空比=(Pulse/Precision)*100%
  unsigned char  Polarity[8];    //波形极性，取值0或者1
  unsigned char  ChannelMask;    //通道号，若要使能某个通道，则对应位为1，最低位对应通道0
}PWM_CONFIG,*PPWM_CONFIG;

//定义函数返回错误代码
#define PWM_SUCCESS             (0)   //函数执行成功
#define PWM_ERR_NOT_SUPPORT     (-1)  //适配器不支持该函数
#define PWM_ERR_USB_WRITE_FAIL  (-2)  //USB写数据失败
#define PWM_ERR_USB_READ_FAIL   (-3)  //USB读数据失败
#define PWM_ERR_CMD_FAIL        (-4)  //命令执行失败

#ifdef __cplusplus
extern "C"
{
#endif

int WINAPI PWM_Init(int DevHandle, PPWM_CONFIG pConfig);
int WINAPI PWM_Start(int DevHandle,unsigned char ChannelMask,unsigned int RunTimeUs);
int WINAPI PWM_SetPulse(int DevHandle,unsigned char ChannelMask,unsigned short *pPulse);
int WINAPI PWM_Stop(int DevHandle,unsigned char ChannelMask);

#ifdef __cplusplus
}
#endif
#endif
