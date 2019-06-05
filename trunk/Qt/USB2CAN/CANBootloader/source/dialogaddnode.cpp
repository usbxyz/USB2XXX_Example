#include "dialogaddnode.h"
#include "ui_dialogaddnode.h"
#include "can_bootloader.h"
DialogAddNode::DialogAddNode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddNode)
{
    ui->setupUi(this);
    NodeAddr = 1;
}

DialogAddNode::~DialogAddNode()
{
    delete ui;
}

void DialogAddNode::on_pushButtonOK_clicked()
{
    NodeAddr = ui->spinBoxNodeAddr->value();
    NodeType = ui->comboBoxNodeType->currentText()=="BOOT"?FW_TYPE_BOOT:FW_TYPE_APP;
    this->accept();
}

void DialogAddNode::on_pushButtonCancel_clicked()
{
    this->reject();
}
