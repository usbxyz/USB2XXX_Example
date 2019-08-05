#include "mainwindow.h"
#ifdef LANGUE_EN
#include "ui_mainwindow_en.h"
#else
#include "ui_mainwindow_ch.h"
#endif
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ScanDevice();
    int tableWidth=930;
    ui->nodeListTableWidget->setColumnWidth(0, 50*tableWidth/930);
    ui->nodeListTableWidget->setColumnWidth(1, 90*tableWidth/930);
    ui->nodeListTableWidget->setColumnWidth(2, 80*tableWidth/930);
    ui->nodeListTableWidget->setColumnWidth(3, 80*tableWidth/930);
    ui->nodeListTableWidget->setColumnWidth(4, 620*tableWidth/930);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *pEvent)
{
    int tableWidth = ui->nodeListTableWidget->width();
    if(tableWidth >= 930){
        ui->nodeListTableWidget->setColumnWidth(0, 50*tableWidth/930);
        ui->nodeListTableWidget->setColumnWidth(1, 90*tableWidth/930);
        ui->nodeListTableWidget->setColumnWidth(2, 80*tableWidth/930);
        ui->nodeListTableWidget->setColumnWidth(3, 80*tableWidth/930);
        ui->nodeListTableWidget->setColumnWidth(4, 620*tableWidth/930);
    }
}

void MainWindow::ScanDevice(void)
{
    int DevHandle[20];
    int deviceNum = USB_ScanDevice(DevHandle);
    ui->deviceHandleComboBox->clear();
    for(int i=0;i<deviceNum;i++){
        ui->deviceHandleComboBox->addItem(QString().sprintf("%08X",DevHandle[i]));
    }
}

QString MainWindow::GetErrorString(int ErrorCode)
{
    QString strTemp;
    strTemp.sprintf("[%d]",ErrorCode);
    switch(ErrorCode){
    case 0:
        strTemp += QString("成功");
        break;
    case 1:
        strTemp += QString("固件擦除出错");
        break;
    case 2:
        strTemp += QString("当前模式为APP，不能擦除固件");
        break;
    case 3:
        strTemp += QString("当前地址超出了正常的地址范围");
        break;
    case 4:
        strTemp += QString("当前模式不能写入固件数据");
        break;
    case 5:
        strTemp += QString("数据写入程序存储器出错");
        break;
    case 6:
        strTemp += QString("数据长度超出了程序存储器范围");
        break;
    case 7:
        strTemp += QString("数据传输CRC校验出错");
        break;
    case 8:
        strTemp += QString("数据写入芯片CRC校验出错");
        break;
    default:
        strTemp += QString("未知错误");
        break;
    }
    return strTemp;
}

void MainWindow::on_openFirmwareFilePushButton_clicked()
{
    QString fileName;
    fileName=QFileDialog::getOpenFileName(this,
                                          tr("Open files"),
                                          "",
                                          "Binary Files (*.bin);;Hex Files (*.hex);;All Files (*.*)");
    if(fileName.isNull()){
        return;
    }
    ui->firmwareLineEdit->setText(fileName);
}

