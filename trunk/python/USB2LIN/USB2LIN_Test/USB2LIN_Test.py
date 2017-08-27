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
    LINSlaveIndex = 1
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
    LINConfig.BaudRate = 10000
    LINConfig.BreakBits = LIN_BREAK_BITS_11
    LINConfig.CheckMode = LIN_CHECK_MODE_EXT
    LINConfig.MasterMode = LIN_MASTER
    ret = LIN_Init(DevHandles[0],LINMasterIndex,byref(LINConfig))
    if ret != LIN_SUCCESS:
        print("Config Master LIN failed!")
        exit()
    else:
        print("Config Master LIN Success!")
    # 初始化配置从LIN
    LINConfig.BaudRate = 10000;
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
    LINSlaveData = SLAVE_LIN_DATA()
    LINSlaveData.DataLen = 8
    LINSlaveData.WorkMode = LIN_SLAVE_READ
    ID = 0X00
    ret = LIN_SlaveSetIDOperation(DevHandles[0],LINSlaveIndex,0x00,LINSlaveData)
    if ret != LIN_SUCCESS:
        print("Set LIN ID[0x%02X] operation mode failed!"%ID)
        exit()
    else:
        print("Set LIN ID[0x%02X] operation mode success!"%ID)
    # 设置从机LIN ID为0x01的操作模式为从发送数据模式，该ID必须接收LINSlaveData.DataLen字节的数据
    LINSlaveData.DataLen = 8
    LINSlaveData.WorkMode = LIN_SLAVE_WRITE
    ID = 0x01
    for i in range(0,LINSlaveData.DataLen):
        LINSlaveData.Data[i] = i
    ret = LIN_SlaveSetIDOperation(DevHandles[0],LINSlaveIndex,ID,LINSlaveData)
    if ret != LIN_SUCCESS:
        print("Set LIN ID[0x%02X] operation mode failed!"%ID)
        exit()
    else:
        print("Set LIN ID[0x%02X] operation mode success!"%ID)

    # 主机写数据
    write_buffer = (c_byte*8)()
    for i in range(0,8):
        write_buffer[i] = 8+i
    ID = 0x00
    ret = LIN_WriteData(DevHandles[0],LINMasterIndex,ID,write_buffer,8)
    if ret != LIN_SUCCESS:
        print("LIN ID[0x%02X] write data failed!"%ID)
        exit()
    else:
        print("LIN ID[0x%02X] write data success!"%ID)
    sleep(0.01)
    # 主机发送数据之后，可以读取从机接收到的数据
    LINSlaveDataBuffer = (SLAVE_LIN_DATA*1024)() # 为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
    ret = LIN_SlaveGetData(DevHandles[0],LINSlaveIndex,LINSlaveDataBuffer)
    if ret < LIN_SUCCESS:
        print("LIN slave read data error!")
        exit()
    elif ret == 0:
        print("LIN slave read no data!")
    else:
        print("LIN Slave Read Data:")
        for i in range(0,ret):
            print("Data[%d]: "%i,end='')
            print("ID = 0x%02X "%(LINSlaveDataBuffer[i].Data[0]&0x3F),end='')
            print("Data = ",end='')
            for j in range(0,LINSlaveDataBuffer[i].DataLen):
                print("0x%02X "%LINSlaveDataBuffer[i].Data[1+j],end='')
            print("")
    # 主机读数据
    ID = 0x01;
    read_buffer = (c_byte*9)()
    ret = LIN_ReadData(DevHandles[0],LINMasterIndex,ID,read_buffer)
    if ret < LIN_SUCCESS:
        print("LIN read data failed!")
        exit()
    elif ret == LIN_SUCCESS:
        print("LIN read data no data!")
        exit()
    else:
        print("LIN Master Read Data:")
        print("LIN Read: ",end='')
        print("ID = 0x%02X "%(ID&0x3F),end='')
        print("Data = ",end='')
        for i in range(0,ret):
            print("0x%02X "%read_buffer[i],end='')
        print("")

    # Close device
    ret = USB_CloseDevice(DevHandles[0])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
