#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include "scandevrangedialog.h"
#include "usb_device.h"
#include "dialogaddnode.h"
#include "can_bootloader.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void ScanDevice(void);
    bool DeviceConfig(void);
    void resizeEvent(QResizeEvent *pEvent);
    QString GetErrorString(int ErrorCode);
private slots:
    void on_openFirmwareFilePushButton_clicked();

    void on_updateFirmwarePushButton_clicked();

    void on_openFirmwareFileAction_triggered();

    void on_scanNodeAction_triggered();

    void on_contactUsAction_triggered();

    void on_aboutAction_triggered();

    void on_exitAction_triggered();

    void on_actionAddNode_triggered();

    void on_comboBoxIDType_currentIndexChanged(int index);

    void on_actionScanDevice_triggered();

    void on_actionDocument_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
