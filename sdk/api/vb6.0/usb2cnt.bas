Attribute VB_Name = "usb2cnt"

Public Const CNT_SUCCESS             As Long = (0)
Public Const CNT_ERR_NOT_SUPPORT     As Long = (-1)
Public Const CNT_ERR_USB_WRITE_FAIL  As Long = (-2)
Public Const CNT_ERR_USB_READ_FAIL   As Long = (-3)
Public Const CNT_ERR_CMD_FAIL        As Long = (-4)
'计数器模式
Public Const COUNTER_MODE_UP     As Byte = 0
Public Const COUNTER_MODE_DOWN        As Byte = 1
'计数器触发条件
Public Const COUNTER_POL_RISING     As Byte = 0
Public Const COUNTER_POL_FALLING    As Byte = 1
Public Const COUNTER_POL_BOTHEDGE   As Byte = 2
'计数器宽度
Public Const COUNTER_BITS16        As Byte = 16
Public Const COUNTER_BITS32        As Byte = 32
'计数器输入引脚模式
Public Const  COUNTER_PIN_NOPULL     As Byte = 0
Public Const  COUNTER_PIN_UP         As Byte = 1
Public Const  COUNTER_PIN_DOWN       As Byte = 2
'计数器通道定义
Public Const COUNTER_CH0             As Byte = 1
Public Const COUNTER_CH1             As Byte = 2
Public Const COUNTER_CH2             As Byte = 4
Public Const COUNTER_CH3             As Byte = 8

Public Type CNT_CONFIG
    CounterMode As Byte     '计数模式，0-Up,1-Down
    CounterPolarity As Byte '计数极性，0-Rising，1-Falling，2-BothEdge
    CounterBitWide As Byte  '计数位宽，16-16bit位宽，32-32bit位宽
    CounterPinMode As Byte  '计数器输入引脚模式，0-浮空输入，1-上拉输入，2-下拉输入
End Type

Declare Function CNT_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal ChannelMask As Byte, ByRef pConfig As CNT_CONFIG) As Long
Declare Function CNT_Start Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal ChannelMask As Byte) As Long
Declare Function CNT_Stop Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal ChannelMask As Byte) As Long
Declare Function CNT_SetValue Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal ChannelMask As Byte,ByVal pCntValues As Any) As Long
Declare Function CNT_GetValue Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal ChannelMask As Byte,ByVal pCntValues As Any) As Long
