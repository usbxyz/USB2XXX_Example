using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class LIN_UDS
    {
        //LIN UDS地址定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]  
        public struct CAN_MSG
        {
            public Byte  ReqID;     //请求报文ID。
            public Byte  ResID;     //应答报文ID。
            public Byte  NAD;       //节点地址，0x7F为广播地址
            public Byte  CheckType; //0-标准，1-增强
            public Byte  STmin;     //连续帧时间间隔，单位为毫秒
        };

        //函数返回错误代码定义
        public const Int32 LIN_UDS_OK            = 0
        public const Int32 LIN_UDS_TRAN_USB      = -98
        public const Int32 LIN_UDS_TRAN_CAN      = -99
        public const Int32 LIN_UDS_TIMEOUT_A     = -100
        public const Int32 LIN_UDS_TIMEOUT_Bs    = -101
        public const Int32 LIN_UDS_TIMEOUT_Cr    = -102
        public const Int32 LIN_UDS_WRONG_SN      = -103
        public const Int32 LIN_UDS_INVALID_FS    = -104
        public const Int32 LIN_UDS_UNEXP_PDU     = -105
        public const Int32 LIN_UDS_WFT_OVRN      = -106
        public const Int32 LIN_UDS_BUFFER_OVFLW  = -107
        public const Int32 LIN_UDS_ERROR         = -108

        //LIN UDS相关函数定义
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_UDS_Request(Int32 DevHandle,Byte LINIndex,LIN_UDS_ADDR UDSAddr,Byte[] pReqData,Int32 DataLen);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_UDS_Response(Int32 DevHandle,Byte LINIndex,LIN_UDS_ADDR UDSAddr,Byte[] pResData,Int32 TimeOutMs);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_UDS_GetMsgFromUDSBuffer(Int32 DevHandle,Byte LINIndex,IntPtr pCanGetMsg,Int32 BufferSize);
    }
}
