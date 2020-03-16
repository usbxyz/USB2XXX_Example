/**
  ******************************************************************************
  * @file    offline_type.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   离线操作相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.toomoss.com">http://www.toomoss.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __OFFLINE_TYPE_H_
#define __OFFLINE_TYPE_H_

#include <stdint.h>

#define OFFLINE_MARK    0x7BB0F5C0

#define OFFLINE_ERROR_NONE      0x00
#define OFFLINE_ERROR_ERASE     0x01
#define OFFLINE_ERROR_WRITE     0x02
#define OFFLINE_ERROR_READ      0x03
#define OFFLINE_ERROR_VERIFY    0x04

#define OFFLINE_TYPE_LIN_SCH          0x00010001//LIN主机模式离线发送数据
#define OFFLINE_TYPE_CAN2LIN          0x00020001//CAN和LIN总线相互转换
#define OFFLINE_TYPE_CAN_SCH          0x00030001//CAN离线发送数据
#define OFFLINE_TYPE_CAN_REAY         0x00030002//CAN离线中继
#define OFFLINE_TYPE_CAN_REAY_ONLINE  0x00030003//CAN在线中继


#define CAN2LIN_TYPE_CAN2MLINW  0x00  //CAN转LIN主机写数据
#define CAN2LIN_TYPE_MLINR2CAN  0x01  //主机LIN读数据转CAN
#define CAN2LIN_TYPE_SLINR2CAN  0x02  //从机LIN接收数据转CAN
#define CAN2LIN_TYPE_CAN2SLINW  0x03  //CAN转LIN从机发送数据

#define CAN2LIN_DATA_TYPE_DIRECT  0x00  //直接转发接收到的数据
#define CAN2LIN_DATA_TYPE_SPECIAL 0x01  //转发制定的数据

#define CAN_RELAY_TYPE_CAN1_2_CAN2      0x00//CAN1->CAN2
#define CAN_RELAY_TYPE_CAN2_2_CAN1      0x01//CAN2->CAN1
#define CAN_RELAY_TYPE_CAN12_2_CAN21    0x02//CAN1<->CAN2

  /*******************************离线数据头相关定义*******************************/
typedef struct {
	uint32_t Mark;      //离线功能标识符，固件通过检测该标识符认为后面是有效的离线功能数据
	uint32_t Type;      //离线功能类型定义，参考CAN2LIN_TYPE_定义
	uint32_t DataLen;   //离线功能有效数据字节数
}OFFLINE_TYPE_HEAD;
/*******************************LIN离线发送相关定义*******************************/
//离线发送LIN初始化结构体
typedef struct {
	uint32_t BaudRate;
	uint8_t MasterMode;
	uint8_t ChannelEnable;//0-不初始化该通道，1-初始化该通道
}OFFLINE_LIN_INIT;
//离线发送LIN帧头
typedef struct {
	OFFLINE_LIN_INIT LINInit[2];
	struct {
		uint8_t Key : 2;      //1-LIN1作为按键，LIN2发送数据，2-LIN2作为按键，LIN1发送数据，其他值-不判断按键，直接发送
		uint8_t KeyType : 2;  //0-低电平发送，1-上升沿发送，2-下降沿发送，3-高电平发送
	}SendType;
	uint32_t MsgLen;
}OFFLINE_LIN_HEAD;
//离线发送LIN数据定义
typedef struct {
	struct {
		uint16_t MsgType : 3;      //帧类型
		uint16_t CheckType : 1;    //校验类型
		uint16_t DataLen : 4;      //LIN数据段有效数据字节数
		uint16_t Ch : 1;
	}Head;
	uint8_t Sync;         //固定值，0x55
	uint8_t PID;          //帧ID
	uint8_t Data[8];      //数据
	uint8_t Check;        //校验,只有校验数据类型为LIN_EX_CHECK_USER的时候才需要用户传入数据
	uint32_t  Timestamp;    //时间戳,单位为100us
}OFFLINE_LIN_DATA;
/*******************************CAN离线发送相关定义*******************************/
//定义CAN波特率参数表
typedef struct {
	uint16_t  SJW;
	uint16_t  BS1;
	uint16_t  BS2;
	uint16_t  BRP;
}OFFLINE_CAN_BAUD;
//离线发送CAN初始化结构体
typedef struct {
	OFFLINE_CAN_BAUD  NBT;
	OFFLINE_CAN_BAUD  DBT;
	uint8_t ISOCRCEnable;//0-禁止ISO CRC,1-使能ISO CRC
	uint8_t ResEnable;//0-不接入内部120欧终端电阻，1-接入内部120欧终端电阻
	uint8_t ChannelEnable;//0-不初始化该通道，1-初始化该通道
}OFFLINE_CAN_INIT;
typedef struct {
	OFFLINE_CAN_INIT CANInit[2];
	struct {
		uint8_t Key : 2;      //1-LIN1作为按键，LIN2发送数据，2-LIN2作为按键，LIN1发送数据，其他值-不判断按键，直接发送
		uint8_t KeyType : 2;  //0-低电平发送，1-上升沿发送，2-下降沿发送，3-高电平发送
	}SendType;
	uint32_t MsgLen;
}OFFLINE_CAN_HEAD;
//离线发送CAN数据定义
typedef struct {
	struct {
		uint32_t ID : 29;
		uint32_t : 1;
		uint32_t RTR : 1;
		uint32_t IDE : 1;
	}Head;
	uint32_t TimeStamp;
	struct {
		uint8_t BRS : 1;//1-CANFD帧
		uint8_t ESI : 1;
		uint8_t FDF : 1;//1-数据域加速
		uint8_t Ch : 2;
		uint8_t RXD : 1;
	}Flag;
	uint8_t DLC;
	uint8_t Data[64];
}OFFLINE_CAN_DATA;
//CAN中继头
typedef struct {
	OFFLINE_CAN_INIT CANInit[2];
	uint32_t MsgLen;
}CAN_RELAY_HEAD;
typedef struct {
	OFFLINE_CAN_DATA CANData[2];
	uint8_t ConvertType;
	uint8_t DataType;
	uint32_t PeriodMs;
}CAN_RELAY_DATA;
/*******************************CAN&LIN互转相关定义*******************************/
//离线实现CAN&LIN互转
typedef struct {
	OFFLINE_LIN_INIT LINInit[2];
	OFFLINE_CAN_INIT CANInit[2];
	uint32_t MsgLen;
}OFFLINE_CAN2LIN_HEAD;
//CAN&LIN互转数据
typedef struct {
	uint8_t ConvertType;
	uint8_t DataType;
	uint32_t PeriodMs;
	OFFLINE_CAN_DATA CANMsg;
	OFFLINE_LIN_DATA LINMsg;
}OFFLINE_CAN2LIN_DATA;


#endif

