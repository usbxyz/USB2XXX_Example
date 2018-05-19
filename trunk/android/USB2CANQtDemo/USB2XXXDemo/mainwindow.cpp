#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>

#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QtAndroid>
#endif

#define USB2CAN (((uint64_t)'U'<<56)|((uint64_t)'S'<<48)|((uint64_t)'B'<<40)|((uint64_t)'2'<<32)|((uint64_t)'C'<<24)|((uint64_t)'A'<<16)|((uint64_t)'N'<<8)|((uint64_t)'B'<<0))

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef Q_OS_ANDROID
    javaClass = new QAndroidJniObject("com/usbxyz/USBDeviceActivity");
#endif
    pDialogCANSetting = new DialogCANSetting(this);
    pDialogCANSetting->exec();
    ui->pushButtonSendData->setDisabled(true);
    ui->spinBoxFrameID->setMaximum(0x7FF);
}

MainWindow::~MainWindow()
{
    delete ui;
}
//以下代码必须要
void MainWindow::keyPressEvent (QKeyEvent* event)
{
    if(event->key() == Qt::Key_Back){
        emit exitApp();
    }else{
        event->accept();
    }
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == ReceiveDataTimer){
        CAN_MSG CanMsgBuffer[1024];
        QMutexLocker locker(&mutex);
        int CanNum = CAN_GetMsg(DeviceHandle,pDialogCANSetting->CANIndex,CanMsgBuffer);
        for(int i=0;i<CanNum;i++){
            QString str;
            str.sprintf("RX:[%08X]",CanMsgBuffer[i].ID);
            for(int j=0;j<CanMsgBuffer[i].DataLen;j++){
                str.append(QString().sprintf("%02X ",CanMsgBuffer[i].Data[j]));
            }
            ui->textBrowser->moveCursor(QTextCursor::End);
            ui->textBrowser->append(str);
        }
    }
}

int MainWindow::CAN_GetBaudRateIndex(unsigned int BaudRate)
{
    for(int i=0;i<27;i++){
        if(BaudRate == CANBaudRateTab[i].BaudRate){
            return i;
        }
    }
    return 0;
}

