"""
文件说明：USB2XXX ADC相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2spi import *

def SlaveGetData(DevHandle,SPIIndex,pData, DataNum):
    print("SPI read data:")
    SPIData = (c_ubyte * DataNum)()# 声明本地内存存储空间
    memmove(byref(SPIData),c_char_p(pData),DataNum)# 拷贝数据到本地，DataNum为ADC数量，每个ADC有2字节，所以得拷贝DataNum*2
    for i in range(0,DataNum):
        print("%02X "%SPIData[i],end='')
    print("")
    return 0

if __name__ == '__main__': 
    SlaveIndex = SPI1_CS0
    DevIndex = 0
    DevHandles = (c_uint * 20)()
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
    # Initialize slave spi
    SPIConfig = SPI_CONFIG()
    SPIConfig.Mode = SPI_MODE_HARD_HDX      # 硬件半双工模式
    SPIConfig.Master = SPI_SLAVE    # 主机模式
    SPIConfig.CPOL = 0
    SPIConfig.CPHA = 0
    SPIConfig.LSBFirst = SPI_MSB
    SPIConfig.SelPolarity = SPI_SEL_LOW
    SPIConfig.ClockSpeedHz = 500000
    ret = SPI_Init(DevHandles[DevIndex],SlaveIndex,byref(SPIConfig))
    if(ret != SPI_SUCCESS):
        print("Initialize slave spi faild!")
        exit()
    else:
        print("Initialize slave spi success")
    
    # SPI slave read data
    ReadBuffer = (c_ubyte * 256)()
    ret = SPI_SlaveReadBytes(DevHandles[DevIndex],SlaveIndex,byref(ReadBuffer),256,5000)#接收256字节数据，若一直没有接收到则会一直等待，直到超时
    if(ret < SPI_SUCCESS):
        print("SPI read data faild!")
        exit()
    else:
        print("SPI read data:")
        for i in range(0,ret):
            print("%02X "%ReadBuffer[i],end='')
        print("")
    # 启动从机连续接收数据模式
    pSlaveGetDataHandle = SPI_GET_DATA_HANDLE(SlaveGetData)
    ret = SPI_SlaveContinueRead(DevHandles[DevIndex],SlaveIndex,pSlaveGetDataHandle)
    if(ret != SPI_SUCCESS):
        print("Start SPI slave continue read faild!")
        exit()
    else:
        print("Start SPI slave continue read success")
    # 连续接收10秒钟数据之后停止接收，10秒内主机可以发送数据给从机，从机接收到数据之后会自动调用回调函数
    sleep(10)
    # 停止连续接收数据
    ret = SPI_SlaveContinueReadStop(DevHandles[DevIndex],SlaveIndex)
    if(ret != SPI_SUCCESS):
        print("Stop SPI slave continue read faild!")
        exit()
    else:
        print("Stop SPI slave continue read success")
    # Close device
    ret = USB_CloseDevice(DevHandles[DevIndex])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
