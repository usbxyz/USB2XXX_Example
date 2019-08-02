Imports System.Runtime.InteropServices
'注意：使用这些函数需要1.5.30及以上的固件才支持
Module usb2gpio
    '定义函数返回错误代码
    Public Const GPIO_SUCCESS As Int32 = (0)             '函数执行成功
    Public Const GPIO_ERR_NOT_SUPPORT As Int32 = (-1)    '适配器不支持该函数
    Public Const GPIO_ERR_USB_WRITE_FAIL As Int32 = (-2) 'USB写数据失败
    Public Const GPIO_ERR_USB_READ_FAIL As Int32 = (-3)  'USB读数据失败
    Public Const GPIO_ERR_CMD_FAIL As Int32 = (-4)       '命令执行失败

    '上下拉电阻配置定义
    Public Const GPIO_PUPD_NOPULL As Byte = 0	'没有上下拉电阻
    Public Const GPIO_PUPD_UP As Byte = 1	'使能上拉电阻
    Public Const GPIO_PUPD_DOWN As Byte = 2	'使能下拉电阻

    Declare Function GPIO_SetInput Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal PinMask As UInt32, ByVal PuPd As Byte) As Int32
	Declare Function GPIO_SetOutput Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal PinMask As UInt32, ByVal PuPd As Byte) As Int32
	Declare Function GPIO_SetOpenDrain Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal PinMask As UInt32, ByVal PuPd As Byte) As Int32
	Declare Function GPIO_Write Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal PinMask As UInt32, ByVal PinValue As UInt32) As Int32
	Declare Function GPIO_Read Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal PinMask As UInt32, <Out()> ByVal pPinValue As UInt32()) As Int32

End Module