bool MainWindow::DeviceConfig(void)
{
    int ret;
    int DeviceHandle = ui->deviceHandleComboBox->currentText().toInt(NULL,16);
    bool state;
    state = USB_OpenDevice(DeviceHandle);
    if(!state){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Open device faild!");
#else
        QMessageBox::warning(this,"警告","打开设备失败！");
#endif
        return false;
    }
    QString str = ui->baudRateComboBox->currentText();
    str.resize(str.length()-4);
    int baud = str.toInt(NULL,10)*1000;
    qDebug()<<"baud = "<<baud;
    ret = CAN_BOOT_Init(DeviceHandle,
                        ui->channelIndexComboBox->currentIndex(),
                        ui->spinBoxSendID->value(),
                        ui->spinBoxReceiveID->value(),
                        ui->comboBoxIDType->currentIndex()|(ui->checkBoxEnResistor->isChecked()?0x80:0x00),
                        baud);
    if(ret != CAN_SUCCESS){
#ifdef LANGUE_EN
        QMessageBox::warning(this,"Warning","Config device faild!");
#else
        QMessageBox::warning(this,"警告",QString().sprintf("配置设备失败！%d",ret));
#endif
        return false;
    }
    return true;
}
void MainWindow::on_updateFirmwarePushButton_clicked()
{
    QTime time;
    time.start();
    int ret;
    int DeviceHandle = ui->deviceHandleComboBox->currentText().toInt(NULL,16);
    bool ConfFlag;
    uint8_t FirmwareData[4096]={0};
    uint8_t NodeAddr=0;

    QFile firmwareFile(ui->firmwareLineEdit->text());
    //初始化设备
    ConfFlag = DeviceConfig();
    if(!ConfFlag){
        return;
    }
    if(!firmwareFile.open(QFile::ReadOnly)){
        QMessageBox::warning(this,"警告","打开文件失败！");
        return;
    }
    //循环更新
    for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
        QWidget *widget = (QWidget *)ui->nodeListTableWidget->cellWidget(r,0);
        QCheckBox *ckb = (QCheckBox *)widget->children().at(1);
        if(ckb->isChecked()){
            uint32_t FWVersion;
            uint8_t FWType;
            NodeAddr = ui->nodeListTableWidget->item(r,1)->text().toUInt(NULL,16);
            ret = CAN_BOOT_GetFWInfo(DeviceHandle,
                                     ui->channelIndexComboBox->currentIndex(),
                                     NodeAddr,
                                     &FWType,
                                     &FWVersion,
                                     50
                                     );
            if(ret == CAN_SUCCESS){
                if(FWType != FW_TYPE_BOOT){
                    ret = CAN_BOOT_EnterBootMode(DeviceHandle,
                                                 ui->channelIndexComboBox->currentIndex(),
                                                 NodeAddr);
                    if(ret != CAN_SUCCESS){
                        ui->nodeListTableWidget->item(r,4)->setText("进入BOOT模式失败！");
                        return;
                    }else{
                        //延时一段时间，保证设备成功进入BOOT模式
#ifndef OS_UNIX
                        Sleep(300);
#else
                        usleep(300*1000);
#endif
                        //再次确认固件信息
                        ret = CAN_BOOT_GetFWInfo(DeviceHandle,
                                                 ui->channelIndexComboBox->currentIndex(),
                                                 NodeAddr,
                                                 &FWType,
                                                 &FWVersion,
                                                 50
                                                 );
                        if(ret != CAN_SUCCESS){
                            ui->nodeListTableWidget->item(r,4)->setText("获取固件信息失败！");
                            return;
                        }else if(FWType != FW_TYPE_BOOT){
                            ui->nodeListTableWidget->item(r,4)->setText("进入BOOT模式失败！");
                            return;
                        }
                    }
                }
                qDebug()<<"ENTER BOOT MODE";
                //成功进入BOOT模式，开始升级固件
                ui->nodeListTableWidget->item(r,4)->setText("开始擦除固件，该操作可能比较耗时...");
                QCoreApplication::processEvents(QEventLoop::AllEvents);
                ret = CAN_BOOT_EraseApp(DeviceHandle,
                                        ui->channelIndexComboBox->currentIndex(),
                                        NodeAddr,
                                        firmwareFile.size(),
                                        10000);
                if(ret != CAN_SUCCESS){
                    ui->nodeListTableWidget->item(r,4)->setText(QString("执行固件擦除指令失败！\r\n")+GetErrorString(ret));
                    return;
                }else{
                    ui->nodeListTableWidget->item(r,4)->setText("固件擦除成功！");
                }
            }else{
                ui->nodeListTableWidget->item(r,4)->setText("获取固件信息失败！");
                return;
            }
        }
    }
    //发送固件数据
    int read_data_num;
#ifdef LANGUE_EN
    QProgressDialog writeDataProcess("Upgrade firmware...","Cancel",0,firmwareFile.size(),this);
    writeDataProcess.setWindowTitle("Upgrade firmware");
#else
    QProgressDialog writeDataProcess("正在更新固件...","取消",0,firmwareFile.size(),this);
    writeDataProcess.setWindowTitle("更新固件");
