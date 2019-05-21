/*******************************************************************************
  * @file    LDFDecoder.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2013-06-29 18:24:57 +0800 #$
  * @brief   LDF文件解析相关函数和数据类型定义.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.toomoss.com">http://www.toomoss.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __LDFDECODER_H_
#define __LDFDECODER_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

#define STR_MAX_LEN     32  //字符串最大字符长度，不要随意修改

#define LDF_SUCCESS                 0
#define LDF_ERROR_FILE_OPEN         (-1)//打开文件出错
#define LDF_ERROR_FILE_FORMAT       (-2)//文件格式错误
#define LDF_ERROR_DEV_DISCONNECT    (-3)//设备未连接
//逻辑类型编码值
typedef struct _LDF_CODING_LOGICAL{
    int RawValue;
    char *pDescription;
}LDF_CODING_LOGICAL;
//物理类型编码值
typedef struct _LDF_CODING_PHYSICAL{
    float Offset;
    float Factor;
    int Minimum;
    int Maximum;
    char *pUnit;
}LDF_CODING_PHYSICAL;
//信号中的值类型
enum LDF_SIGNAL_VALUE_TYPE{
    LDF_SIGNAL_VALUE_TYPE_SCALAR,   //标量值
    LDF_SIGNAL_VALUE_TYPE_ARRAY     //数组值
};
//信号值编码类型
enum LDF_SIGNAL_CODING_TYPE{
    LDF_SIGNAL_CODING_TYPE_NONE,
    LDF_SIGNAL_CODING_TYPE_PHYSICAL,
    LDF_SIGNAL_CODING_TYPE_LOGICAL
};
//信号类型
enum LDF_SIGNAL_TYPE{
    LDF_SIGNAL_TYPE_NORMAL,
    LDF_SIGNAL_TYPE_DIAGNOSTIC
};
//帧类型
enum LDF_FRAME_TYPE{
    LDF_FRAME_TYPE_NORMAL,
    LDF_FRAME_TYPE_DIAGNOSTIC
};
//信号编码
typedef struct _LDF_SIGNAL_CODING{
    char *pName;
    int LogicalValueLen;
    int PhysicalValueLen;
    LDF_CODING_LOGICAL *pLogicalValues;
    LDF_CODING_PHYSICAL *pPhysicalValues;
}LDF_SIGNAL_CODING;
//信号和信号编码对应值
typedef struct _LDF_SIGNAL_REPRESENTATION{
    char *pSignalName;
    char *pCodingName;
}LDF_SIGNAL_REPRESENTATION;
//信号值结构体
typedef struct _LDF_SIGNAL_VALUE{
    long long ScalarValue;
    unsigned char ArrayValue[8];
}LDF_SIGNAL_VALUE;
//信号编码
typedef struct _LDF_LIN_SIGNAL{
    char *pName;       //信号名称
    int Size;           //单位为bit
    LDF_SIGNAL_TYPE Type;   //普通信号或者诊断信号
    LDF_SIGNAL_VALUE_TYPE ValueType;//数值或者数组
    LDF_SIGNAL_VALUE Value;
    char *pPublisher;
    int SubscriberLen;
    char *pSubscribers[16];
    int Offset; //该信号在8字节数据中的偏移量
    LDF_SIGNAL_CODING SignalCodings;
}LDF_LIN_SIGNAL;
//帧，每帧数据包含多个信号
typedef struct _LDF_LIN_FRAME{
    char *pName;         //帧名称
    unsigned char ID;               //帧ID
    char *pPublisher;    //发布者名称
    unsigned char Size;             //帧长度，单位为字节
    LDF_FRAME_TYPE Type;
    int SignalListLen;
    LDF_LIN_SIGNAL *pSignalList;  //信号列表
}LDF_LIN_FRAME;

typedef struct _LDF_LIN_NODE{
    char *pMasterName;
    float time_base;
    float jitter;
    int SlaveLen;
    char *pSlaveNames[16];
}LDF_LIN_NODE;

typedef struct _LDF_LIN_SCH_CMD{
    LDF_LIN_FRAME Frame;
    float DelayOfMs;                //延时时间，单位为毫秒
}LDF_LIN_SCH_CMD;

typedef struct _LDF_LIN_SCH{
    char *pName;             //调度表名称
    int ScheduleCmdLen;                 //该表下面包含的帧数
    LDF_LIN_SCH_CMD *pScheduleCmdList;//改表下面的帧列表
}LDF_LIN_SCH;

typedef struct _LDF_NODE_ATTR{
    char *pName;
    unsigned short LIN_protocol;
    unsigned char configured_NAD;
    unsigned char initial_NAD;
    struct {
        unsigned int supplier_id;
        unsigned int function_id;
        unsigned int variant;
    }product_id;
    char *response_error;//signal_name
    char *fault_state_signals[64];
    int fault_state_signal_len;
    float P2_min;
    float ST_min;
    float N_As_timeout;
    float N_Cr_timeout;
    char *configurable_frames[64];
    int configurable_frame_len;
}LDF_NODE_ATTR;

typedef struct _LDF_CODING{
    int ProtocolVersion;
    int LanguageVersion;
    int LINSpeed;
    LDF_LIN_NODE *pNode;
    int FrameLen;
    LDF_LIN_FRAME *pFrames;
    int NodeAttrLen;
    LDF_NODE_ATTR *pNodeAttrs;
    int LINSchLen;
    LDF_LIN_SCH *pLINSch;
}LDF_CODING;

#ifdef __cplusplus
extern "C"
{
#endif
    int WINAPI LDF_Decode(int DevHandle,char *pLDFFileName,LDF_CODING *pLDFCoding);
    int WINAPI LDF_DataRawToFrame(LDF_LIN_FRAME *pFrame,unsigned char *pRawData);
    int WINAPI LDF_DataFrameToRaw(LDF_LIN_FRAME *pFrame,unsigned char *pRawData);
#ifdef __cplusplus
}
#endif

#endif


