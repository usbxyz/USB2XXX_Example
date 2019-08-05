  /*
  ******************************************************************************
  * @file     : USB2XXXLINTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX LIN test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2014, usbxyz.com
  * http://www.usbxyz.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LDFDecoder.h"
#include "usb_device.h"
#include "usb2lin_ex.h"

#define PRINT_LDF_INFO  1

int main(int argc, const char* argv[])
{
    LDF_LIN_FRAME *LINFrames = NULL;
    LDF_LIN_SCH *SchTables = NULL;
    int SignalCodingLen=0;
    int DevHandle[10]={0};
    LDF_CODING LDFCoding;
    char *MSGTypeStr[]={"UN","MW","MR","SW","SR","BK","SY","ID","DT","CK"};
    //扫描查找设备
    int ret;
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //打开设备
    bool state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //ldf文件解析
    ret = LDF_Decode(DevHandle[0],"test.ldf",&LDFCoding);
    if(ret != LDF_SUCCESS){
        printf("LDF decode failed\n");
        return 0;
    }
    //获取帧
    int LINFrameLen = LDFCoding.FrameLen;//LDF_GetFrames(&LINFrames,LINSignals,LINSignalsLen);
    LINFrames = LDFCoding.pFrames;
#if PRINT_LDF_INFO
    printf("LINFrameLen = %d\n",LINFrameLen);
    for(int i=0;i<LINFrameLen;i++){
        printf("Name = %s\n",LINFrames[i].pName);
        printf("--ID = 0x%02X\n",LINFrames[i].ID);
        printf("--Publisher = %s\n",LINFrames[i].pPublisher);
        printf("--Size = %d Bytes\n",LINFrames[i].Size);
        printf("--SignalListLen = %d\n",LINFrames[i].SignalListLen);
        for(int j=0;j<LINFrames[i].SignalListLen;j++){
            printf("--SignalName = %s\n",LINFrames[i].pSignalList[j].pName);
            printf("----Size = %d bits\n",LINFrames[i].pSignalList[j].Size);
            printf("----Offset = %d \n",LINFrames[i].pSignalList[j].Offset);
            printf("----InitValue = ");
            if(LINFrames[i].pSignalList[j].ValueType == LDF_SIGNAL_VALUE_TYPE_SCALAR){
                printf("%lld\n",LINFrames[i].pSignalList[j].Value.ScalarValue);
            }else{
                for(int ii=0;ii<(LINFrames[i].pSignalList[j].Size/8);ii++){
                    printf("%02X ",LINFrames[i].pSignalList[j].Value.ArrayValue[ii]);
                }
                printf("\n");
            }
            printf("----Publisher = %s\n",LINFrames[i].pSignalList[j].pPublisher);
            printf("----Subscribers = ");
            for(int ii=0;ii<LINFrames[i].pSignalList[j].SubscriberLen;ii++){
                printf("%s,",LINFrames[i].pSignalList[j].pSubscribers[ii]);
            }
            printf("\n");
            if((LINFrames[i].pSignalList[j].SignalCodings.pName!=NULL)&&(strlen(LINFrames[i].pSignalList[j].SignalCodings.pName)>0)){
                printf("----SignalCodings = %s\n",LINFrames[i].pSignalList[j].SignalCodings.pName);
            }
            printf("\n");
        }
    }
#endif
    //获取调度表
    int SchTableLen = LDFCoding.LINSchLen;//LDF_GetScheduleTables(&SchTables,LINFrames,LINFrameLen);
    SchTables = LDFCoding.pLINSch;
#if PRINT_LDF_INFO
    printf("SchTableLen = %d\n",SchTableLen);
    for(int i=0;i<SchTableLen;i++){
        printf("%s{",SchTables[i].pName);
        for(int j=0;j<SchTables[i].ScheduleCmdLen;j++){
            printf("%s delay %f ms;",SchTables[i].pScheduleCmdList[j].Frame.pName,SchTables[i].pScheduleCmdList[j].DelayOfMs);
        }
        printf("}\n");
    }
#endif
    //获取节点信息
    LDF_LIN_NODE Node = *LDFCoding.pNode;
    //int NodeLen = LDF_GetNodes(&Node);
#if PRINT_LDF_INFO
    printf("Master:%s,%f ms,%f ms\n",Node.pMasterName,Node.time_base,Node.jitter);
    printf("Slaves:");
    for(int i=0;i<Node.SlaveLen;i++){
        printf("%s,",Node.pSlaveNames[i]);
    }
    printf("\n");
#endif
    
    //初始化LIN适配器
    ret = LIN_EX_Init(DevHandle[0],0,9600,1);
    if(ret != LIN_EX_SUCCESS){
        printf("Config LIN failed!\n");
        return 0;
    }else{
        printf("Config LIN Success!\n");
    }
    printf("SchTableLen = %d\n",SchTableLen);
    //执行调度表
    for(int i=0;i<SchTableLen;i++){
        for(int j=0;j<SchTables[i].ScheduleCmdLen;j++){
            LDF_LIN_FRAME LINFrame = SchTables[i].pScheduleCmdList[j].Frame;
            LIN_EX_MSG LINMsg;
            LIN_EX_MSG LINOutMsg[10];
            if(strlen(LINFrame.pName) > 0){//确保当前帧是有效帧
                if((strcmp(LINFrame.pName,"MasterReq")==0)||((LINFrame.Type == LDF_FRAME_TYPE_NORMAL)&&(strcmp(LINFrame.pPublisher,Node.pMasterName)==0))){
                    //该帧为主机发送数据帧
                    if(LINFrame.Type == LDF_FRAME_TYPE_NORMAL){
                        LINMsg.CheckType = LIN_EX_CHECK_EXT;//可以根据ldf文件中的版本号来设置，大于等于2.0的一般都是增强校验，小于2.0的一般都是标准校验
                    }else{
                        LINMsg.CheckType = LIN_EX_CHECK_STD;
                    }
                    LINMsg.MsgType = LIN_EX_MSG_TYPE_MW;
                    LINMsg.PID = LINFrame.ID;
                    LINMsg.Timestamp = SchTables[i].pScheduleCmdList[j].DelayOfMs;
                    //将信号进行赋值,此处可以根据实际情况进行赋值
                    for(int i=0;i<LINFrame.SignalListLen;i++){
                        LINFrame.pSignalList[i].Value.ScalarValue = 0;
                    }
                    //将帧信号中的值转换到字节数组中
                    LDF_DataFrameToRaw(&LINFrame,LINMsg.Data);
                    LINMsg.DataLen = LINFrame.Size;
                    ret = LIN_EX_MasterSync(DevHandle[0],0,&LINMsg,LINOutMsg,1);
                    if(ret < LIN_EX_SUCCESS){
                        printf("MasterSync LIN failed!\n");
                        return 0;
                    }else{
                        printf("MsgLen = %d\n",ret);
                        for(int i=0;i<ret;i++){
                            printf("%s[%s] SYNC[%02X] PID[%02X] ",LINFrame.pName,MSGTypeStr[LINMsg.MsgType],LINOutMsg[i].Sync,LINOutMsg[i].PID);
                            for(int j=0;j<LINOutMsg[i].DataLen;j++){
                                printf("%02X ",LINOutMsg[i].Data[j]);
                            }
                            printf("[%02X] [%02d:%02d:%02d.%03d]\n",LINOutMsg[i].Check,(LINOutMsg[i].Timestamp/36000000)%60,(LINOutMsg[i].Timestamp/600000)%60,(LINOutMsg[i].Timestamp/10000)%60,(LINOutMsg[i].Timestamp/10)%1000);
                        }
                    }
                }else{
                    //该帧为主机读数据帧
                    LINMsg.MsgType = LIN_EX_MSG_TYPE_MR;
                    LINMsg.PID = LINFrame.ID;
                    LINMsg.Timestamp = SchTables[i].pScheduleCmdList[j].DelayOfMs;
                    ret = LIN_EX_MasterSync(DevHandle[0],0,&LINMsg,LINOutMsg,1);
                    if(ret < LIN_EX_SUCCESS){
                        printf("MasterSync LIN failed!\n");
                        return 0;
                    }else{
                        printf("MsgLen = %d\n",ret);
                        for(int i=0;i<ret;i++){
                            printf("%s[%s] SYNC[%02X] PID[%02X] ",LINFrame.pName,MSGTypeStr[LINMsg.MsgType],LINOutMsg[i].Sync,LINOutMsg[i].PID);
                            for(int j=0;j<LINOutMsg[i].DataLen;j++){
                                printf("%02X ",LINOutMsg[i].Data[j]);
                            }
                            printf("[%02X] [%02d:%02d:%02d.%03d]\n",LINOutMsg[i].Check,(LINOutMsg[i].Timestamp/36000000)%60,(LINOutMsg[i].Timestamp/600000)%60,(LINOutMsg[i].Timestamp/10000)%60,(LINOutMsg[i].Timestamp/10)%1000);
                            if(LINOutMsg[i].DataLen > 0){
                                //将字节数组的数据解析到帧信号中
                                LDF_DataRawToFrame(&LINFrame,LINOutMsg[i].Data);
                                //循环显示信号值
                                for(int j=0;j<LINFrame.SignalListLen;j++){
                                    if(LINFrame.pSignalList[j].ValueType == LDF_SIGNAL_VALUE_TYPE_SCALAR){
                                        printf("%s = %d\n",LINFrame.pSignalList[j].pName,LINFrame.pSignalList[j].Value.ScalarValue);
                                    }else{
                                        printf("%s = ",LINFrame.pSignalList[j].pName);
                                        for(int k=0;k<LINFrame.pSignalList[j].Size/8;k++){
                                            printf("%02X ",LINFrame.pSignalList[j].Value.ArrayValue[k]);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