#endif
    writeDataProcess.setModal(true);
    writeDataProcess.show();
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    int i=0;
    uint16_t BufferSize=0;
    uint16_t CRC16;
    for(i=0;i<firmwareFile.size();){
        for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
            QWidget *widget = (QWidget *)ui->nodeListTableWidget->cellWidget(r,0);
            QCheckBox *ckb = (QCheckBox *)widget->children().at(1);
            if(ckb->isChecked()){
                ui->nodeListTableWidget->item(r,4)->setText("开始写入数据...");
                NodeAddr = ui->nodeListTableWidget->item(r,1)->text().toUInt(NULL,16);
                ret = CAN_BOOT_SetAddrOffset(DeviceHandle,
                                             ui->channelIndexComboBox->currentIndex(),
                                             NodeAddr,
                                             i,
                                             &BufferSize,
                                             50);
                if(ret != CAN_SUCCESS){
                    ui->nodeListTableWidget->item(r,4)->setText(QString("设置地址偏移失败！\r\n")+GetErrorString(ret));
                    return;
                }
            }
        }
        if(BufferSize <= 0){
            QMessageBox::warning(this,"警告","获取数据缓冲区大小为0！");
            return;
        }
        qDebug()<<"SetAddrOffset = "<<i;
        qDebug()<<"BufferSize = "<<BufferSize;
        read_data_num = firmwareFile.read((char*)FirmwareData,BufferSize);
        qDebug()<<"read_data_num = "<<read_data_num;
        i += BufferSize;
        if(ui->allNodeCheckBox->isChecked()){
            ret = CAN_BOOT_SendAppData(DeviceHandle,
                                       ui->channelIndexComboBox->currentIndex(),
                                       0x7F,
                                       FirmwareData,
                                       read_data_num,
                                       &CRC16);
            if(ret != CAN_SUCCESS){
                for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
                    ui->nodeListTableWidget->item(r,4)->setText(QString("发送数据失败！\r\n")+GetErrorString(ret));
                }
                writeDataProcess.close();
                return;
            }
        }else{
            for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
                QWidget *widget = (QWidget *)ui->nodeListTableWidget->cellWidget(r,0);
                QCheckBox *ckb = (QCheckBox *)widget->children().at(1);
                if(ckb->isChecked()){
                    NodeAddr = ui->nodeListTableWidget->item(r,1)->text().toUInt(NULL,16);
                    ret = CAN_BOOT_SendAppData(DeviceHandle,
                                               ui->channelIndexComboBox->currentIndex(),
                                               NodeAddr,
                                               FirmwareData,
                                               read_data_num,
                                               &CRC16);
                    if(ret != CAN_SUCCESS){
                        ui->nodeListTableWidget->item(r,4)->setText(QString("发送数据失败！\r\n")+GetErrorString(ret));
                        writeDataProcess.close();
                        return;
                    }
                    qDebug()<<"Send App Data Success";
                    qDebug()<<"CRC16 = "<<QString().sprintf("%04X",CRC16);
                }
            }
        }
        for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
            QWidget *widget = (QWidget *)ui->nodeListTableWidget->cellWidget(r,0);
            QCheckBox *ckb = (QCheckBox *)widget->children().at(1);
            if(ckb->isChecked()){
                NodeAddr = ui->nodeListTableWidget->item(r,1)->text().toUInt(NULL,16);
                ret = CAN_BOOT_WriteAppData(DeviceHandle,
                                             ui->channelIndexComboBox->currentIndex(),
                                             NodeAddr,
                                             CRC16,
                                             1000);
                if(ret != CAN_SUCCESS){
                    ui->nodeListTableWidget->item(r,4)->setText(QString("APP数据写入失败！\r\n")+GetErrorString(ret));
                    writeDataProcess.close();
                    return;
                }
            }
        }
        writeDataProcess.setValue(i);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if(writeDataProcess.wasCanceled()){
            return;
        }
    }
    writeDataProcess.setValue(firmwareFile.size());
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    if(writeDataProcess.wasCanceled()){
        return;
    }
    //数据发送完毕，发送执行固件指令
    if(ui->allNodeCheckBox->isChecked()){
        ret = CAN_BOOT_ExecuteApp(DeviceHandle,
                                   ui->channelIndexComboBox->currentIndex(),
                                   0x7F);
        if(ret != CAN_SUCCESS){
            for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
                ui->nodeListTableWidget->item(r,4)->setText(QString("发送执行固件指令失败！\r\n")+GetErrorString(ret));
            }
            return;
        }
    }else{
        for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
            QWidget *widget = (QWidget *)ui->nodeListTableWidget->cellWidget(r,0);
            QCheckBox *ckb = (QCheckBox *)widget->children().at(1);
            if(ckb->isChecked()){
                NodeAddr = ui->nodeListTableWidget->item(r,1)->text().toUInt(NULL,16);
                ret = CAN_BOOT_ExecuteApp(DeviceHandle,
                                           ui->channelIndexComboBox->currentIndex(),
                                           NodeAddr);
                if(ret != CAN_SUCCESS){
                    ui->nodeListTableWidget->item(r,4)->setText(QString("发送执行固件指令失败！\r\n")+GetErrorString(ret));
                    return;
                }
            }
        }
    }
