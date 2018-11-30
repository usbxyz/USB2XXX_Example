Attribute VB_Name = "usb2iic"

Public Type IIC_CONFIG
    ClockSpeedHz As Long
    OwnAddr As Integer
    Master As Byte
    AddrBits As Byte
    EnablePu As Byte
End Type

Public Const IIC_SUCCESS As Long = 0
Public Const IIC_ERR_NOT_SUPPORT As Long = -1
Public Const IIC_ERR_USB_WRITE_FAIL As Long = -2
Public Const IIC_ERR_USB_READ_FAIL As Long = -3
Public Const IIC_ERR_CMD_FAIL As Long = -4
Public Const IIC_ERR_PARA_ERROR As Long = -5

Public Const IIC_ERROR_SUCCESS As Long = 0
Public Const IIC_ERROR_CHANNEL As Long = 1
Public Const IIC_ERROR_BUSY As Long = 2
Public Const IIC_ERROR_START_FAILD As Long = 3
Public Const IIC_ERROR_TIMEOUT As Long = 4
Public Const IIC_ERROR_NACK As Long = 5
Public Const IIC_ERROR_PEC As Long = 6

Declare Function IIC_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByRef pConfig As IIC_CONFIG) As Long
Declare Function IIC_GetSlaveAddr Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal pSlaveAddr As Any) As Long
Declare Function IIC_WriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal SlaveAddr As Integer, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal TimeOutMs As Long) As Long
Declare Function IIC_ReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal SlaveAddr As Integer, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal TimeOutMs As Long) As Long
Declare Function IIC_WriteReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal SlaveAddr As Integer, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal TimeOutMs As Long) As Long
Declare Function IIC_SlaveWriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal TimeOutMs As Long) As Long
Declare Function IIC_SlaveReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal pReadData As Any, ByVal TimeOutMs As Long) As Long
Declare Function IIC_SlaveWriteRemain Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long) As Long

Declare Function IIC_WriteBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal SlaveAddr As Integer, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfEventMs As Long) As Long
Declare Function IIC_ReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal SlaveAddr As Integer, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfEventMs As Long) As Long
Declare Function IIC_WriteReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal IICIndex As Long, ByVal SlaveAddr As Integer, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfEventMs As Long) As Long

