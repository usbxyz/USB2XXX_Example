"""
文件说明：USB2XXX LIN相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2lin import *


if __name__ == '__main__': 
    LINMasterIndex = 0
    DevHandles = (c_uint * 20)()
    # Scan device
    ret = USB_ScanDevice(byref(DevHandles))
    if(ret == 0):
        print("No device connected!")
        exit()
    else:
        print("Have %d device connected!"%ret)
    # Open device
    ret = USB_OpenDevice(DevHandles[0])
    if(bool(ret)):
        print("Open device success!")
    else:
        print("Open device faild!")
        exit()
    # Get device infomation
    USB2XXXInfo = DEVICE_INFO()
    USB2XXXFunctionString = (c_char * 256)()
    ret = DEV_GetDeviceInfo(DevHandles[0],byref(USB2XXXInfo),byref(USB2XXXFunctionString))
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

    # 初始化配置主LIN
    LINConfig = LIN_CONFIG()
    LINConfig.BaudRate = 19200
    LINConfig.BreakBits = LIN_BREAK_BITS_11
    LINConfig.CheckMode = LIN_CHECK_MODE_EXT
    LINConfig.MasterMode = LIN_MASTER
    ret = LIN_Init(DevHandles[0],LINMasterIndex,byref(LINConfig))
    if ret != LIN_SUCCESS:
        print("Config Master LIN failed!")
        exit()
    else:
        print("Config Master LIN Success!")
    #发送BREAK信号，一般用于唤醒设备
    ret = LIN_SendBreak(DevHandles[0],LINMasterIndex)
    if ret != LIN_SUCCESS:
        print("Send LIN break failed!")
        exit()
    else:
        print("Send LIN break success!")
    sleep(0.01)

    # 主机写数据
    data_buffer = (c_byte*8)(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88)
    LINMsg = LIN_MSG()
    LINMsg.ID = 0x00
    LINMsg.DataLen = 8
    for i in range(0,LINMsg.DataLen):
        LINMsg.Data[i] = data_buffer[i]
    ret = LIN_Write(DevHandles[0],LINMasterIndex,byref(LINMsg),1)
    if ret != LIN_SUCCESS:
        print("LIN ID[0x%02X] write data failed!"%LINMsg.ID)
        exit()
    else:
        print("M2S","[0x%02X] "%LINMsg.ID,end='')
        for i in range(LINMsg.DataLen):
            print("0x%02X "%LINMsg.Data[i],end='')
        print("")
    sleep(0.01)
    

    # 主机读数据
    LINMsg.ID = 0x01
    ret = LIN_Read(DevHandles[0],LINMasterIndex,byref(LINMsg),1)
    if ret != LIN_SUCCESS:
        print("LIN read data failed!")
        exit()
    else:
        print("S2M","[0x%02X] "%LINMsg.ID,end='')
        for i in range(LINMsg.DataLen):
            print("0x%02X "%LINMsg.Data[i],end='')
        print("")

    # Close device
    ret = USB_CloseDevice(DevHandles[0])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
