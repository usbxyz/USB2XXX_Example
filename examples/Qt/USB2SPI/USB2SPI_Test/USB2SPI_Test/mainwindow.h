#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "usb_device.h"
#include "usb2spi.h"
#include <QMessageBox>
#include <QFileDialog>

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
    bool initDevice();
    void data_display_hex(uint8_t *pData, int32_t len, bool write);

private slots:
    void on_pushButtonWrite_clicked();

    void on_pushButtonRead_clicked();

    void on_pushButtonWriteRead_clicked();

    void on_pushButtonSelectFile_clicked();

    void on_pushButtonSendFile_clicked();

private:
    Ui::MainWindow *ui;
    bool needInitFlag;
    SPI_CONFIG SPI_Config;
};

#endif // MAINWINDOW_H
