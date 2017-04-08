"""
文件说明：USB2XXX ADC操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# Error code define
ADC_SUCCESS = 0             # success
ADC_ERR_NOT_SUPPORT = -1    # USB2XXX not support
ADC_ERR_USB_WRITE_FAIL = -2 # USB write data error
ADC_ERR_USB_READ_FAIL = -3  # USB read data error
ADC_ERR_CMD_FAIL = -4       # execute function error
ADC_ERR_CH_NO_INIT = -5     # the channel not initialize

# definition of ADC receive callback
if(platform.system()=="Windows"):
    ADC_GET_DATA_HANDLE = WINFUNCTYPE(c_int,c_int,c_void_p,c_int)# First argument is return type
else:
    ADC_GET_DATA_HANDLE = CFUNCTYPE(c_int,c_int,c_void_p,c_int)# First argument is return type

# Initialize adc
def ADC_Init(DevHandle,Channel,SampleRateHz):
    return USB2XXXLib.ADC_Init(DevHandle,Channel,SampleRateHz)

# Read adc data
def ADC_Read(DevHandle,pData,DataNum):
    return USB2XXXLib.ADC_Read(DevHandle,pData,DataNum)

# Start continue read data
def ADC_StartContinueRead(DevHandle,Channel,SampleRateHz, FrameSize, pGetDataHandle):
    return USB2XXXLib.ADC_StartContinueRead(DevHandle,Channel, SampleRateHz, FrameSize, pGetDataHandle)

# Stop continue read data
def ADC_StopContinueRead(DevHandle):
    return USB2XXXLib.ADC_StopContinueRead(DevHandle)

# Get data from buffer
def ADC_GetData(DevHandle,pDataBuffer, BufferSize):
    return USB2XXXLib.ADC_GetData( DevHandle, pDataBuffer, BufferSize)