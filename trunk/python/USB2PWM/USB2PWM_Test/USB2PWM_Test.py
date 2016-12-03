"""
文件说明：USB2XXX PWM相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2pwm import *

if __name__ == '__main__': 
    DevIndex = 0
    # Scan device
    ret = USB_ScanDevice(0)
    if(ret == 0):
        print("No device connected!")
        exit()
    else:
        print("Have %d device connected!"%ret)
    # Open device
    ret = USB_OpenDevice(DevIndex)
    if(bool(ret)):
        print("Open device success!")
    else:
        print("Open device faild!")
        exit()
    # Get device infomation
    USB2XXXInfo = DEVICE_INFO()
    USB2XXXFunctionString = (c_char * 256)()
    ret = USB_GetDeviceInfo(DevIndex,byref(USB2XXXInfo),byref(USB2XXXFunctionString))
    if(bool(ret)):
        print("USB2XXX device infomation:")
        print("--Firmware Name: %s"%bytes(USB2XXXInfo.FirmwareName).decode('ascii'))
        print("--Firmware Version: v%d.%d.%d"%((USB2XXXInfo.FirmwareVersion>>24)&0xFF,(USB2XXXInfo.FirmwareVersion>>16)&0xFF,USB2XXXInfo.FirmwareVersion&0xFFFF))
        print("--Hardware Version: v%d.%d.%d"%((USB2XXXInfo.HardwareVersion>>24)&0xFF,(USB2XXXInfo.HardwareVersion>>16)&0xFF,USB2XXXInfo.HardwareVersion&0xFFFF))
        print("--Build Date: %s"%bytes(USB2XXXInfo.BuildDate).decode('ascii'))
        print("--Serial Number: ",end='')
        for i in range(0, len(USB2XXXInfo.SerialNumber)):
            print("%08X"%USB2XXXInfo.SerialNumber[i],end='')
        print("")
        print("--Function String: %s"%bytes(USB2XXXFunctionString.value).decode('ascii'))
    else:
        print("Get device infomation faild!")
        exit()
    # Initialize adc
    PWMConfig = PWM_CONFIG()
    PWMConfig.ChannelMask = 0xFF # 初始化所有通道
    for i in range(0,8):
        PWMConfig.Polarity[i] = 1 # 将所有PWM通道都设置为正极性
    for i in range(0,8):
        PWMConfig.Precision[i] = 100 # 将所有通道的占空比调节精度都设置为1%
    for i in range(0,8):
        PWMConfig.Prescaler[i] = 10 # 将所有通道的预分频器都设置为10，则PWM输出频率为200MHz/(PWMConfig.Precision*PWMConfig.Prescaler)
    for i in range(0,8):
        PWMConfig.Pulse[i] = PWMConfig.Precision[i]*30//100 # 将所有通道的占空比都设置为30%
    # 初始化PWM
    ret = PWM_Init(DevIndex,byref(PWMConfig));
    if ret != PWM_SUCCESS:
        print("Initialize pwm faild!")
        exit()
    else:
        print("Initialize pwm sunccess!")
    # 启动PWM,RunTimeOfUs之后自动停止，利用该特性可以控制输出脉冲个数，脉冲个数=RunTimeOfUs*200/(PWMConfig.Precision*PWMConfig.Prescaler)
    RunTimeOfUs = 10000
    ret = PWM_Start(DevIndex,PWMConfig.ChannelMask,RunTimeOfUs)
    if(ret != PWM_SUCCESS):
        print("Start pwm faild!")
        exit()
    else:
        print("Start pwm sunccess!")
    
    # 停止PWM
    # ret = PWM_Stop(DevIndex,PWMConfig.ChannelMask)
    # if(ret != PWM_SUCCESS):
    #     print("Stop pwm faild!");
    #     exit()
    # else:
    #     print("Stop pwm sunccess!")
    # Close device
    ret = USB_CloseDevice(DevIndex)
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
