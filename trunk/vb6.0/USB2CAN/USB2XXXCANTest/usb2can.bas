Attribute VB_Name = "u2b2can"

Public Type CAN_MSG
    ID As Long
    TimeStamp As Long
    RemoteFlag As Byte
    ExternFlag As Byte
    DataLen As Byte
    Data(7) As Byte
End Type

Public Type CAN_INIT_CONFIG
    CAN_BRP As Long
    CAN_SJW As Byte
    CAN_BS1 As Byte
    CAN_BS2 As Byte
    CAN_Mode As Byte
    CAN_ABOM As Byte
    CAN_NART As Byte
    CAN_RFLM As Byte
    CAN_TXFP As Byte
End Type

Public Type CAN_FILTER_CONFIG
    Enable As Byte
    FilterIndex As Byte
    FilterMode As Byte
    ExtFrame As Byte
    ID_Std_Ext As Long
    ID_IDE As Long
    ID_RTR As Long
    MASK_Std_Ext As Long
    MASK_IDE As Long
    MASK_RTR As Long
End Type

Public Type CAN_STATUS
    TSR As Long
    ESR As Long
    RECounter As Byte
    TECounter As Byte
    LECode As Byte
End Type

Public Type CBL_CMD_LIST
    Erase As Byte
    WriteInfo As Byte
    Write As Byte
    Check As Byte
    SetBaudRate As Byte
    Excute As Byte
    CmdSuccess As Byte
    CmdFaild As Byte
End Type

Public Const CAN_SUCCESS As Long = 0
Public Const CAN_ERR_NOT_SUPPORT As Long = -1
Public Const CAN_ERR_USB_WRITE_FAIL As Long = -2
Public Const CAN_ERR_USB_READ_FAIL As Long = -3
Public Const CAN_ERR_CMD_FAIL As Long = -4
Public Const CAN_BL_ERR_CONFIG As Long = -20
Public Const CAN_BL_ERR_SEND As Long = -21
Public Const CAN_BL_ERR_TIME_OUT As Long = -22
Public Const CAN_BL_ERR_CMD As Long = -23

Public Const CAN_BL_BOOT As Long = &H55555555
Public Const CAN_BL_APP As Long = &HAAAAAAAA


Declare Function CAN_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByRef pCanConfig As CAN_INIT_CONFIG) As Long
Declare Function CAN_Filter_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByRef pFilterConfig As CAN_FILTER_CONFIG) As Long
Declare Function CAN_SendMsg Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal pCanSendMsg As Long, ByVal SendMsgNum As Long) As Long
Declare Function CAN_GetMsg Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal pCanGetMsg As Long) As Long
Declare Function CAN_GetStatus Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByRef pCANStatus As CAN_STATUS) As Long

Declare Function CAN_BL_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByRef pCanConfig As CAN_INIT_CONFIG, ByRef pCmdList As CBL_CMD_LIST) As Long
Declare Function CAN_BL_NodeCheck Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal NodeAddr As Integer, ByRef pVersion As Long, ByRef pType As Long, ByVal TimeOut As Long) As Long
Declare Function CAN_BL_Erase Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal NodeAddr As Integer, ByVal FlashSize As Long, ByVal TimeOut As Long) As Long
Declare Function CAN_BL_Write Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal NodeAddr As Integer, ByVal AddrOffset As Long, ByVal pData As Long, ByVal DataNum As Long, ByVal TimeOut As Long) As Long
Declare Function CAN_BL_Excute Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal NodeAddr As Integer, ByVal FirmwareType As Long) As Long
Declare Function CAN_BL_SetNewBaudRate Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal CANIndex As Byte, ByVal NodeAddr As Integer, ByRef pInitConfig As CAN_INIT_CONFIG, ByVal NewBaudRate As Long, ByVal TimeOut As Long) As Long


