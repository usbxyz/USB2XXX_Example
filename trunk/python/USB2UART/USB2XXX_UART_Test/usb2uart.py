"""
文件说明：USB2XXX UART操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# Error code define
UART_SUCCESS = 0             # 函数执行成功
UART_ERR_NOT_SUPPORT = -1    # 适配器不支持该函数
UART_ERR_USB_WRITE_FAIL = -2 # USB写数据失败
UART_ERR_USB_READ_FAIL = -3  # USB读数据失败
UART_ERR_CMD_FAIL = -4       # 命令执行失败

#数据位宽
UART_WORD_LENGTH_8BIT = 0
UART_WORD_LENGTH_9BIT = 1

#停止位
UART_STOP_BITS_1 = 0
UART_STOP_BITS_05 = 1
UART_STOP_BITS_2 = 2
UART_STOP_BITS_15 = 3

#奇偶校验位
UART_PARITY_NO = 0
UART_PARITY_EVEN = 4
UART_PARITY_ODD = 6

#TE控制信号输出
UART_TE_DISEN  = 0x00
UART_TE_EN_LOW = 0x80
UART_TE_EN_HIGH = 0x81

#定义初始化UART初始化数据类型
class UART_CONFIG(Structure):
    _fields_ = [
        ("BaudRate", c_uint),       # 波特率
        ("WordLength", c_ubyte),    # 数据位宽，0-8bit,1-9bit
        ("StopBits", c_ubyte),      # 停止位宽，0-1bit,1-0.5bit,2-2bit,3-1.5bit
        ("Parity",c_ubyte),         # 奇偶校验，0-No,4-Even,6-Odd
        ("TEPolarity",c_ubyte),     # TE输出控制，0x80-输出TE信号，且低电平有效，0x81-输出TE信号，且高电平有效，0x00不输出TE信号
    ]

def UART_Init(DevHandle, Channel, UART_CONFIG):
    return USB2XXXLib.UART_Init(DevHandle, Channel, UART_CONFIG)

def UART_WriteBytes(DevHandle, Channel, pWriteData, DataSize):
    return USB2XXXLib.UART_WriteBytes(DevHandle, Channel, pWriteData, DataSize)

def UART_WriteBytesAsync(DevHandle, Channel, pWriteData, DataSize):
    return USB2XXXLib.UART_WriteBytesAsync(DevHandle, Channel, pWriteData, DataSize)

def UART_ReadBytes(DevHandle, Channel, pReadData, TimeOutMs):
    return USB2XXXLib.UART_ReadBytes(DevHandle, Channel, pReadData, TimeOutMs)