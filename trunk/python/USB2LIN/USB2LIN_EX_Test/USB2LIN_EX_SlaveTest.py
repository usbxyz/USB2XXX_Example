"""
文件说明：USB2XXX LIN相关函数测试程序
更多帮助：www.toomoss.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2lin_ex import *


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

    # 初始化配置从机LIN
    ret = LIN_EX_Init(DevHandles[0],LINSlaveIndex,19200,LIN_EX_SLAVE)
    if ret != LIN_EX_SUCCESS:
        print("Config Master LIN failed!")
        exit()
    else:
        print("Config Master LIN Success!")
    
    # 将ID=0x01设置为从机发送数据模式，这样主机发送帧头之后，从机就可以自动返回数据
    # 其他不设置ID模式的默认为从机接收数据模式，也就是获取主机发送过来的数据
    LINMsg = LIN_EX_MSG()
    LINMsg.PID = 0x01
    LINMsg.MsgType = LIN_EX_MSG_TYPE_SW
    LINMsg.CheckType = LIN_EX_CHECK_EXT #使用增强型校验
    data_buffer = (c_byte*8)(0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88)
    LINMsg.DataLen = 8
    for i in range(0,LINMsg.DataLen):
        LINMsg.Data[i] = data_buffer[i]
    ret = LIN_EX_SlaveSetIDMode(DevHandles[0],LINSlaveIndex,byref(LINMsg),1);
    if ret != LIN_EX_SUCCESS:
        print("Set LIN ID Mode failed!")
        exit()
    else:
        print("Set LIN ID Mode success!")
    sleep(0.01)

    # 循环接收LIN总线上的数据并显示出来
    GetDataTime = 10000 #循环10秒钟获取数据
    while GetDataTime > 0:
        LINOutMsg = (LIN_EX_MSG*1024)() #缓冲区尽量大一点，防止缓冲区溢出，程序异常崩溃
        ret = LIN_EX_SlaveGetData(DevHandles[0],LINSlaveIndex,LINOutMsg);
        if ret < LIN_EX_SUCCESS:
            print("LIN slave read data error!")
            exit()
        elif ret > 0:
            for i in range(ret):
                print("S2S","[0x%02X] "%LINOutMsg[i].PID,end='')
                for j in range(LINOutMsg[i].DataLen):
                    print("0x%02X "%LINOutMsg[i].Data[j],end='')
                print("")
        sleep(0.05)
        GetDataTime -= 50

    # Close device
    ret = USB_CloseDevice(DevHandles[0])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