bool MainWindow::initDevice()
{
    int devHandles[10];
    int ret;
    char FuncStr[256]={0};
    DEVICE_INFO DevInfo;
#ifdef Q_OS_ANDROID
    int fd = 0;
    fd = javaClass->callMethod<jint>("getDevFd");
    ret = USB_ScanDevice(devHandles,fd);
#else
    ret = USB_ScanDevice(devHandles);
#endif
    if(ret <= 0){
        QMessageBox::warning(this,"警告",QString().sprintf("没有扫描到设备！%d",ret));
        return false;
    }
    DeviceHandle = devHandles[0];
    if(!USB_OpenDevice(DeviceHandle)){
        QMessageBox::warning(this,"警告","打开设备失败！");
        return false;
    }
    if(!DEV_GetDeviceInfo(DeviceHandle,&DevInfo,FuncStr)){
        QMessageBox::warning(this,"警告","获取设备信息失败！");
        return false;
    }
    /*
    QString FirmwareInfo;
    FirmwareInfo += "固件名称: "+QString().sprintf("%s\n",DevInfo.FirmwareName);
    FirmwareInfo += "编译日期: "+QString().sprintf("%s\n",DevInfo.BuildDate);
    FirmwareInfo += "固件版本: "+QString().sprintf("v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
    FirmwareInfo += "硬件版本: "+QString().sprintf("v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
    QString SerialNumStr;
    for(int i=0;i<3;i++){
        SerialNumStr += QString().sprintf("%08X",DevInfo.SerialNumber[i]);
    }
    FirmwareInfo += "设备序号: "+SerialNumStr+"\n";
    FirmwareInfo += "固件功能: "+QString().sprintf("%s",FuncStr);
    QMessageBox::information(this,"固件信息",FirmwareInfo);
    */
    uint64_t DevType = ((uint64_t)DevInfo.SerialNumber[0]<<32)|(DevInfo.SerialNumber[1]);
    CAN_INIT_CONFIG CANConfig;
    //配置波特率
    int BRIndex = CAN_GetBaudRateIndex(pDialogCANSetting->CANBaudRate);
    if(DevType == USB2CAN){
        CANConfig.CAN_BRP = CANBaudRateTab42M[BRIndex].PreScale;
        CANConfig.CAN_SJW = CANBaudRateTab42M[BRIndex].SJW;
        CANConfig.CAN_BS1 = CANBaudRateTab42M[BRIndex].BS1;
        CANConfig.CAN_BS2 = CANBaudRateTab42M[BRIndex].BS2;
    }else{
        CANConfig.CAN_BRP = CANBaudRateTab[BRIndex].PreScale;
        CANConfig.CAN_SJW = CANBaudRateTab[BRIndex].SJW;
        CANConfig.CAN_BS1 = CANBaudRateTab[BRIndex].BS1;
        CANConfig.CAN_BS2 = CANBaudRateTab[BRIndex].BS2;
    }
    //其他参数
    CANConfig.CAN_Mode = pDialogCANSetting->CANWorkMode;//正常模式
    CANConfig.CAN_ABOM = 0;//禁止自动离线
    CANConfig.CAN_NART = 1;//禁止报文重传
    CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
    CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
    ret = CAN_Init(DeviceHandle,pDialogCANSetting->CANIndex,&CANConfig);
    if(ret!=CAN_SUCCESS){
        QMessageBox::warning(this,"警告","配置设备失败！");
        return false;
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
    ret = CAN_Filter_Init(DeviceHandle,pDialogCANSetting->CANIndex,&CANFilter);
    if(ret!=CAN_SUCCESS){
        QMessageBox::warning(this,"警告","配置过滤器失败！");
        return false;
    }

    return true;
}

void MainWindow::on_pushButtonStartReceive_clicked()
{
    if(ui->pushButtonStartReceive->text() == "启动CAN"){
        if(initDevice()){
            ui->pushButtonStartReceive->setText("停止CAN");
            ReceiveDataTimer = startTimer(10);
            ui->pushButtonSendData->setEnabled(true);
        }
    }else{
        ui->pushButtonStartReceive->setText("启动CAN");
        killTimer(ReceiveDataTimer);
        ui->pushButtonSendData->setEnabled(false);
    }
}

void MainWindow::on_pushButtonSendData_clicked()
{
    QMutexLocker locker(&mutex);
    CAN_MSG CANMsg;
    //获取帧数据
    QStringList DataList = ui->lineEditFrameData->text().split(" ", QString::SkipEmptyParts);
    CANMsg.DataLen = 0;
    for(int i = 0; (i<DataList.length()&&(i<8)); i++){
        CANMsg.Data[i] = DataList.at(i).toInt(NULL,16);
        CANMsg.DataLen++;
    }
    CANMsg.ID = ui->spinBoxFrameID->value();
    CANMsg.ExternFlag = ui->comboBoxFrameType->currentIndex();
    CANMsg.RemoteFlag = ui->comboBoxFrameFormat->currentIndex();
    int ret = CAN_SendMsg(DeviceHandle,pDialogCANSetting->CANIndex,&CANMsg,1);
    if(ret <= 0){
        QMessageBox::warning(this,"警告","发送数据失败！");
    }else{
        QString str;
        str.sprintf("TX:[%08X]",CANMsg.ID);
        for(int j=0;j<CANMsg.DataLen;j++){
            str.append(QString().sprintf("%02X ",CANMsg.Data[j]));
        }
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->textBrowser->append(str);
    }
}

void MainWindow::on_comboBoxFrameType_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "标准帧"){
        ui->spinBoxFrameID->setMaximum(0x7FF);
    }else{
        ui->spinBoxFrameID->setMaximum(0x1FFFFFFF);
    }
}

void MainWindow::on_comboBoxFrameFormat_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "数据帧"){
        ui->lineEditFrameData->setEnabled(true);
    }else{
        ui->lineEditFrameData->setDisabled(true);
    }
}

void MainWindow::on_pushButtonClearDisplay_clicked()
{
    ui->textBrowser->clear();
}
