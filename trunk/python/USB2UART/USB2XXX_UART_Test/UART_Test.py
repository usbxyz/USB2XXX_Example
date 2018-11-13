"""
文件说明：USB2XXX IIC相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2uart import *

if __name__ == '__main__': 
    DevHandles = (c_uint * 20)()
    DevIndex = 0
    UARTIndex = 0
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
    # 设置GPIO输入输出电压，只针对带可变电压输出版本的适配器有用，其他适配器默认是3.3V
    DEV_SetPowerLevel(DevHandles[DevIndex],POWER_LEVEL_3V3)
    # Initialize UART
    UARTConfig = UART_CONFIG()
    UARTConfig.BaudRate = 115200
    UARTConfig.WordLength = UART_WORD_LENGTH_8BIT
    UARTConfig.StopBits = UART_STOP_BITS_1
    UARTConfig.Parity = UART_PARITY_NO
    UARTConfig.TEPolarity = UART_TE_DISEN

    # 初始化UART
    ret = UART_Init(DevHandles[DevIndex],UARTIndex,byref(UARTConfig));
    if ret != UART_SUCCESS:
        print("Initialize uart faild!")
        exit()
    else:
        print("Initialize uart sunccess!")
    # 发送数据
    WriteBuffer = (c_byte * 128)()
    for i in range(0,len(WriteBuffer)):
        WriteBuffer[i] = i
    ret = UART_WriteBytes(DevHandles[DevIndex],UARTIndex,byref(WriteBuffer),len(WriteBuffer))
    if ret != UART_SUCCESS:
        print("Write uart faild!")
        exit()
    else:
        print("Write uart sunccess!")
    sleep(0.01)
    # 读取接收到缓冲区中的数据
    ReadBuffer = (c_byte * 10240)()#接收数据缓冲区可以尽量大一点，以免缓冲区溢出
    ret = UART_ReadBytes(DevHandles[DevIndex],UARTIndex,byref(ReadBuffer),100)
    if ret < UART_SUCCESS:
        print("Read uart faild!")
        exit()
    else:
        print("Read uart sunccess!")
        print("Read Data:")
        for i in range(0,ret):
            print("%02X "%ReadBuffer[i],end='')
        print("")
    # Close device
    ret = USB_CloseDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
