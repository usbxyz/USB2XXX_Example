"""
文件说明：USB2XXX ADC相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2ads1256 import *
'''
硬件连接说明,注意蓝色端子的电源必须接上，否则可能会导致初始化失败
USB2XXX_CH1  USB2XXX_CH0   ADS1256|
----------------------------------|
P0           P8            DRDY   |
P1           P9            RESET  |
P4           P13           CS     |
P5           P12           SCLK   |
P6           P11           DOUT   |
P7           P10           DIN    |
3V3                        DVDD   |
GND                        DGND   |
'''

if __name__ == '__main__': 
    DevIndex = 0
    DevHandles = (c_uint * 20)()
    ADS1256Channel = 1;
    # Scan device
    ret = USB_ScanDevice(byref(DevHandles))
    if(ret == 0):
        print("No device connected!")
        exit()
    else:
        print("Have %d device connected!"%ret)
    # Open device
    ret = USB_OpenDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Open device success!")
    else:
        print("Open device faild!")
        exit()
    # Get device infomation
    USB2XXXInfo = DEVICE_INFO()
    USB2XXXFunctionString = (c_char * 256)()
    ret = DEV_GetDeviceInfo(DevHandles[DevIndex],byref(USB2XXXInfo),byref(USB2XXXFunctionString))
    if(bool(ret)):
        print("USB2XXX device infomation:")
        print("--Firmware Name: %s"%bytes(USB2XXXInfo.FirmwareName).decode('ascii'))
        print("--Firmware Version: v%d.%d.%d"%((USB2XXXInfo.FirmwareVersion>>24)&0xFF,(USB2XXXInfo.FirmwareVersion>>16)&0xFF,USB2XXXInfo.FirmwareVersion&0xFFFF))
        print("--Hardware Version: v%d.%d.%d"%((USB2XXXInfo.HardwareVersion>>24)&0xFF,(USB2XXXInfo.HardwareVersion>>16)&0xFF,USB2XXXInfo.HardwareVersion&0xFFFF))
        print("--Build Date: %s"%bytes(USB2XXXInfo.BuildDate).decode('ascii'))
        print("--Serial Number: ",end='')
        for i in range(0, len(USB2XXXInfo.SerialNumber)):
            print("%08X"%USB2XXXInfo.SerialNumber[i],end='')
        print("")
        print("--Function String: %s"%bytes(USB2XXXFunctionString.value).decode('ascii'))
    else:
        print("Get device infomation faild!")
        exit()
    # 初始化ADS1256
    ADS1256Config = ADS1256_CONFIG()
    ADS1256Config.BufferEn = 0;#不使能内部Buffer
    ADS1256Config.PGA = 0;#内部增益为1，具体增益值为2的PGA次方
    ADS1256Config.SampleRate = ADS1256_DRATE_1000SPS;#ADS1256数据采样率;
    ADS1256Config.SampleMode = 0x05;#单端模式，采集AIN0和AIN1信号
    ret = ADS1256_Init(DevHandles[DevIndex],ADS1256Channel,byref(ADS1256Config))
    if(ret != ADS1256_SUCCESS):
        print("Initialize ADS1256 faild!")
        exit()
    else:
        print("Initialize ADS1256 success")
    # 启动ADS1256
    ret = ADS1256_Start(DevHandles[DevIndex],ADS1256Channel);
    if(ret != ADS1256_SUCCESS):
        print("Start ADS1256 error!")
        exit()
    for t in range(0,1000):
        # 延时一段时间后读取数据
        sleep(0.01)
        DataBuffer =  (c_uint32 * 20480)()
        DataNum = 0
        ret = ADS1256_Read(DevHandles[DevIndex],ADS1256Channel,byref(DataBuffer))
        if(ret < 0):
            print("Read ADS1256 error!");
            exit()
        else:
            DataNum = ret
        # 打印数据
        for i in range(0, DataNum):
            print("ADCDataBuffer[d%][%d] = %f"%(t,i,(DataBuffer[i]*0.59604644775390625)/((1<<ADS1256Config.PGA)*(1000000))))
    # 停止ADS1256
    ret = ADS1256_Stop(DevHandles[DevIndex],ADS1256Channel);
    if(ret != ADS1256_SUCCESS):
        print("Stop ADS1256 error!")
        exit()
    print("ADS1256 Test End!")
    # Close device
    ret = USB_CloseDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
