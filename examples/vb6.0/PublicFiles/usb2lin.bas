Attribute VB_Name = "u2b2lin"

Public Type LIN_CONFIG
    BaudRate As Long
    CheckMode As Byte
    MasterMode As Byte
    BreakBits As Byte
End Type

Public Type LIN_MSG
    ID As Byte
    DataLen As Byte
    Data(8) As Byte
End Type


Public Const LIN_SUCCESS             As Long = (0)  
Public Const LIN_ERR_NOT_SUPPORT     As Long = (-1)  
Public Const LIN_ERR_USB_WRITE_FAIL  As Long = (-2)  
Public Const LIN_ERR_USB_READ_FAIL   As Long = (-3)  
Public Const LIN_ERR_CMD_FAIL        As Long = (-4)  
Public Const LIN_ERR_CH_NO_INIT      As Long = (-5)  
Public Const LIN_ERR_READ_DATA       As Long = (-6)  

Public Const LIN_CHECK_MODE_STD  As Byte =  0
Public Const LIN_CHECK_MODE_EXT  As Byte =  1
Public Const LIN_CHECK_MODE_NONE As Byte =  2

Public Const LIN_BREAK_BITS_10  As Byte =  0x00
Public Const LIN_BREAK_BITS_11  As Byte =  0x20

Public Const LIN_MASTER  As Byte = 1
Public Const LIN_SLAVE   As Byte = 0

Public Const LIN_SLAVE_WRITE As Byte =  0
Public Const LIN_SLAVE_READ  As Byte =  1

Declare Function LIN_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal LINIndex As Byte,ByRef pConfig As LIN_CONFIG) As Long
Declare Function LIN_SendBreak Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal LINIndex As Byte) As Long
Declare Function LIN_Write Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal LINIndex As Byte,ByVal pLINMsg As Long,ByVal Len As Long) As Long
Declare Function LIN_Read Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal LINIndex As Byte,ByVal pLINMsg As Long,ByVal Len As Long) As Long
Declare Function LIN_SlaveSetIDMode Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal LINIndex As Byte,ByVal IDMode As Byte,ByVal pLINMsg As Long,ByVal Len As Long) As Long
Declare Function LIN_SlaveGetData Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal LINIndex As Byte,ByVal pLINMsg As Long) As Long
