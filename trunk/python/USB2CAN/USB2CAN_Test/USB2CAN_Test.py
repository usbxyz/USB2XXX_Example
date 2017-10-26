"""
文件说明：USB2XXX ADC相关函数测试程序
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from time import sleep
from usb_device import *
from usb2can import *


if __name__ == '__main__': 
    CANIndex = 0
    DevHandles = (c_uint * 20)()
    # Scan device
    ret = USB_ScanDevice(byref(DevHandles))
    if(ret == 0):
        print("No device connected!")
        exit()
    else:
        print("Have %d device connected!"%ret)
    # Open device
    ret = USB_OpenDevice(DevHandles[0])
    if(bool(ret)):
        print("Open device success!")
    else:
        print("Open device faild!")
        exit()
    # Get device infomation
    USB2XXXInfo = DEVICE_INFO()
    USB2XXXFunctionString = (c_char * 256)()
    ret = DEV_GetDeviceInfo(DevHandles[0],byref(USB2XXXInfo),byref(USB2XXXFunctionString))
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
    # 初始化CAN
    CANConfig = CAN_INIT_CONFIG()
    CANConfig.CAN_Mode = 1      # 1-自发自收模式，0-正常模式
    CANConfig.CAN_ABOM = 0
    CANConfig.CAN_NART = 1
    CANConfig.CAN_RFLM = 0
    CANConfig.CAN_TXFP = 1
    # 配置波特率,波特率 = 100M/(BRP*(SJW+BS1+BS2))
    CANConfig.CAN_BRP_CFG3 = 25;
    CANConfig.CAN_BS1_CFG1 = 2;
    CANConfig.CAN_BS2_CFG2 = 1;
    CANConfig.CAN_SJW = 1;
    ret = CAN_Init(DevHandles[0],CANIndex,byref(CANConfig))
    if(ret != CAN_SUCCESS):
        print("Config CAN failed!")
        exit()
    else:
        print("Config CAN Success!")
    # 配置过滤器，必须配置，否则可能无法收到数据
    CANFilter = CAN_FILTER_CONFIG()
    CANFilter.Enable = 1
    CANFilter.ExtFrame = 0
    CANFilter.FilterIndex = 0
    CANFilter.FilterMode = 0
    CANFilter.MASK_IDE = 0
    CANFilter.MASK_RTR = 0
    CANFilter.MASK_Std_Ext = 0
    ret = CAN_Filter_Init(DevHandles[0],CANIndex,byref(CANFilter))
    if(ret != CAN_SUCCESS):
        print("Config CAN Filter failed!")
        exit()
    else:
        print("Config CAN Filter Success!")
    # 发送CAN帧
    CanMsg = (CAN_MSG*5)()
    for i in range(0,5):
        CanMsg[i].ExternFlag = 0
        CanMsg[i].RemoteFlag = 0
        CanMsg[i].ID = i
        CanMsg[i].DataLen = 8
        for j in range(0,CanMsg[i].DataLen):
            CanMsg[i].Data[j] = (i<<4)|j
    SendedNum = CAN_SendMsg(DevHandles[0],CANIndex,byref(CanMsg),5)
    if SendedNum >= 0 :
        print("Success send frames:%d"%SendedNum)
    else:
        print("Send CAN data failed!")
    # Delay
    sleep(0.5)
    # 读取CAN数据
    CanMsgBuffer = (CAN_MSG*10240)()
    CanNum = CAN_GetMsg(DevHandles[0],CANIndex,byref(CanMsgBuffer))
    if CanNum > 0:
        print("CanNum = %d"%CanNum)
        for i in range(0,CanNum):
            print("CanMsg[%d].ID = %d"%(i,CanMsgBuffer[i].ID))
            print("CanMsg[%d].TimeStamp = %d"%(i,CanMsgBuffer[i].TimeStamp))
            print("CanMsg[%d].Data = "%i,end='')
            for j in range(0,CanMsgBuffer[i].DataLen):
                print("%02X "%CanMsgBuffer[i].Data[j],end='')
            print("")
    elif CanNum == 0:
        print("No CAN data!")
    else:
        print("Get CAN data error!")
    # Close device
    ret = USB_CloseDevice(DevHandles[0])
    if(bool(ret)):
        print("Close device success!")
    else:
        print("Close device faild!")
        exit()
