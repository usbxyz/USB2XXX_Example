"""
文件说明：USB2XXX LIN操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# 定义函数返回错误代码
LIN_SUCCESS            = (0)   # 函数执行成功
LIN_ERR_NOT_SUPPORT    = (-1)  # 适配器不支持该函数
LIN_ERR_USB_WRITE_FAIL = (-2)  # USB写数据失败
LIN_ERR_USB_READ_FAIL  = (-3)  # USB读数据失败
LIN_ERR_CMD_FAIL       = (-4)  # 命令执行失败
LIN_ERR_CH_NO_INIT     = (-5)  # 该通道未初始化
LIN_ERR_READ_DATA      = (-6)  # LIN读数据失败
# LIN和校验模式
LIN_CHECK_MODE_STD    = 0
LIN_CHECK_MODE_EXT    = 1
LIN_CHECK_MODE_NONE   = 2
# BREAK位数定义
LIN_BREAK_BITS_10   = 0x00
LIN_BREAK_BITS_11   = 0x20
# 定义主从模式
LIN_MASTER         = 1
LIN_SLAVE          = 0
# 定义从机操作模式
LIN_SLAVE_WRITE    = 0
LIN_SLAVE_READ     = 1

# 定义初始化LIN初始化数据类型
class LIN_CONFIG(Structure):
    _fields_ = [
        ("BaudRate",c_uint),      # 波特率,最大20K
        ("CheckMode",c_ubyte),     # 校验模式，0-标准校验模式，1-增强校验模式（包含PID）
        ("MasterMode",c_ubyte),    # 主从模式，0-从模式，1-主模式
        ("BreakBits",c_ubyte),     # Break长度，0x00-10bit,0x20-11bit
    ]
# 设置从模式下ID操作模式
class LIN_MSG(Structure):
    _fields_ = [
        ("ID",c_ubyte),         #ID，取值范围0~0x3F
        ("DataLen",c_ubyte),    #发送数据时，代表发送数据的长度，不含校验数据，接收数据时，数据的长度，包含校验数据
        ("Data",c_ubyte*9),     # 数据存储区
    ]
def LIN_Init(DevHandle,Channel,pConfig):
    return USB2XXXLib.LIN_Init(DevHandle,Channel,pConfig)

def LIN_SendBreak(DevHandle,LINIndex):
    return USB2XXXLib.LIN_SendBreak(DevHandle,LINIndex)

def LIN_Write(DevHandle,Channel,pLINMsg,Len):
    return USB2XXXLib.LIN_Write(DevHandle,Channel,pLINMsg,Len)

def LIN_Read(DevHandle,Channel,pLINMsg,Len):
    return USB2XXXLib.LIN_Read(DevHandle,Channel,pLINMsg,Len)

def LIN_SlaveSetIDMode(DevHandle,LINIndex,IDMode,pLINMsg,Len):
    return USB2XXXLib.LIN_SlaveSetIDMode(DevHandle,LINIndex,IDMode,pLINMsg,Len)

def LIN_SlaveGetData(DevHandle,LINIndex,pLINMsg):
    return USB2XXXLib.LIN_SlaveGetData(DevHandle,LINIndex,pLINMsg)