"""
文件说明：USB2XXX GPIO操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# Error code define
GPIO_SUCCESS = 0             # success
GPIO_ERR_NOT_SUPPORT = -1    # USB2XXX not support
GPIO_ERR_USB_WRITE_FAIL = -2 # USB write data error
GPIO_ERR_USB_READ_FAIL = -3  # USB read data error
GPIO_ERR_CMD_FAIL = -4       # execute function error


# 将GPIO设置为输入模式
def GPIO_SetInput(DevIndex,PinMask,PuPd):
    return USB2XXXLib.GPIO_SetInput(DevIndex,PinMask,PuPd)
# 将GPIO设置为输出模式
def GPIO_SetOutput(DevIndex,PinMask,PuPd):
    return USB2XXXLib.GPIO_SetOutput(DevIndex,PinMask,PuPd)
# 将GPIO设置为开漏模式（可做双向引脚）
def GPIO_SetOpenDrain(DevIndex,PinMask,PuPd):
    return USB2XXXLib.GPIO_SetOpenDrain(DevIndex,PinMask,PuPd)
# 控制GPIO输出高电平或者低电平
def GPIO_Write(DevIndex,PinMask,PinValue):
    return USB2XXXLib.GPIO_Write(DevIndex,PinMask,PinValue)
# 读取GPIO引脚状态
def GPIO_Read(DevIndex,PinMask,pPinValue):
    return USB2XXXLib.GPIO_Read(DevIndex,PinMask,pPinValue)

