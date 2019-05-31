"""
文件说明：USB2XXX LIN相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2lin_ex import *


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
    ret = LIN_EX_Init(DevHandles[0],LINMasterIndex,19200,LIN_EX_MASTER)
    if ret != LIN_EX_SUCCESS:
        print("Config Master LIN failed!")
        exit()
    else:
        print("Config Master LIN Success!")
    #发送BREAK信号，一般用于唤醒设备
    LINOutMsg = LIN_EX_MSG()
    LINMsg = LIN_EX_MSG()
    LINMsg.MsgType = LIN_EX_MSG_TYPE_BK #消息类型为同步间隔信号
    LINMsg.Timestamp = 10 #发送同步间隔信号之后延时10ms
    ret = LIN_EX_MasterSync(DevHandles[0],LINMasterIndex,byref(LINMsg),byref(LINOutMsg),1)
    if ret != 1:
        print("Send LIN break failed!")
        exit()
    else:
        print("Send LIN break success!")
    sleep(0.01)

    # 主机写数据
    data_buffer = (c_byte*8)(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88)
    LINMsg.MsgType = LIN_EX_MSG_TYPE_MW #消息类型为主机写数据
    LINMsg.Timestamp = 10 #发送完毕数据之后延时10ms
    LINMsg.PID = 0x02 #高2位的校验数据可以设置为0，底层会自动计算
    LINMsg.CheckType = LIN_EX_CHECK_EXT #使用增强型校验
    LINMsg.DataLen = 8
    for i in range(0,LINMsg.DataLen):
        LINMsg.Data[i] = data_buffer[i]
    ret = LIN_EX_MasterSync(DevHandles[0],LINMasterIndex,byref(LINMsg),byref(LINOutMsg),1)
    if ret != 1:
        print("LIN ID[0x%02X] write data failed!"%LINMsg.ID)
        exit()
    else:
        #显示发送的同时接收到的数据，若数据发送成功，那么接收到的数据应该是跟发送出去的数据一样，否则数据出现了冲突
        print("M2S","[0x%02X] "%LINOutMsg.PID,end='')
        for i in range(LINOutMsg.DataLen):
            print("0x%02X "%LINOutMsg.Data[i],end='')
        print("")
    sleep(0.01)
    

    # 主机读数据
    LINMsg.MsgType = LIN_EX_MSG_TYPE_MR #消息类型为主机读数据
    LINMsg.Timestamp = 10 #发送完毕数据之后延时10ms
    LINMsg.PID = 0x03 #高2位的校验数据可以设置为0，底层会自动计算
    ret = LIN_EX_MasterSync(DevHandles[0],LINMasterIndex,byref(LINMsg),byref(LINOutMsg),1)
    if ret != 1:
        print("LIN read data failed!")
        exit()
    else:
        print("S2M","[0x%02X] "%LINOutMsg.PID,end='')
        for i in range(LINOutMsg.DataLen):
            print("0x%02X "%LINOutMsg.Data[i],end='')
        print("")

    # Close device
    ret = USB_CloseDevice(DevHandles[0])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
