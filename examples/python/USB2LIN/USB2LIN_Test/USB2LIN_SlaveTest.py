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
    LINSlaveIndex = 0
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

    # 初始化配置从LIN
    LINConfig = LIN_CONFIG()
    LINConfig.BaudRate = 19200;
    LINConfig.BreakBits = LIN_BREAK_BITS_11;
    LINConfig.CheckMode = LIN_CHECK_MODE_EXT;
    LINConfig.MasterMode = LIN_SLAVE;
    ret = LIN_Init(DevHandles[0],LINSlaveIndex,byref(LINConfig))
    if ret != LIN_SUCCESS:
        print("Config Slave LIN failed!")
        exit()
    else:
        print("Config Slave LIN Success!")
    # 设置从机LIN ID为0x00的操作模式为从接收数据模式，该ID必须接收LINSlaveData.DataLen字节的数据
    LINMsg = LIN_MSG()
    LINMsg.ID = 0x00
    LINMsg.DataLen = 0x09 #接收数据最大长度为9字节（包含校验数据）
    ret = LIN_SlaveSetIDMode(DevHandles[0],LINSlaveIndex,LIN_SLAVE_READ,byref(LINMsg),1)
    if ret != LIN_SUCCESS:
        print("Set LIN ID[0x%02X] operation mode failed!"%LINMsg.ID)
        exit()
    else:
        print("Set LIN ID[0x%02X] operation mode success!"%LINMsg.ID)
    # 设置从机LIN ID为0x01的操作模式为从发送数据模式，该ID必须接收LINSlaveData.DataLen字节的数据
    LINMsg = LIN_MSG()
    LINMsg.ID = 0x01
    LINMsg.DataLen = 0x09 #接收数据最大长度为9字节（包含校验数据）
    data_buffer = (c_byte*8)(0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08)
    for i in range(LINMsg.DataLen-1):
        LINMsg.Data[i] = data_buffer[i]
    ret = LIN_SlaveSetIDMode(DevHandles[0],LINSlaveIndex,LIN_SLAVE_WRITE,byref(LINMsg),1)
    if ret != LIN_SUCCESS:
        print("Set LIN ID[0x%02X] operation mode failed!"%LINMsg.ID)
        exit()
    else:
        print("Set LIN ID[0x%02X] operation mode success!"%LINMsg.ID)

    #读取接收到的数据
    LINMsgBuffer = (LIN_MSG*1024)()# 为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
    ret = LIN_SlaveGetData(DevHandles[0],LINSlaveIndex,LINMsgBuffer)
    if ret < LIN_SUCCESS:
        print("LIN slave read data error!")
        exit()
    elif ret == 0:
        print("LIN slave read no data!")
    else:
        print("LIN Slave Read Data:")
        for i in range(ret):
            print("S2S","[0x%02X] "%LINSlaveDataBuffer[i].ID,end='')
            for j in range(LINSlaveDataBuffer[i].DataLen):
                print("0x%02X "%LINSlaveDataBuffer[i].Data[j],end='')
            print("")

    # Close device
    ret = USB_CloseDevice(DevHandles[0])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
