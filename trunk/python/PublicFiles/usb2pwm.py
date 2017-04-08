"""
文件说明：USB2XXX PWM操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# Error code define
PWM_SUCCESS = 0             # success
PWM_ERR_NOT_SUPPORT = -1    # USB2XXX not support
PWM_ERR_USB_WRITE_FAIL = -2 # USB write data error
PWM_ERR_USB_READ_FAIL = -3  # USB read data error
PWM_ERR_CMD_FAIL = -4       # execute function error

# PWM初始化配置参数
class PWM_CONFIG(Structure):  
    _fields_ = [
        ("Prescaler", c_ushort*8),   # 预分频器
        ("Precision", c_ushort*8),   # 占空比调节精度
        ("Pulse", c_ushort*8),       # 占空比，实际占空比=(Pulse/Precision)*100%
        ("Polarity",c_byte*8),       # 波形极性
        ("ChannelMask",c_byte)       # 使能的通道号，每个通道对应一个bit位，bit0对应PWM_CH1
    ]


# Initialize pwm
def PWM_Init(DevHandle,pConfig):
    return USB2XXXLib.PWM_Init(DevHandle,pConfig)

# 启动PWM
def PWM_Start(DevHandle,ChannelMask,RunTimeOfUs):
    return USB2XXXLib.PWM_Start(DevHandle,ChannelMask,RunTimeOfUs)

# 停止PWM
def PWM_Stop(DevHandle,ChannelMask):
    return USB2XXXLib.PWM_Stop(DevHandle,ChannelMask)