#ifndef OS_UNIX
    Sleep(300);
#else
    usleep(300*1000);
#endif
    //循环获取固件信息
    for(int r=0;r<ui->nodeListTableWidget->rowCount();r++){
        QWidget *widget = (QWidget *)ui->nodeListTableWidget->cellWidget(r,0);
        QCheckBox *ckb = (QCheckBox *)widget->children().at(1);
        if(ckb->isChecked()){
            uint32_t FWVersion;
            uint8_t FWType;
            NodeAddr = ui->nodeListTableWidget->item(r,1)->text().toUInt(NULL,16);
            qDebug()<<"GET CHIP INFO "<<NodeAddr;
            ret = CAN_BOOT_GetFWInfo(DeviceHandle,
                                     ui->channelIndexComboBox->currentIndex(),
                                     NodeAddr,
                                     &FWType,
                                     &FWVersion,
                                     50
                                     );
            if(ret == CAN_SUCCESS){
                QString str;
                //固件类型
                if(FWType == FW_TYPE_BOOT){
                    str = "BOOT";
                }else{
                    str = "APP";
                }
                ui->nodeListTableWidget->item(r,2)->setText(str);
                //固件版本
                str.sprintf("v%d.%d.%d.%d",((FWVersion>>24)&0xFF),((FWVersion>>16)&0xFF),((FWVersion>>8)&0xFF),(FWVersion&0xFF));
                ui->nodeListTableWidget->item(r,3)->setText(str);
                //更新状态
                ui->nodeListTableWidget->item(r,4)->setText("固件更新成功！");
            }else{
                ui->nodeListTableWidget->item(r,4)->setText("固件执行失败！");
            }
        }
    }
    qDebug()<<time.elapsed()/1000.0<<"s";
}

void MainWindow::on_openFirmwareFileAction_triggered()
{
    on_openFirmwareFilePushButton_clicked();
}

