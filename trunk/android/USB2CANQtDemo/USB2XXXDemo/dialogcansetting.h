#ifndef DIALOGCANSETTING_H
#define DIALOGCANSETTING_H

#include <QDialog>

namespace Ui {
class DialogCANSetting;
}

class DialogCANSetting : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCANSetting(QWidget *parent = 0);
    ~DialogCANSetting();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DialogCANSetting *ui;

public:
    int CANIndex;
    int CANBaudRate;
    int CANWorkMode;
    bool CANENRegister;
};

#endif // DIALOGCANSETTING_H
