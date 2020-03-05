Attribute VB_Name = "usb2lin_ex"
Public Const LIN_EX_SUCCESS             As Long = (0)
Public Const LIN_EX_ERR_NOT_SUPPORT     As Long = (-1)
Public Const LIN_EX_ERR_USB_WRITE_FAIL  As Long = (-2)
Public Const LIN_EX_ERR_USB_READ_FAIL   As Long = (-3)
Public Const LIN_EX_ERR_CMD_FAIL        As Long = (-4)
Public Const LIN_EX_ERR_CH_NO_INIT      As Long = (-5)
Public Const LIN_EX_ERR_READ_DATA       As Long = (-6)

Public Const LIN_EX_CHECK_STD   As Byte = 0     '标准校验，不含PID
Public Const LIN_EX_CHECK_EXT   As Byte = 1     '增强校验，包含PID
Public Const LIN_EX_CHECK_USER  As Byte = 2     '自定义校验类型，需要用户自己计算并传入Check，不进行自动校验
Public Const LIN_EX_CHECK_NONE  As Byte = 3     '接收数据校验错误
Public Const LIN_EX_CHECK_ERROR As Byte = 4     '接收数据校验错误

Public Const LIN_EX_MASTER  As Byte = 1 '主机
Public Const LIN_EX_SLAVE   As Byte = 0 '从机

Public Const LIN_EX_MSG_TYPE_UN  As Byte = 0    '未知类型
Public Const LIN_EX_MSG_TYPE_MW  As Byte = 1            '主机向从机发送数据
Public Const LIN_EX_MSG_TYPE_MR  As Byte = 2            '主机从从机读取数据
Public Const LIN_EX_MSG_TYPE_SW  As Byte = 3            '从机发送数据
Public Const LIN_EX_MSG_TYPE_SR  As Byte = 4            '从机接收数据
Public Const LIN_EX_MSG_TYPE_BK  As Byte = 5            '只发送BREAK信号，若是反馈回来的数据，表明只检测到BREAK信号
Public Const LIN_EX_MSG_TYPE_SY  As Byte = 6            '表明检测到了BREAK，SYNC信号
Public Const LIN_EX_MSG_TYPE_ID  As Byte = 7            '表明检测到了BREAK，SYNC，PID信号
Public Const LIN_EX_MSG_TYPE_DT  As Byte = 8            '表明检测到了BREAK，SYNC，PID,DATA信号
Public Const LIN_EX_MSG_TYPE_CK  As Byte = 9            '表明检测到了BREAK，SYNC，PID,DATA,CHECK信号

Public Type LIN_EX_MSG
    Timestamp As Long
    MsgType As Byte
    CheckType As Byte
    DataLen As Byte
    Sync As Byte
    PID As Byte
    Data(7) As Byte
    Check  As Byte
    Reserve0 As Byte
    Reserve1 As Byte
End Type

Declare Function LIN_EX_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal BaudRate As Long, ByVal MasterMode As Byte) As Long
Declare Function LIN_EX_MasterSync Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal pInMsg As Long, ByVal pOutMsg As Long, ByVal MsgLen As Long) As Long
Declare Function LIN_EX_SlaveSetIDMode Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal pLINMsg As Long, ByVal MsgLen As Long) As Long
Declare Function LIN_EX_SlaveGetIDMode Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal pLINMsg As Long) As Long
Declare Function LIN_EX_SlaveGetData Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal pLINMsg As Long) As Long
Declare Function LIN_EX_CtrlPowerOut Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal State As Byte) As Long
Declare Function LIN_EX_GetVbatValue Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal pBatValue As Long) As Long

Declare Function LIN_EX_MasterStartSch Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal pLINMsg As Long, ByVal MsgLen As Long) As Long
Declare Function LIN_EX_MasterStopSch Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte) As Long
Declare Function LIN_EX_MasterGetSch Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal pLINMsg As Long) As Long

Declare Function LIN_EX_MasterOfflineSch Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal LINIndex As Byte, ByVal BaudRate As Long, ByVal pLINMsg As Long, ByVal MsgLen As Long) As Long
