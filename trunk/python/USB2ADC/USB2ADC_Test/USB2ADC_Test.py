#-*- coding: utf-8 -*-
"""
文件说明：USB2XXX ADC相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2adc import *

# define callback function
def GetAdcDataHandle(DevIndex,pData,DataNum):
    print("Get ADC data number:%d"%DataNum)
    if(DataNum > 0):
        ADCData = (c_ushort * DataNum)()# 声明本地内存存储空间
        memmove(byref(ADCData),c_char_p(pData),DataNum*2)# 拷贝数据到本地，DataNum为ADC数量，每个ADC有2字节，所以得拷贝DataNum*2
        print("ADCData = %f V"%(ADCData[0]*3.3/4095))# print the voltage value
    return 0

if __name__ == '__main__': 
    DevIndex = 0
    ADCChannel = 0x01
    DevHandles = (c_uint * 20)()
    # Scan device
    ret = USB_ScanDevice(DevHandles)
    if(ret == 0):
        print("No device connected!")
        exit()
    else:
        print("Have %d device connected!"%ret)
    # Open device
    ret = USB_OpenDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Open device success!")
    else:
        print("Open device faild!")
        exit()
    # Get device infomation
    USB2XXXInfo = DEVICE_INFO()
    USB2XXXFunctionString = (c_char * 256)()
    ret = DEV_GetDeviceInfo(DevHandles[DevIndex],byref(USB2XXXInfo),byref(USB2XXXFunctionString))
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
    ret = ADC_Init(DevHandles[DevIndex],ADCChannel,1000000)
    if(ret != ADC_SUCCESS):
        print("Initialize adc faild!")
        exit()
    else:
        print("Initialize adc success")
    # Read adc value
    ADCData = (c_ushort * 8)()
    ret = ADC_Read(DevHandles[DevIndex],byref(ADCData),len(ADCData))
    if(ret != ADC_SUCCESS):
        print("Adc read data faild!")
        exit()
    else:
        print("Read Data:")
        for i in range(0,len(ADCData)):
            print("ADCData[%d] = %f V"%(i,ADCData[i]*3.3/4095))
    # Continue Read
    pGetAdcDataHandle = ADC_GET_DATA_HANDLE(GetAdcDataHandle)
    ret = ADC_StartContinueRead(DevHandles[DevIndex],ADCChannel,100000,1024,pGetAdcDataHandle)
    if(ret != ADC_SUCCESS):
        print("Start continue read adc faild!")
        exit()
    else:
        print("Start continue read adc success")
    # Delay
    sleep(5)
    ret = ADC_StopContinueRead(DevHandles[DevIndex])
    if(ret != ADC_SUCCESS):
        print("Stop continue read adc faild!")
        exit()
    else:
        print("Stop continue read adc success")
    # Close device
    ret = USB_CloseDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
