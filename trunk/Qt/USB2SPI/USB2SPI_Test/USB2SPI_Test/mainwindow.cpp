#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    needInitFlag = true;
    ScanDevice();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ScanDevice(void)
{
    int DevHandle[20];
    int deviceNum = USB_ScanDevice(DevHandle);
    ui->comboBoxDeviceHandle->clear();
    for(int i=0;i<deviceNum;i++){
        ui->comboBoxDeviceHandle->addItem(QString().sprintf("%08X",DevHandle[i]));
    }
}

bool MainWindow::initDevice()
{
    int ret;
    bool state;
    int DevHandle = ui->comboBoxDeviceHandle->currentText().toInt(NULL,16);
    int SPIIndex;
    if(!needInitFlag){
        return true;
    }
    //获取SPI索引号
    SPIIndex = ui->comboBoxChannel->currentIndex();
    //扫描设备
    //打开设备
    state = USB_OpenDevice(DevHandle);
    if(!state){
        QMessageBox::warning(this,"警告","打开设备失败！");
        return false;
    }
    //初始化配置设备
    SPI_Config.Mode = SPI_MODE_HARD_HDX;
    SPI_Config.Master = SPI_MASTER;
    SPI_Config.CPHA = ui->comboBoxCPHA->currentIndex();
    SPI_Config.CPOL = ui->comboBoxCPOL->currentIndex();
    SPI_Config.ClockSpeedHz = ui->comboBoxClockSpeed->currentText().toInt(NULL,10);
    SPI_Config.LSBFirst = SPI_MSB;
    SPI_Config.SelPolarity = ui->comboBoxSelPolarity->currentIndex();
    ret = SPI_Init(DevHandle,SPIIndex,&SPI_Config);
    if(ret != SPI_SUCCESS){
        QMessageBox::warning(this,"警告","配置设备失败！");
        return false;
    }else{
        needInitFlag = false;
        //QMessageBox::information(this,"Information","Initialize device success!");
    }
    //此函数只针对电平可配置的适配器有用
    DEV_SetPowerLevel(DevHandle,POWER_LEVEL_3V3);
    return true;
}

void MainWindow::on_pushButtonWrite_clicked()
{
    int pack_size = 10240;//单次发送数据字节数
    uint8_t *write_buffer;
    QString writeStr;
    int write_byte_count = 0;
    int ret;
    int DevHandle = ui->comboBoxDeviceHandle->currentText().toInt(NULL,16);
    //获取SPI索引号
    int SPIIndex = ui->comboBoxChannel->currentIndex();
    if(initDevice()){
        //从界面获取数据并存放到发送数据缓冲区中
        writeStr = ui->textEdit->toPlainText();
        QStringList writeStrList = writeStr.split(" ",QString::SkipEmptyParts);
        write_byte_count = writeStrList.length();
        write_buffer = (uint8_t *)malloc(write_byte_count);
        for(int i=0;i<write_byte_count;i++){
            write_buffer[i] = writeStrList.at(i).toUInt(NULL,16);
        }
        //循环发送数据
        for(int i=0;i<write_byte_count;i+=pack_size){
            int write_data_size = ((i+pack_size)<write_byte_count)?pack_size:write_byte_count;
            //硬件半双工
            ret = SPI_WriteBytes(DevHandle,SPIIndex,write_buffer+i*pack_size,write_data_size);
            if(ret != SPI_SUCCESS){
                needInitFlag = true;
                QMessageBox::warning(this,"警告","写数据失败！");
                free(write_buffer);
                return;
            }else{
                data_display_hex(write_buffer+i*pack_size,write_data_size,true);
            }
        }
    }
}

void MainWindow::on_pushButtonRead_clicked()
{
    const int pack_size = 10240;//单次读数据最大字节数
    uint8_t read_buffer[40960];
    int read_byte_count = 0;
    int ret;
    int DevHandle = ui->comboBoxDeviceHandle->currentText().toInt(NULL,16);
    //获取SPI索引号
    int SPIIndex = ui->comboBoxChannel->currentIndex();
    if(initDevice()){
        read_byte_count = ui->spinBoxReadDataCount->value();
        for(int i=0;i<read_byte_count;i+=pack_size){
            int read_bytes = ((i+pack_size)<read_byte_count)?pack_size:read_byte_count;
            ret = SPI_ReadBytes(DevHandle,SPIIndex,read_buffer,read_bytes);
            if(ret != SPI_SUCCESS){
                needInitFlag = true;
                QMessageBox::warning(this,"警告","读数据失败！");
                return;
            }else{
                data_display_hex(read_buffer,read_bytes,false);
            }
        }
    }
}