void MainWindow::on_scanNodeAction_triggered()
{
    int ret;
    bool GotNode=false;
    int DeviceHandle = ui->deviceHandleComboBox->currentText().toInt(NULL,16);
    uint8_t startAddr = 0,endAddr = 0;
    ScanDevRangeDialog *pScanDevRangeDialog = new ScanDevRangeDialog();
    if(pScanDevRangeDialog->exec() == QDialog::Accepted){
        startAddr = pScanDevRangeDialog->StartAddr;
        endAddr = pScanDevRangeDialog->EndAddr;
    }else{
        return ;
    }
    bool ConfFlag = DeviceConfig();
    if(!ConfFlag){
        return;
    }
    ui->nodeListTableWidget->verticalHeader()->hide();
    ui->nodeListTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->nodeListTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->nodeListTableWidget->setRowCount(0);
#ifdef LANGUE_EN
    QProgressDialog scanNodeProcess("Scanning CAN node...","Cancel",0,MAX_NODE_NUM,this);
    scanNodeProcess.setWindowTitle("Scanning CAN node");
#else
    QProgressDialog scanNodeProcess("正在扫描节点...","取消",0,endAddr-startAddr,this);
    scanNodeProcess.setWindowTitle("扫描节点");
#endif
    scanNodeProcess.setModal(true);
    scanNodeProcess.show();
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    int i=0;
    while(startAddr <= endAddr){
        uint32_t FWVersion;
        uint8_t FWType;
        i++;
        ret = CAN_BOOT_GetFWInfo(DeviceHandle,
                                 ui->channelIndexComboBox->currentIndex(),
                                 startAddr,
                                 &FWType,
                                 &FWVersion,
                                 50
                                 );
        if(ret == CAN_SUCCESS){
            ui->nodeListTableWidget->setRowCount(ui->nodeListTableWidget->rowCount()+1);
            ui->nodeListTableWidget->setRowHeight(ui->nodeListTableWidget->rowCount()-1,20);
            //选择
            QHBoxLayout *hLayout = new QHBoxLayout();
            QCheckBox *ckb = new QCheckBox();
            QWidget *widget = new QWidget(ui->nodeListTableWidget);
            ckb->setChecked(true);
            hLayout->addWidget(ckb);
            hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
            hLayout->setAlignment(ckb, Qt::AlignCenter);
            widget->setLayout(hLayout);
            ui->nodeListTableWidget->setCellWidget(ui->nodeListTableWidget->rowCount()-1, 0, widget);
            //节点地址
            QString str;
            str.sprintf("0x%X",startAddr);
            QTableWidgetItem *item = new QTableWidgetItem(str);
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,1,item);
            //固件类型
            if(FWType == FW_TYPE_BOOT){
                str = "BOOT";
            }else{
                str = "APP";
            }
            item = new QTableWidgetItem(str);
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,2,item);
            //固件版本
            str.sprintf("v%d.%d.%d.%d",((FWVersion>>24)&0xFF),((FWVersion>>16)&0xFF),((FWVersion>>8)&0xFF),(FWVersion&0xFF));
            item = new QTableWidgetItem(str);
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,3,item);
            //更新状态
            ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,4,new QTableWidgetItem("等待更新"));
            GotNode = true;
        }else if(ret == CAN_BOOT_ERR_SEND){
            scanNodeProcess.close();
            QMessageBox::warning(this,"警告","发送CAN数据失败！\r\n"
                                           "1，检查CAN总线波特率是否匹配？\r\n"
                                           "2，检查总线终端电阻是否接好？\r\n"
                                           "3，检查总线接线是否正确？");
            return;
        }
        qDebug()<<"ret = "<<ret;
        scanNodeProcess.setValue(i);
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        if(scanNodeProcess.wasCanceled()){
            return;
        }
        startAddr++;
    }
    if(!GotNode){
        QMessageBox::warning(this,"警告","没有扫描到节点！\r\n"
                                       "1，请确定节点地址是在设置的节点地址范围？\r\n"
                                       "2，请确定从节点是否按照协议要求格式返回节点信息？\r\n"
                                       "3，请确定软件上数据收发ID设置跟节点固件数据收发ID设置一致？\r\n"
                                       "4，请确定软件上数据收发ID类型跟节点固件数据收发ID类型一致？");
    }
}


