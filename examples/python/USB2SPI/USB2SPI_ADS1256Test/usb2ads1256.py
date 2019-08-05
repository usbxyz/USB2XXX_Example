"""
文件说明：USB2XXX ADS1256操作相关函数集合
更多帮助：www.toomoss.com
"""
from ctypes import *
import platform
from usb_device import *

# Error code define
ADS1256_SUCCESS = 0             # success
ADS1256_ERR_NOT_SUPPORT = -1    # USB2XXX not support
ADS1256_ERR_USB_WRITE_FAIL = -2 # USB write data error
ADS1256_ERR_USB_READ_FAIL = -3  # USB read data error
ADS1256_ERR_CMD_FAIL = -4       # execute function error
ADS1256_ERR_CH_NO_INIT = -5     # 未初始化
# 定义ADS采样率
ADS1256_DRATE_30000SPS   = 0xF0
ADS1256_DRATE_15000SPS   = 0xE0
ADS1256_DRATE_7500SPS    = 0xD0
ADS1256_DRATE_3750SPS    = 0xC0
ADS1256_DRATE_2000SPS    = 0xB0
ADS1256_DRATE_1000SPS    = 0xA1
ADS1256_DRATE_500SPS     = 0x92
ADS1256_DRATE_100SPS     = 0x82
ADS1256_DRATE_60SPS      = 0x72
ADS1256_DRATE_50SPS      = 0x63
ADS1256_DRATE_30SPS      = 0x53
ADS1256_DRATE_25SPS      = 0x43
ADS1256_DRATE_15SPS      = 0x33
ADS1256_DRATE_10SPS      = 0x23
ADS1256_DRATE_5SPS       = 0x13
ADS1256_DRATE_2_5SPS     = 0x03
# IIC初始化配置参数
class ADS1256_CONFIG(Structure):
    _fields_ = [
        ("SampleRate", c_uint16),  # 采样率
        ("PGA", c_uint8),           # 内部增益，2^PGA
        ("BufferEn", c_uint8),     # 1：使能Buffer,0：禁止Buffer
        ("SampleMode",c_uint32),   # 两个bit控制一个通道，00：不使用该通道，01：该通道为单端模式，10：该通道为差分负极，11：该通道为差分正极
    ]
# 初始化配置ADS1256
def ADS1256_Init(DevHandle,Channel, pConfig):
    return USB2XXXLib.ADS1256_Init(DevHandle,Channel, pConfig)
# 启动ADS转换，调用该函数后，适配器会自动读取ADS转换数据
def ADS1256_Start(DevHandle,Channel):
    return USB2XXXLib.ADS1256_Start(DevHandle,Channel)
# 读取已经转换好的数据
def ADS1256_Read(DevHandle,Channel,pData):
    return USB2XXXLib.ADS1256_Read(DevHandle,Channel,pData)
# 停止ADS转换，调用该函数后，适配器会自动停止ADS转换数据
def ADS1256_Stop(DevHandle,Channel):
    return USB2XXXLib.ADS1256_Stop(DevHandle,Channel)
