#include "dialogcansetting.h"
#include "ui_dialogcansetting.h"

DialogCANSetting::DialogCANSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCANSetting)
{
    ui->setupUi(this);
}

DialogCANSetting::~DialogCANSetting()
{
    delete ui;
}

void DialogCANSetting::on_pushButton_clicked()
{
    CANIndex = ui->comboBoxCANIndex->currentIndex();
    CANBaudRate = ui->comboBoxBaudRate->currentText().remove("Kbps").toInt(NULL,10)*1000;
    CANENRegister = ui->checkBox->isChecked();
    CANWorkMode = ui->comboBoxWorkMode->currentIndex()|(CANENRegister?0x80:0x00);
    this->accept();
}
