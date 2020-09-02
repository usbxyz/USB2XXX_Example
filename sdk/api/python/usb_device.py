"""
文件说明：USB2XXX设备操作相关函数集合
更多帮助：www.toomoss.com
使用说明：程序正常运行，需要将sdk/libs目录复制到程序目录下
"""

from ctypes import *
import platform
import os
import shutil

# Device info define
class DEVICE_INFO(Structure):
    _fields_ = [
        ("FirmwareName", c_char*32),   # firmware name string
        ("BuildDate", c_char*32),      # firmware build date and time string
        ("HardwareVersion", c_uint),   # hardware version
        ("FirmwareVersion",c_uint),    # firmware version
        ("SerialNumber",c_uint*3),     # USB2XXX serial number
        ("Functions",c_uint)           # USB2XXX functions
    ]

# 定义电压输出值
POWER_LEVEL_NONE    = 0 # 不输出
POWER_LEVEL_1V8     = 1 # 输出1.8V
POWER_LEVEL_2V5     = 2 # 输出2.5V
POWER_LEVEL_3V3     = 3 # 输出3.3V
POWER_LEVEL_5V0     = 4 # 输出5.0V

#复制库文件到程序目录下
if(not os.path.exists("libs")):
    if(os.path.exists("../../../../sdk/libs")):
        shutil.copytree("../../../../sdk/libs", "./libs")
    else:
        print("libs does not exist,You need to manually copy the libs directory to the current directory")
        exit()

#根据系统自动导入对应的库文件，若没能识别到正确的系统，可以修改下面的源码
if(platform.system()=="Windows"):
    if "64bit" in platform.architecture():
        windll.LoadLibrary(os.getcwd()+"/libs/msvc_x64/libusb-1.0.dll" )
        USB2XXXLib = windll.LoadLibrary(os.getcwd()+"/libs/msvc_x64/USB2XXX.dll" )
    else:
        windll.LoadLibrary(os.getcwd()+"/libs/msvc_x86/libusb-1.0.dll" )
        USB2XXXLib = windll.LoadLibrary(os.getcwd()+"/libs/msvc_x86/USB2XXX.dll" )
elif(platform.system()=="Darwin"):
    cdll.LoadLibrary(os.getcwd()+"/libs/mac_os/libusb-1.0.dylib" )
    USB2XXXLib = cdll.LoadLibrary(os.getcwd()+"/libs/mac_os/libUSB2XXX.dylib" )
elif(platform.system()=="Linux"):
    if 'arm' in platform.machine():
        cdll.LoadLibrary(os.getcwd()+"/libs/Linux_ARMv7/libusb-1.0.so" )
        USB2XXXLib = cdll.LoadLibrary(os.getcwd()+"/libs/Linux_ARMv7/libUSB2XXX.so" )
    else:
        if "64bit" in platform.architecture():
            cdll.LoadLibrary(os.getcwd()+"/libs/Linux_x64/libusb-1.0.so" )
            USB2XXXLib = cdll.LoadLibrary(os.getcwd()+"/lib/Linux_x64/libUSB2XXX.so" )
        else:
            cdll.LoadLibrary(os.getcwd()+"/libs/Linux_x86/libusb-1.0.so" )
            USB2XXXLib = cdll.LoadLibrary(os.getcwd()+"/libs/Linux_x86/libUSB2XXX.so" )
else:
    print("unsupported system")
    exit()

# Scan device
def USB_ScanDevice(pDevHandle):
    return USB2XXXLib.USB_ScanDevice(pDevHandle)

# Open device
def USB_OpenDevice(DevHandle):
    return USB2XXXLib.USB_OpenDevice(DevHandle)

# Reset device
def USB_ResetDevice(DevHandle):
    return USB2XXXLib.USB_ResetDevice(DevHandle)

# Get USB2XXX infomation
def DEV_GetDeviceInfo(DevHandle, pDevInfo, pFunctionStr):
    return USB2XXXLib.DEV_GetDeviceInfo(DevHandle, pDevInfo, pFunctionStr)

# Close device
def USB_CloseDevice(DevHandle):
    return USB2XXXLib.USB_CloseDevice(DevHandle)

def DEV_EraseUserData(DevHandle):
    return USB2XXXLib.DEV_EraseUserData(DevHandle)

def DEV_WriteUserData(DevHandle,OffsetAddr,pWriteData,DataLen):
    return USB2XXXLib.DEV_WriteUserData(DevHandle,OffsetAddr,pWriteData,DataLen)

def DEV_ReadUserData(DevHandle,OffsetAddr,pReadData,DataLen):
    return USB2XXXLib.DEV_ReadUserData(DevHandle,OffsetAddr,pReadData,DataLen)

def DEV_SetPowerLevel(DevHandle,PowerLevel):
    return USB2XXXLib.DEV_SetPowerLevel(DevHandle,PowerLevel)

def DEV_GetTimestamp(DevHandle,BusType,pTimestamp):
    return USB2XXXLib.DEV_GetTimestamp(DevHandle,BusType,pTimestamp)


