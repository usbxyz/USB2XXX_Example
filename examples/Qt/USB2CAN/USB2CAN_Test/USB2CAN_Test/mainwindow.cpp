#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <QDebug>
#include "usb_device.h"
#include "usb2can.h"

#define GET_FIRMWARE_INFO   1
#define CAN_MODE_LOOPBACK   1
#define CAN_SEND_MSG        1
#define CAN_GET_MSG         1
#define CAN_GET_STATUS      1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int CANIndex = 0;//USB2XXX只支持一个通道，所以该参数必须为0
    bool state;
    int ret;
    //扫描查找设备
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        qDebug()<<QString().sprintf("No device connected!");
        return;
    }
    //打开设备
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        qDebug()<<QString().sprintf("Open device error!");
        return;
    }
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //获取固件信息
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FunctionStr);
    if(!state){
        qDebug()<<QString().sprintf("Get device infomation error!");
        return;
    }else{
        qDebug()<<QString().sprintf("Firmware Info:");
        qDebug()<<QString().sprintf("Firmware Name:%s",DevInfo.FirmwareName);
        qDebug()<<QString().sprintf("Firmware Build Date:%s",DevInfo.BuildDate);
        qDebug()<<QString().sprintf("Firmware Version:v%d.%d.%d",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        qDebug()<<QString().sprintf("Hardware Version:v%d.%d.%d",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        qDebug()<<QString().sprintf("Firmware Functions:%s",FunctionStr);
    }
#endif
    //初始化配置CAN
    CAN_INIT_CONFIG CANConfig;
#if CAN_MODE_LOOPBACK
    CANConfig.CAN_Mode = 1;//环回模式
#else
    CANConfig.CAN_Mode = 0;//正常模式
#endif
    CANConfig.CAN_ABOM = 0;//禁止自动离线
    CANConfig.CAN_NART = 1;//禁止报文重传
    CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
    CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
    //配置波特率,波特率 = 42M/(BRP*(SJW+BS1+BS2))
    CANConfig.CAN_BRP = 4;
    CANConfig.CAN_BS1 = 16;
    CANConfig.CAN_BS2 = 4;
    CANConfig.CAN_SJW = 1;
    ret = CAN_Init(DevHandle[0],CANIndex,&CANConfig);
    if(ret != CAN_SUCCESS){
        qDebug()<<QString().sprintf("Config CAN failed!");
        return;
    }else{
        qDebug()<<QString().sprintf("Config CAN Success!");
    }
    //配置过滤器，必须配置，否则可能无法收到数据
    CAN_FILTER_CONFIG CANFilter;
    CANFilter.Enable = 1;
    CANFilter.ExtFrame = 0;
    CANFilter.FilterIndex = 0;
    CANFilter.FilterMode = 0;
    CANFilter.MASK_IDE = 0;
    CANFilter.MASK_RTR = 0;
    CANFilter.MASK_Std_Ext = 0;
    CAN_Filter_Init(DevHandle[0],CANIndex,&CANFilter);
#if CAN_SEND_MSG//发送CAN帧
    CAN_MSG CanMsg[5];
    for(int i=0;i<5;i++){
        CanMsg[i].ExternFlag = 0;
        CanMsg[i].RemoteFlag = 0;
        CanMsg[i].ID = i;
        CanMsg[i].DataLen = 8;
        for(int j=0;j<CanMsg[i].DataLen;j++){
            CanMsg[i].Data[j] = j;
        }
    }
    int SendedNum = CAN_SendMsg(DevHandle[0],CANIndex,CanMsg,5);
    if(SendedNum >= 0){
        qDebug()<<QString().sprintf("Success send frames:%d",SendedNum);
    }else{
        qDebug()<<QString().sprintf("Send CAN data failed!");
    }
#endif
#if CAN_GET_STATUS
    CAN_STATUS CANStatus;
    ret = CAN_GetStatus(DevHandle[0],CANIndex,&CANStatus);
    if(ret == CAN_SUCCESS){
        qDebug()<<QString().sprintf("TSR = %08X",CANStatus.TSR);
        qDebug()<<QString().sprintf("ESR = %08X",CANStatus.ESR);
    }else{
        qDebug()<<QString().sprintf("Get CAN status error!");
    }
#endif
    //延时
#ifndef OS_UNIX
    Sleep(100);
#else
    usleep(100*1000);
#endif
#if CAN_GET_MSG
    CAN_MSG CanMsgBuffer[10240];
    int CanNum = CAN_GetMsg(DevHandle[0],CANIndex,CanMsgBuffer);
    if(CanNum > 0){
        qDebug()<<QString().sprintf("CanNum = %d",CanNum);
        for(int i=0;i<CanNum;i++){
            qDebug()<<QString().sprintf("CanMsg[%d].ID = %d",i,CanMsgBuffer[i].ID);
            qDebug()<<QString().sprintf("CanMsg[%d].TimeStamp = %d",i,CanMsgBuffer[i].TimeStamp);
            QString str;
            for(int j=0;j<CanMsgBuffer[i].DataLen;j++){
                str.append(QString().sprintf("%02X ",CanMsgBuffer[i].Data[j]));
            }
            qDebug()<<QString().sprintf("CanMsg[%d].Data = ",i)<<str;
        }
    }else if(CanNum == 0){
        qDebug()<<QString().sprintf("No CAN data!");
    }else{
        qDebug()<<QString().sprintf("Get CAN data error!");
    }
#endif
    //关闭设备
    USB_CloseDevice(DevHandle[0]);
    return;
}
