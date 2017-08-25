"""
文件说明：USB2XXX设备操作相关函数集合
更多帮助：www.usbxyz.com
"""

from ctypes import *
import platform

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

# Import library
if(platform.system()=="Windows"):
    if "64bit" in platform.architecture():
        USB2XXXLib = windll.LoadLibrary( ".\\lib\\windows\\x86_64\\USB2XXX.dll" )
    else:
        USB2XXXLib = windll.LoadLibrary( ".\\lib\\windows\\x86_32\\USB2XXX.dll" )
elif(platform.system()=="Darwin"):
    USB2XXXLib = cdll.LoadLibrary( "./lib/macos/libUSB2XXX.dylib" )
elif(platform.system()=="Linux"):
    if "64bit" in platform.architecture():
        USB2XXXLib = cdll.LoadLibrary( "./lib/linux/x86_64/libUSB2XXX.so" )
    else:
        USB2XXXLib = cdll.LoadLibrary( "./lib/linux/x86_32/libUSB2XXX.so" )
else:
    print("unsupported system")
    exit()

# Scan device
def USB_ScanDevice(pDevHandle):
    return USB2XXXLib.USB_ScanDevice(pDevHandle)

# Open device
def USB_OpenDevice(DevHandle):
    return USB2XXXLib.USB_OpenDevice(DevHandle)

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