"""
文件说明：USB2XXX IIC/I2C操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# Error code define
IIC_SUCCESS = 0             # success
IIC_ERR_NOT_SUPPORT = -1    # USB2XXX not support
IIC_ERR_USB_WRITE_FAIL = -2 # USB write data error
PWM_ERR_USB_READ_FAIL = -3  # USB read data error
IIC_ERR_CMD_FAIL = -4       # execute function error
IIC_ERR_PARA_ERROR = -5     # parameter error

IIC_ERROR_CHANNEL       = 1 # 该通道不支持该函数
IIC_ERROR_BUSY          = 2 # 总线忙
IIC_ERROR_START_FAILD   = 3 # 启动总线失败
IIC_ERROR_TIMEOUT       = 4 # 超时
IIC_ERROR_NACK          = 5 # 从机无应答
# IIC初始化配置参数
class IIC_CONFIG(Structure):  
    _fields_ = [
        ("ClockSpeed", c_uint),   # IIC时钟频率:单位为Hz
        ("OwnAddr", c_ushort),    # USB2XXX为从机时自己的地址
        ("Master", c_ubyte),      # 主从选择控制:0-从机，1-主机
        ("AddrBits",c_ubyte),     # 从机地址模式，7-7bit模式，10-10bit模式
        ("EnablePu",c_ubyte)      # 1-使能引脚芯片内部上拉电阻，0-不使能，I2C总线上必须接上拉电阻
    ]
# 初始化I2C总线
def IIC_Init(DevHandle,IICIndex, pConfig):
    return USB2XXXLib.IIC_Init(DevHandle,IICIndex, pConfig)
# 获取总线上能正常应答的设备地址列表
def IIC_GetSlaveAddr(DevHandle,IICIndex,pSlaveAddr):
    return USB2XXXLib.IIC_GetSlaveAddr(DevHandle,IICIndex,pSlaveAddr)
# 主机模式写数据
def IIC_WriteBytes(DevHandle,IICIndex,SlaveAddr,pWriteData,WriteLen,TimeOutMs):
    return USB2XXXLib.IIC_WriteBytes(DevHandle,IICIndex,SlaveAddr,pWriteData,WriteLen,TimeOutMs)
# 主机模式读数据
def IIC_ReadBytes(DevHandle,IICIndex,SlaveAddr,pReadData,ReadLen,TimeOutMs):
    return USB2XXXLib.IIC_ReadBytes(DevHandle,IICIndex,SlaveAddr,pReadData,ReadLen,TimeOutMs)
# 主机模式写读数据，写数据和读数据之间有重复启动信号，无停止信号
def IIC_WriteReadBytes(DevHandle,IICIndex,SlaveAddr,pWriteData,WriteLen,pReadData,ReadLen,TimeOutMs):
    return USB2XXXLib.IIC_WriteReadBytes(DevHandle,IICIndex,SlaveAddr,pWriteData,WriteLen,pReadData,ReadLen,TimeOutMs)
# 从机模式写数据
def IIC_SlaveWriteBytes(DevHandle,IICIndex,pWriteData,WriteLen,TimeOutMs):
    return USB2XXXLib.IIC_SlaveWriteBytes(DevHandle,IICIndex,pWriteData,WriteLen,TimeOutMs)
# 从机模式接收数据
def IIC_SlaveReadBytes(DevHandle,IICIndex,pReadData,TimeOutMs):
    return USB2XXXLib.IIC_SlaveReadBytes(DevHandle,IICIndex,pReadData,TimeOutMs)
# 获取从机写数据模式下，发送数据缓冲区中遗留的数据
def IIC_SlaveWriteRemain(DevHandle,IICIndex):
    return USB2XXXLib.IIC_SlaveWriteRemain(DevHandle,IICIndex)
