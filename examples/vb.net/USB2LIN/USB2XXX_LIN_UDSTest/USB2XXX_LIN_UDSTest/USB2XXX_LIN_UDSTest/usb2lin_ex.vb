Imports System.Runtime.InteropServices
'注意：使用这些函数需要1.5.30及以上的固件才支持
Module usb2lin_ex
    'LIN数据收发帧格式定义
    Public Structure LIN_EX_MSG
        Dim Timestamp As UInt32 '接收数据时为时间戳，发送数据时为延时的时间，单位为毫秒
        Dim MsgType As Byte     '帧类型，参考帧类型常数定义
        Dim CheckType As Byte   '帧数据校验类型
        Dim DataLen As Byte    'LIN数据段有效数据字节数
        Dim Sync As Byte       '固定值0x55,可以不用赋值，底层会自动处理
        Dim PID As Byte        '带校验位的帧ID
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Dim Data As Byte()      '数据存储区
        Dim Check As Byte      '数据和校验，若校验模式是增强型，则计算的数据包含PID，若校验模式为标准型，则计算校验和的数据不包含PID
        Dim BreakBits As Byte   '该帧的BRAK信号位数，有效值为10到26，若设置为其他值则默认为13位
        Dim Reserve1 As Byte   '保留数据
    End Structure
    '定义函数返回错误代码
    Public Const LIN_EX_SUCCESS As Int32 = (0)             '函数执行成功
    Public Const LIN_EX_ERR_NOT_SUPPORT As Int32 = (-1)    '适配器不支持该函数
    Public Const LIN_EX_ERR_USB_WRITE_FAIL As Int32 = (-2) 'USB写数据失败
    Public Const LIN_EX_ERR_USB_READ_FAIL As Int32 = (-3)  'USB读数据失败
    Public Const LIN_EX_ERR_CMD_FAIL As Int32 = (-4)       '命令执行失败
    Public Const LIN_EX_ERR_CH_NO_INIT As Int32 = (-5)     '该通道未初始化
    Public Const LIN_EX_ERR_READ_DATA As Int32 = (-6)      'LIN读数据失败
	Public Const LIN_EX_ERR_PARAMETER As Int32 = (-7)	   '函数参数传入有误
    'LIN和校验模式
    Public Const LIN_EX_CHECK_STD As Byte = 0	'标准校验，不含PID
    Public Const LIN_EX_CHECK_EXT As Byte = 1	'增强校验，包含PID
    Public Const LIN_EX_CHECK_USER As Byte = 2	'自定义校验类型，需要用户自己计算并传入Check，不进行自动校验
    Public Const LIN_EX_CHECK_NONE As Byte = 3	'不计算校验数据
    Public Const LIN_EX_CHECK_ERROR As Byte = 4	'接收数据校验错误
    '定义主从模式
    Public Const LIN_EX_MASTER As Byte = 1
    Public Const LIN_EX_SLAVE As Byte = 0
    '帧类型定义
	Public Const LIN_EX_MSG_TYPE_UN		=0  '未知类型
	Public Const LIN_EX_MSG_TYPE_MW		=1	'主机向从机发送数据
	Public Const LIN_EX_MSG_TYPE_MR		=2	'主机从从机读取数据
	Public Const LIN_EX_MSG_TYPE_SW		=3	'从机发送数据
	Public Const LIN_EX_MSG_TYPE_SR		=4	'从机接收数据
	Public Const LIN_EX_MSG_TYPE_BK		=5	'只发送BREAK信号，若是反馈回来的数据，表明只检测到BREAK信号
	Public Const LIN_EX_MSG_TYPE_SY		=6	'表明检测到了BREAK，SYNC信号
	Public Const LIN_EX_MSG_TYPE_ID		=7	'表明检测到了BREAK，SYNC，PID信号
	Public Const LIN_EX_MSG_TYPE_DT		=8	'表明检测到了BREAK，SYNC，PID,DATA信号
	Public Const LIN_EX_MSG_TYPE_CK		=9	'表明检测到了BREAK，SYNC，PID,DATA,CHECK信号

    Declare Function LIN_EX_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, ByVal BaudRate As UInt32, ByVal MasterMode As Byte) As Int32
    Declare Function LIN_EX_MasterSync Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <[In]()> ByVal pInMsg As LIN_EX_MSG(), <Out()> ByVal pOutMsg As LIN_EX_MSG(), ByVal MsgLen As UInt32) As Int32
	Declare Function LIN_EX_MasterWrite Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal LINIndex As Byte,ByVal PID As Byte,<[In]()> ByVal pData As Byte(),ByVal DataLen As Byte,ByVal CheckType As Byte) As Int32
	Declare Function LIN_EX_MasterRead Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal LINIndex As Byte,ByVal PID As Byte,<[Out]()> ByVal pData As Byte()) As Int32
	Declare Function LIN_EX_SlaveSetIDMode Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <[In]()> ByVal pLINMsg As LIN_EX_MSG(), ByVal MsgLen As UInt32) As Int32
    Declare Function LIN_EX_SlaveGetData Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <Out()> ByVal pLINMsg As LIN_EX_MSG()) As Int32
    Declare Function LIN_EX_CtrlPowerOut Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal State As Byte) As Int32
    Declare Function LIN_EX_GetVbatValue Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal pBatValue As Int16) As Int32
    Declare Function LIN_EX_MasterStartSch Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <[In]()> ByVal pLINMsg As LIN_EX_MSG(), ByVal MsgLen As UInt32) As Int32
    Declare Function LIN_EX_MasterStopSch Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte) As Int32
    Declare Function LIN_EX_MasterGetSch Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <Out()> ByVal pLINMsg As LIN_EX_MSG()) As Int32
End Module
