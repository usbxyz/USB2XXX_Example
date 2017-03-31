#-*- coding: utf-8 -*-
"""
文件说明：USB2XXX GPIO相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2gpio import *

if __name__ == '__main__':
    DevHandles = (c_int * 20)()
    DevHandle = 0
    # Scan device
    ret = USB_ScanDevice(byref(DevHandles))
    if(ret == 0):
        print("No device connected!")
        exit()
    else:
        print("Have %d device connected!"%ret)
        DevHandle = DevHandles[0]#选择设备0
    # Open device
    ret = USB_OpenDevice(DevHandle)
    if(bool(ret)):
        print("Open device success!")
    else:
        print("Open device faild!")
        exit()
    # Get device infomation
    USB2XXXInfo = DEVICE_INFO()
    USB2XXXFunctionString = (c_char * 256)()
    ret = DEV_GetDeviceInfo(DevHandle,byref(USB2XXXInfo),byref(USB2XXXFunctionString))
    if(bool(ret)):
        print("USB2XXX device infomation:")
        print("--Firmware Name: %s"%bytes(USB2XXXInfo.FirmwareName).decode('ascii'))
        print("--Firmware Version: v%d.%d.%d"%((USB2XXXInfo.FirmwareVersion>>24)&0xFF,(USB2XXXInfo.FirmwareVersion>>16)&0xFF,USB2XXXInfo.FirmwareVersion&0xFFFF))
        print("--Hardware Version: v%d.%d.%d"%((USB2XXXInfo.HardwareVersion>>24)&0xFF,(USB2XXXInfo.HardwareVersion>>16)&0xFF,USB2XXXInfo.HardwareVersion&0xFFFF))
        print("--Build Date: %s"%bytes(USB2XXXInfo.BuildDate).decode('ascii'))
        print("--Serial Number: ",end=' ')
        for i in range(0, len(USB2XXXInfo.SerialNumber)):
            print("%08X"%USB2XXXInfo.SerialNumber[i],end='')
        print("")
        print("--Function String: %s"%bytes(USB2XXXFunctionString.value).decode('ascii'))
    else:
        print("Get device infomation faild!")
        exit()
    # 输出测试没——上下拉
    GPIO_SetOutput(DevHandle,0xFFFF,0)
    for i in range(0,10):
        GPIO_Write(DevHandle,0xFFFF,0xAAAA)
        GPIO_Write(DevHandle,0xFFFF,0x5555)
    # 输出测试——上拉
    GPIO_SetOutput(DevHandle,0xFFFF,1)
    for i in range(0,10):
        GPIO_Write(DevHandle,0xFFFF,0xAAAA)
        GPIO_Write(DevHandle,0xFFFF,0x5555)
    # 输出测试——下拉
    GPIO_SetOutput(DevHandle,0xFFFF,2)
    for i in range(0,10):
        GPIO_Write(DevHandle,0xFFFF,0xAAAA)
        GPIO_Write(DevHandle,0xFFFF,0x5555)
    # 测试输入——浮空
    GPIO_SetInput(DevHandle,0xFFFF,0)
    PinValue = c_uint(0)
    GPIO_Read(DevHandle,0xFFFF,byref(PinValue))
    print("READ DATA(Float):%04X"%PinValue.value)
    # 测试输入——上拉输入
    GPIO_SetInput(DevHandle,0xFFFF,1)
    GPIO_Read(DevHandle,0xFFFF,byref(PinValue))
    print("READ DATA(Pu):%04X"%PinValue.value)
    # 测试输入——下拉输入
    GPIO_SetInput(DevHandle,0xFFFF,2)
    GPIO_Read(DevHandle,0xFFFF,byref(PinValue))
    print("READ DATA(Pd):%04X"%PinValue.value)
    # 测试开漏输入——浮空
    GPIO_SetOpenDrain(DevHandle,0xFFFF,0)
    GPIO_Read(DevHandle,0xFFFF,byref(PinValue))
    print("READ DATA(OD-Float):%04X"%PinValue.value)
    # 测试开漏输入——上拉输入
    GPIO_SetOpenDrain(DevHandle,0xFFFF,1)
    GPIO_Read(DevHandle,0xFFFF,byref(PinValue))
    print("READ DATA(OD-Pu):%04X"%PinValue.value)
    # 测试开漏输入——下拉输入
    GPIO_SetOpenDrain(DevHandle,0xFFFF,2)
    GPIO_Read(DevHandle,0xFFFF,byref(PinValue))
    print("READ DATA(OD-Pd):%04X"%PinValue.value)
    # Close device
    ret = USB_CloseDevice(DevHandle)
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
