

Imports System.Runtime.InteropServices

Module usb_device
    Public Structure DEVICE_INFO
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> _
        Dim FirmwareName As Byte()
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=32)> _
        Dim BuildDate As Byte()
        Dim HardwareVersion As UInt32
        Dim FirmwareVersion As UInt32
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=3)> _
        Dim SerialNumber As UInt32()
        Dim Functions As UInt32
    End Structure

    Declare Function USB_ScanDevice Lib "USB2XXX.dll" (<Out()> ByVal pDevHandle As UInt32()) As Int32
    Declare Function USB_OpenDevice Lib "USB2XXX.dll" (ByVal DevHandle As UInt32) As Boolean
    Declare Function USB_CloseDevice Lib "USB2XXX.dll" (ByVal DevHandle As UInt32) As Boolean
    Declare Function DEV_GetDeviceInfo Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByRef pDevInfo As DEVICE_INFO, ByRef pFunctionStr As Byte) As Boolean

    Declare Function DEV_EraseUserData Lib "USB2XXX.dll" (ByVal DevHandle As UInt32) As Boolean
    Declare Function DEV_WriteUserData Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal OffsetAddr As UInt32, ByVal pWriteData As UInt32, ByVal DataLen As UInt32) As Boolean
    Declare Function DEV_ReadUserData Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal OffsetAddr As UInt32, ByVal pReadData As UInt32, ByVal DataLen As UInt32) As Boolean
    Declare Function DEV_SetPowerLevel Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal PowerLevel As Byte) As Boolean

End Module
