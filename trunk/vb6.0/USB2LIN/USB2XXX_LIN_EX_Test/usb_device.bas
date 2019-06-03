Attribute VB_Name = "usb_device"

Public Type DEVICE_INFO
    FirmwareName(31) As Byte
    BuildDate(31) As Byte
    HardwareVersion As Long
    FirmwareVersion As Long
    SerialNumber(2) As Long
    Functions As Long
End Type

Declare Function USB_ScanDevice Lib "USB2XXX.dll" (ByVal pDevHandle As Long) As Long
Declare Function USB_OpenDevice Lib "USB2XXX.dll" (ByVal DevHandle As Long) As Boolean
Declare Function USB_CloseDevice Lib "USB2XXX.dll" (ByVal DevHandle As Long) As Boolean
Declare Function DEV_GetDeviceInfo Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByRef pDevInfo As DEVICE_INFO, ByRef pFunctionStr As Byte) As Boolean

Declare Function DEV_EraseUserData Lib "USB2XXX.dll" (ByVal DevHandle As Long) As Boolean
Declare Function DEV_WriteUserData Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal OffsetAddr As Long, ByVal pWriteData As Long, ByVal DataLen As Long) As Boolean
Declare Function DEV_ReadUserData Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal OffsetAddr As Long, ByVal pReadData As Long, ByVal DataLen As Long) As Boolean
Declare Function DEV_SetPowerLevel Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal PowerLevel As Byte) As Boolean
