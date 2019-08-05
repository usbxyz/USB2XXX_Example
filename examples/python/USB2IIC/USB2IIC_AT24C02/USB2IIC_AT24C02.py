"""
文件说明：USB2XXX IIC相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2iic import *

if __name__ == '__main__': 
    DevHandles = (c_uint * 20)()
    DevIndex = 0
    IICIndex = 0
    # Scan device
    ret = USB_ScanDevice(byref(DevHandles))
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
    # 设置GPIO输入输出电压，只针对带可变电压输出版本的适配器有用，其他适配器默认是3.3V
    DEV_SetPowerLevel(DevHandles[DevIndex],POWER_LEVEL_3V3)
    # Initialize i2c
    IICConfig = IIC_CONFIG()
    IICConfig.ClockSpeed = 400000
    IICConfig.Master = 1
    IICConfig.AddrBits = 7
    IICConfig.EnablePu = 1
    # 初始化IIC
    ret = IIC_Init(DevHandles[DevIndex],IICIndex,byref(IICConfig));
    if ret != IIC_SUCCESS:
        print("Initialize iic faild!")
        exit()
    else:
        print("Initialize iic sunccess!")
    # 扫描IIC总线上能正常应答的设备
    SlaveAddr = (c_ushort * 128)()
    SlaveAddrNum = IIC_GetSlaveAddr(DevHandles[DevIndex],IICIndex,byref(SlaveAddr))
    if SlaveAddrNum <= 0:
        print("Get iic address faild!")
        exit()
    else:
        print("Get iic address sunccess!")
        print("IIC addr:")
        for i in range(0,SlaveAddrNum):
            print("%02X "%SlaveAddr[i],end='')
        print("")
    # 向0x08地址开始连续写8字节数据
    WriteBuffer = (c_byte * 9)()
    WriteBuffer[0] = 0x08
    for i in range(0,8):
        WriteBuffer[1+i] = i
    ret = IIC_WriteBytes(DevHandles[DevIndex],IICIndex,0x50,byref(WriteBuffer),9,100)
    if ret != IIC_SUCCESS:
        print("Write iic faild!")
        exit()
    else:
        print("Write iic sunccess!")
    sleep(0.01)
    # 从0x08地址开始读取8字节数据
    ReadBuffer = (c_byte * 8)()
    WriteBuffer[0] = 0x08
    for i in range(0,8):
        WriteBuffer[1+i] = i
    ret = IIC_WriteReadBytes(DevHandles[DevIndex],IICIndex,0x50,byref(WriteBuffer),1,byref(ReadBuffer),8,100)
    if ret != IIC_SUCCESS:
        print("WriteRead iic faild!")
        exit()
    else:
        print("WriteRead iic sunccess!")
        print("Read Data:")
        for i in range(0,len(ReadBuffer)):
            print("%02X "%ReadBuffer[i],end='')
        print("")
    # Close device
    ret = USB_CloseDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
