Imports System.Runtime.InteropServices
Module usb2lin
    '定义初始化LIN初始化数据类型
    Public Structure LIN_CONFIG
        Dim BaudRate As UInt32  '波特率,最大20K
        Dim CheckMode As Byte   '校验模式，0-标准校验模式，1-增强校验模式（包含PID）
        Dim MasterMode As Byte  '主从模式，0-从模式，1-主模式
        Dim BreakBits As Byte   'Break长度，0x00-10bit,0x20-11bit,主机模式下无效，从机模式下可以任意设置
    End Structure
    'LIN数据收发帧格式定义
    Public Structure LIN_MSG
        Dim ID As Byte          'ID，取值范围0~0x3F
        Dim DataLen As Byte     '发送数据时，代表发送数据的长度，不含校验数据，接收数据时，数据的长度，包含校验数据
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=9)> _
        Dim Data As Byte()      '数据存储区
    End Structure
    '定义函数返回错误代码
    Public Const LIN_SUCCESS As Int32 = (0)             '函数执行成功
    Public Const LIN_ERR_NOT_SUPPORT As Int32 = (-1)    '适配器不支持该函数
    Public Const LIN_ERR_USB_WRITE_FAIL As Int32 = (-2) 'USB写数据失败
    Public Const LIN_ERR_USB_READ_FAIL As Int32 = (-3)  'USB读数据失败
    Public Const LIN_ERR_CMD_FAIL As Int32 = (-4)       '命令执行失败
    Public Const LIN_ERR_CH_NO_INIT As Int32 = (-5)     '该通道未初始化
    Public Const LIN_ERR_READ_DATA As Int32 = (-6)      'LIN读数据失败
    'LIN和校验模式
    Public Const LIN_CHECK_MODE_STD As Byte = 0
    Public Const LIN_CHECK_MODE_EXT As Byte = 1
    Public Const LIN_CHECK_MODE_NONE As Byte = 2
    'BREAK位数定义
    Public Const LIN_BREAK_BITS_10 As Byte = &H0
    Public Const LIN_BREAK_BITS_11 As Byte = &H20
    '定义主从模式
    Public Const LIN_MASTER As Byte = 1
    Public Const LIN_SLAVE As Byte = 0
    '定义从机操作模式
    Public Const LIN_SLAVE_WRITE As Byte = 0
    Public Const LIN_SLAVE_READ As Byte = 1

    Declare Function LIN_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, ByRef pConfig As LIN_CONFIG) As Int32
    Declare Function LIN_Write Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, ByVal pLINMsg As LIN_MSG(), ByVal Len As Int32) As Int32
    Declare Function LIN_Read Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <[In](), Out()> ByVal pLINMsg As LIN_MSG(), ByVal Len As Int32) As Int32
    Declare Function LIN_SlaveSetIDMode Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, ByVal IDMode As Byte, ByVal pLINMsg As LIN_MSG(), ByVal Len As Int32) As Int32
    Declare Function LIN_SlaveGetData Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <Out()> ByVal pLINMsg As LIN_MSG()) As Int32

End Module
