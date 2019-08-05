Imports System.Runtime.InteropServices
Module usb2can
    Public Structure CAN_MSG
        Dim ID As UInt32
        Dim TimeStamp As UInt32
        Dim RemoteFlag As Byte
        Dim ExternFlag As Byte
        Dim DataLen As Byte
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Dim Data As Byte()
    End Structure

    Public Structure CAN_INIT_CONFIG
        Dim CAN_BRP As UInt32
        Dim CAN_SJW As Byte
        Dim CAN_BS1 As Byte
        Dim CAN_BS2 As Byte
        Dim CAN_Mode As Byte
        Dim CAN_ABOM As Byte
        Dim CAN_NART As Byte
        Dim CAN_RFLM As Byte
        Dim CAN_TXFP As Byte
    End Structure

    Public Structure CAN_FILTER_CONFIG
        Dim Enable As Byte
        Dim FilterIndex As Byte
        Dim FilterMode As Byte
        Dim ExtFrame As Byte
        Dim ID_Std_Ext As UInt32
        Dim ID_IDE As UInt32
        Dim ID_RTR As UInt32
        Dim MASK_Std_Ext As UInt32
        Dim MASK_IDE As UInt32
        Dim MASK_RTR As UInt32
    End Structure

    Public Structure CAN_STATUS
        Dim TSR As UInt32
        Dim ESR As UInt32
        Dim RECounter As Byte
        Dim TECounter As Byte
        Dim LECode As Byte
    End Structure

    Public Structure CBL_CMD_LIST
        Dim EraseFlash As Byte
        Dim WriteInfo As Byte
        Dim Write As Byte
        Dim Check As Byte
        Dim SetBaudRate As Byte
        Dim Excute As Byte
        Dim CmdSuccess As Byte
        Dim CmdFaild As Byte
    End Structure

    Public Const CAN_SUCCESS As Int32 = 0
    Public Const CAN_ERR_NOT_SUPPORT As Int32 = -1
    Public Const CAN_ERR_USB_WRITE_FAIL As Int32 = -2
    Public Const CAN_ERR_USB_READ_FAIL As Int32 = -3
    Public Const CAN_ERR_CMD_FAIL As Int32 = -4
    Public Const CAN_BL_ERR_CONFIG As Int32 = -20
    Public Const CAN_BL_ERR_SEND As Int32 = -21
    Public Const CAN_BL_ERR_TIME_OUT As Int32 = -22
    Public Const CAN_BL_ERR_CMD As Int32 = -23

    Public Const CAN_BL_BOOT As Int32 = &H55555555
    Public Const CAN_BL_APP As Int32 = &HAAAAAAAA


    Declare Function CAN_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByRef pCanConfig As CAN_INIT_CONFIG) As Int32
    Declare Function CAN_Filter_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByRef pFilterConfig As CAN_FILTER_CONFIG) As Int32
    Declare Function CAN_SendMsg Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByVal pCanSendMsg As CAN_MSG(), ByVal SendMsgNum As UInt32) As Int32
    Declare Function CAN_GetMsg Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, <Out()> ByVal pCanGetMsg As CAN_MSG()) As Int32
    Declare Function CAN_GetStatus Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByRef pCANStatus As CAN_STATUS) As Int32

    Declare Function CAN_BL_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByRef pCanConfig As CAN_INIT_CONFIG, ByRef pCmdList As CBL_CMD_LIST) As Int32
    Declare Function CAN_BL_NodeCheck Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByVal NodeAddr As UInt16, ByRef pVersion As UInt32, ByRef pType As UInt32, ByVal TimeOut As UInt32) As Int32
    Declare Function CAN_BL_Erase Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByVal NodeAddr As UInt16, ByVal FlashSize As UInt32, ByVal TimeOut As UInt32) As Int32
    Declare Function CAN_BL_Write Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByVal NodeAddr As UInt16, ByVal AddrOffset As UInt32, ByVal pData As Byte(), ByVal DataNum As UInt32, ByVal TimeOut As UInt32) As Int32
    Declare Function CAN_BL_Excute Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByVal NodeAddr As UInt16, ByVal FirmwareType As Int32) As Int32
    Declare Function CAN_BL_SetNewBaudRate Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal CANIndex As Byte, ByVal NodeAddr As UInt16, ByRef pInitConfig As CAN_INIT_CONFIG, ByVal NewBaudRate As UInt32, ByVal TimeOut As UInt32) As Int32


End Module