void MainWindow::on_pushButtonWriteRead_clicked()
{
    uint8_t write_buffer[20480]={0};
    uint8_t read_buffer[20480]={0};
    int write_byte_count = 0;
    int read_byte_count = 0;
    QString writeStr;
    int ret;
    int DevHandle = ui->comboBoxDeviceHandle->currentText().toInt(NULL,16);
    //获取SPI索引号
    int SPIIndex = ui->comboBoxChannel->currentIndex();
    if(initDevice()){
        read_byte_count = ui->spinBoxReadDataCount->value();
        //从界面获取数据并存放到数据缓冲区中
        writeStr = ui->textEdit->toPlainText();
        QStringList writeStrList = writeStr.split(" ",QString::SkipEmptyParts);
        write_byte_count = writeStrList.length();
        for(int i=0;i<write_byte_count;i++){
            write_buffer[i] = writeStrList.at(i).toUInt(NULL,16);
        }
        //写读数据
        ret = SPI_WriteReadBytes(DevHandle,SPIIndex,write_buffer,write_byte_count,read_buffer,read_byte_count,0);
        if(ret != SPI_SUCCESS){
            needInitFlag = true;
            QMessageBox::warning(this,"警告","写读数据失败！");
            return;
        }else{
            //显示发送的数据
            data_display_hex(write_buffer,write_byte_count,true);
            //显示读取的数据
            data_display_hex(read_buffer,read_byte_count,false);
        }
    }
}

void MainWindow::data_display_hex(uint8_t *pData, int32_t len, bool write)
{
    if(write){
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->textBrowser->setTextColor(Qt::green);
        ui->textBrowser->insertPlainText("TX : ");
    }else{
        ui->textBrowser->moveCursor(QTextCursor::End);
        ui->textBrowser->setTextColor(Qt::blue);
        ui->textBrowser->insertPlainText("RX : ");
    }
    QString dspStr,strTmp;
    for(int i=0;i<len;i++){
        dspStr += strTmp.sprintf("%02X ",pData[i]);
    }
    ui->textBrowser->moveCursor(QTextCursor::End);
    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->insertPlainText(dspStr);
    ui->textBrowser->append("");
}

void MainWindow::on_pushButtonSelectFile_clicked()
{
    QString fileName;
    fileName=QFileDialog::getOpenFileName(this,
                                          "打开文件",
                                          "",
                                          "All Files (*.*)");
    if(fileName.isNull()){
        return;
    }
    ui->lineEditFileName->setText(fileName);
}

void MainWindow::on_pushButtonSendFile_clicked()
{
    int pack_size = 10240;//单次发送数据字节数
    uint8_t write_buffer[pack_size];
    int write_byte_count = 0;
    int ret;
    int DevHandle = ui->comboBoxDeviceHandle->currentText().toInt(NULL,16);
    //获取SPI索引号
    int SPIIndex = ui->comboBoxChannel->currentIndex();
    //打开文件
    QFile WriteDataFile(ui->lineEditFileName->text());
    if(!WriteDataFile.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"警告","打开文件失败！");
        return;
    }
    if(initDevice()){
        //循环发送数据
        while(!WriteDataFile.atEnd()){
            //从文件读取数据
            write_byte_count = WriteDataFile.read((char*)write_buffer,pack_size);
            //硬件半双工
            ret = SPI_WriteBytes(DevHandle,SPIIndex,write_buffer,write_byte_count);
            if(ret != SPI_SUCCESS){
                needInitFlag = true;
                WriteDataFile.close();
                QMessageBox::warning(this,"警告","写数据失败！");
                return;
            }else{
                data_display_hex(write_buffer,write_byte_count,true);
            }
        }
    }
    WriteDataFile.close();
}
