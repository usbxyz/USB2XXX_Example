Imports System.Runtime.InteropServices
'注意：使用这些函数需要1.5.30及以上的固件才支持
Module lin_uds
    'LIN数据收发帧格式定义
    Public Structure LIN_UDS_ADDR
        Dim ReqID As Byte '请求报文ID。
        Dim ResID As Byte '应答报文ID。
        Dim NAD As Byte   '节点地址，0x7F为广播地址
        Dim CheckType As Byte '0-标准，1-增强
        Dim STmin As Byte '连续帧时间间隔，单位为毫秒
    End Structure
    '定义函数返回错误代码
    Public Const LIN_UDS_OK As Int32 = (0)
    Public Const LIN_UDS_TRAN_USB As Int32 = (-98)
    Public Const LIN_UDS_TRAN_LIN As Int32 = (-99)
    Public Const LIN_UDS_TIMEOUT_A As Int32 = (-100)
    Public Const LIN_UDS_TIMEOUT_Bs As Int32 = (-101)
    Public Const LIN_UDS_TIMEOUT_Cr As Int32 = (-102)
    Public Const LIN_UDS_WRONG_SN As Int32 = (-103)
    Public Const LIN_UDS_INVALID_FS As Int32 = (-104)
    Public Const LIN_UDS_UNEXP_PDU As Int32 = (-105)
    Public Const LIN_UDS_WFT_OVRN As Int32 = (-106)
    Public Const LIN_UDS_BUFFER_OVFLW As Int32 = (-107)
    Public Const LIN_UDS_ERROR As Int32 = (-108)

    Declare Function LIN_UDS_Request Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, ByVal UDSAddr As LIN_UDS_ADDR, <[In]()> ByVal pReqData As Byte(), ByVal DataLen As UInt32) As Int32
    Declare Function LIN_UDS_Response Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, ByVal UDSAddr As LIN_UDS_ADDR, <[Out]()> ByVal pResData As Byte(), ByVal TimeOutMs As UInt32) As Int32
    Declare Function LIN_UDS_GetMsgFromUDSBuffer Lib "USB2XXX.dll" (ByVal DevHandle As UInt32, ByVal LINIndex As Byte, <Out()> ByVal pCanGetMsg As LIN_EX_MSG(), ByVal BufferSize As UInt32) As Int32

End Module