void MainWindow::on_contactUsAction_triggered()
{
    QString AboutStr;
#ifndef LANG_EN
    AboutStr.append(("官方网站<span style=\"font-size:12px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>：<a href=\"http://www.usbxyz.com\">www.usbxyz.com</a><br>"));
    AboutStr.append(("官方论坛<span style=\"font-size:12px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>：<a href=\"http://www.embed-net.com\">www.embed-net.com</a><br>"));
    AboutStr.append(("官方淘宝店<span style=\"font-size:9px;\">&nbsp;&nbsp;&nbsp;</span>：<a href=\"http://usb2xxx.taobao.com/\">usb2xxx.taobao.com</a><br>"));
    AboutStr.append(("技术支持QQ：188298598<br>"));
    AboutStr.append(("产品咨询QQ：188298598"));
    QMessageBox::about(this,("联系我们"),AboutStr);
#else
    AboutStr.append("Official website<span style=\"font-size:12px;\">&nbsp;</span>:&nbsp;<a href=\"http://www.usbxyz.com\">www.usbxyz.com</a><br>");
    AboutStr.append("Official BBS<span style=\"font-size:12px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>:&nbsp;<a href=\"http://www.embed-net.com\">www.embed-net.com</a><br>");
    AboutStr.append("Official Taobao<span style=\"font-size: 9px;\">&nbsp;</span>:&nbsp;<a href=\"http://usb2xxx.taobao.com/\">usb2xxx.taobao.com</a><br>");
    AboutStr.append("Technical Suuport QQ&nbsp;:&nbsp;188298598<br>");
    AboutStr.append("Product Consultant QQ&nbsp;:&nbsp;188298598");
    QMessageBox::about(this,"Contact Us",AboutStr);
#endif
}

void MainWindow::on_aboutAction_triggered()
{
    QString AboutStr;
    AboutStr.append("USB2XXX USB2CAN Bootloader Pro 1.0.0<br>");
#ifndef LANG_EN
    AboutStr.append(("支持硬件：USB2XXX<br>"));
    AboutStr.append(("购买地址：<a href=\"http://usb2xxx.taobao.com/\">usb2xxx.taobao.com</a>"));
    QMessageBox::about(this,("关于USB2XXX USB2CAN Bootloader Pro"),AboutStr);
#else
    AboutStr.append("Hardware Suuport: USB2XXX<br>");
    AboutStr.append(("Purchase URL:<a href=\"http://usb2xxx.taobao.com/\">usb2xxx.taobao.com</a>"));
    QMessageBox::about(this,"About USB2XXX USB2CAN Bootloader",AboutStr);
#endif
}

void MainWindow::on_exitAction_triggered()
{
    this->close();
}



void MainWindow::on_actionAddNode_triggered()
{
    DialogAddNode *pDialogAddNode = new DialogAddNode(this);
    if(pDialogAddNode->exec() == QDialog::Accepted){
        ui->nodeListTableWidget->setRowCount(ui->nodeListTableWidget->rowCount()+1);
        ui->nodeListTableWidget->setRowHeight(ui->nodeListTableWidget->rowCount()-1,20);
        //选择
        QHBoxLayout *hLayout = new QHBoxLayout();
        QCheckBox *ckb = new QCheckBox();
        QWidget *widget = new QWidget(ui->nodeListTableWidget);
        ckb->setChecked(true);
        hLayout->addWidget(ckb);
        hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
        hLayout->setAlignment(ckb, Qt::AlignCenter);
        widget->setLayout(hLayout);
        ui->nodeListTableWidget->setCellWidget(ui->nodeListTableWidget->rowCount()-1, 0, widget);
        //节点地址
        QString str;
        str.sprintf("0x%X",pDialogAddNode->NodeAddr);
        QTableWidgetItem *item = new QTableWidgetItem(str);
        ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,1,item);
        //固件类型
        if(pDialogAddNode->NodeType == FW_TYPE_BOOT){
            str = "BOOT";
        }else{
            str = "APP";
        }
        item = new QTableWidgetItem(str);
        ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,2,item);
        //固件版本
        str.sprintf("v%d.%d.%d.%d",0,0,0,0);
        item = new QTableWidgetItem(str);
        ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,3,item);
        //更新状态
        ui->nodeListTableWidget->setItem(ui->nodeListTableWidget->rowCount()-1,4,new QTableWidgetItem("等待更新"));
    }
}

void MainWindow::on_comboBoxIDType_currentIndexChanged(int index)
{
    if(index){
        ui->spinBoxSendID->setMaximum(0x1FFFFFFF);
        ui->spinBoxReceiveID->setMaximum(0x1FFFFFFF);
    }else{
        ui->spinBoxSendID->setMaximum(0x7FF);
        ui->spinBoxReceiveID->setMaximum(0x7FF);
    }
}

void MainWindow::on_actionScanDevice_triggered()
{
    ScanDevice();
}

void MainWindow::on_actionDocument_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("Bootloader协议.pdf"));
}
