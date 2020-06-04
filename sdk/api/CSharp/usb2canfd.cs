using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2CANFD
    {
        //1.CANFD信息帧的数据类型定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]  
        public struct CANFD_MSG
        {
            public UInt32  ID;       //报文ID,bit[30]-RTR,bit[31]-IDE,bit[28..0]-ID
            public Byte    DLC;      //数据字节长度，可设置为-0,1,2,3,4,5,6,7,8,12,16,20,24,32,48,64
            public Byte    Flags;    //bit[0]-BRS,bit[1]-ESI,bit[2]-FDF,bit[6..5]-Channel,bit[7]-RXD
            public Byte    __Res0;   //保留
            public Byte    __Res1;   //保留
            public UInt32  TimeStamp;//帧接收或者发送时的时间戳，单位为10us
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64, ArraySubType = UnmanagedType.I1)]
            public Byte[]  Data;        //报文的数据。
        };

        //2.CANFD初始化配置数据类型定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]  
        public struct CANFD_INIT_CONFIG
        {
            public Byte Mode; //0-正常模式，1-自发自收模式
            public Byte ISOCRCEnable;//0-禁止ISO CRC,1-使能ISO CRC
            public Byte RetrySend;//0-禁止重发，1-无限制重发
            public Byte ResEnable;//0-不接入内部120欧终端电阻，1-接入内部120欧终端电阻
            //波特率参数可以用TCANLINPro软件里面的波特率计算工具计算
            //仲裁段波特率参数,波特率=40M/NBT_BRP*(1+NBT_SEG1+NBT_SEG2)
            public Byte NBT_BRP;
            public Byte NBT_SEG1;
            public Byte NBT_SEG2;
            public Byte NBT_SJW;
            //数据段波特率参数,波特率=40M/DBT_BRP*(1+DBT_SEG1+DBT_SEG2)
            public Byte DBT_BRP;
            public Byte DBT_SEG1;
            public Byte DBT_SEG2;
            public Byte DBT_SJW;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8, ArraySubType = UnmanagedType.I1)]
            public Byte[] __Res0;//保留
        };
        //3.CANFD诊断帧信息结构体定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]  
        public struct CANFD_DIAGNOSTIC {
            public Byte NREC;//标称比特率接收错误计数
            public Byte NTEC;//标称比特率发送错误计数
            public Byte DREC;//数据比特率接收错误计数
            public Byte DTEC;//数据比特率发送错误计数
            public UInt16 ErrorFreeMsgCount;//无错误帧计数
            public UInt16 Flags;//参考诊断标志定义
        };
        //4.CANFD总线错误信息结构体定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]  
        public struct CANFD_BUS_ERROR {
            public Byte TEC;//发送错误计数
            public Byte REC;//接收错误计数
            public Byte Flags;//参考总线错误标志定义
        };
        //5.CAN 滤波器设置数据类型定义
        public struct CANFD_FILTER_CONFIG{
            public Byte   Enable; //使能该过滤器，1-使能，0-禁止
            public Byte   Index;  //过滤器索引号，取值范围为0到31
            public UInt32 ID_Accept;  //验收码ID,bit[28..0]为有效ID位，bit[31]为IDE
            public UInt32 ID_Mask;    //屏蔽码，对应bit位若为1，则需要对比对应验收码bit位，相同才接收
        };

        //函数返回错误代码定义
        public const Int32 CANFD_SUCCESS             = (0);   //函数执行成功
        public const Int32 CANFD_ERR_NOT_SUPPORT     = (-1);  //适配器不支持该函数
        public const Int32 CANFD_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
        public const Int32 CANFD_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
        public const Int32 CANFD_ERR_CMD_FAIL        = (-4);  //命令执行失败
        //CANFD_MSG.ID定义
        public const Int32 CANFD_MSG_FLAG_RTR      = (1<<30);
        public const Int32 CANFD_MSG_FLAG_IDE      = (1<<31);
        public const Int32 CANFD_MSG_FLAG_ID_MASK  = (0x1FFFFFFF);
        //CANFD_MSG.Flags定义
        public const Int32 CANFD_MSG_FLAG_BRS      = (1<<0);
        public const Int32 CANFD_MSG_FLAG_ESI      = (1<<1);
        public const Int32 CANFD_MSG_FLAG_FDF      = (1<<2);
        public const Int32 CANFD_MSG_FLAG_RXD      = (1<<7);
        //CANFD_DIAGNOSTIC.Flags定义
        public const Int32 CANFD_DIAGNOSTIC_FLAG_NBIT0_ERR     = (0x0001);//在发送报文（或应答位、主动错误标志或过载标志）期间，器件要发送显性电平（逻辑值为0的数据或标识符位），但监视的总线值为隐性。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_NBIT1_ERR     = (0x0002);//在发送报文（仲裁字段除外）期间，器件要发送隐性电平（逻辑值为1的位），但监视到的总线值为显性。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_NACK_ERR      = (0x0004);//发送报文未应答。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_NFORM_ERR     = (0x0008);//接收报文的固定格式部分格式错误。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_NSTUFF_ERR    = (0x0010);//在接收报文的一部分中，序列中包含了5个以上相等位，而报文中不允许出现这种序列。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_NCRC_ERR      = (0x0020);//接收的报文的CRC校验和不正确。输入报文的CRC与通过接收到的数据计算得到的CRC不匹配。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_TXBO_ERR      = (0x0080);//器件进入离线状态（且自动恢复）。
        public const Int32 CANFD_DIAGNOSTIC_FLAG_DBIT0_ERR     = (0x0100);//见NBIT0_ERR
        public const Int32 CANFD_DIAGNOSTIC_FLAG_DBIT1_ERR     = (0x0200);//见NBIT1_ERR
        public const Int32 CANFD_DIAGNOSTIC_FLAG_DFORM_ERR     = (0x0800);//见NFORM_ERR
        public const Int32 CANFD_DIAGNOSTIC_FLAG_DSTUFF_ERR    = (0x1000);//见NSTUFF_ERR
        public const Int32 CANFD_DIAGNOSTIC_FLAG_DCRC_ERR      = (0x2000);//见NCRC_ERR
        public const Int32 CANFD_DIAGNOSTIC_FLAG_ESI_ERR       = (0x4000);//接收的CAN FD报文的ESI标志置1
        public const Int32 CANFD_DIAGNOSTIC_FLAG_DLC_MISMATCH  = (0x8000);//DLC不匹配,在发送或接收期间，指定的DLC大于FIFO元素的PLSIZE
        //CANFD_BUS_ERROR.Flags定义
        public const Int32 CANFD_BUS_ERROR_FLAG_TX_RX_WARNING    = (0x01);
        public const Int32 CANFD_BUS_ERROR_FLAG_RX_WARNING       = (0x02);
        public const Int32 CANFD_BUS_ERROR_FLAG_TX_WARNING       = (0x04);
        public const Int32 CANFD_BUS_ERROR_FLAG_RX_BUS_PASSIVE   = (0x08);
        public const Int32 CANFD_BUS_ERROR_FLAG_TX_BUS_PASSIVE   = (0x10);
        public const Int32 CANFD_BUS_ERROR_FLAG_TX_BUS_OFF       = (0x20);
        //USB2CANFD相关函数定义
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_Init(Int32 DevHandle, Byte CANIndex, ref CANFD_INIT_CONFIG pCanConfig);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_StartGetMsg(Int32 DevHandle, Byte CANIndex);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_StopGetMsg(Int32 DevHandle, Byte CANIndex);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_SendMsg(Int32 DevHandle, Byte CANIndex, IntPtr pCanSendMsg, Int32 SendMsgNum);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_GetMsg(Int32 DevHandle, Byte CANIndex, IntPtr pCanGetMsg, Int32 BufferSize);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_SetFilter(Int32 DevHandle, Byte CANIndex,ref CANFD_FILTER_CONFIG pCanFilter,Byte Len);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_GetDiagnostic(Int32 DevHandle, Byte CANIndex,ref CANFD_DIAGNOSTIC pCanDiagnostic);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_GetBusError(Int32 DevHandle, Byte CANIndex,ref CANFD_BUS_ERROR pCanBusError);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CANFD_SetRelay(Int32 DevHandle, Byte RelayState);
        //[DllImport("USB2XXX.dll")]
        //public static extern Int32 CANFD_SetRelayData(Int32 DevHandle,ref CAN_RELAY_HEAD pCANRelayHead,ref CAN_RELAY_DATA pCANRelayData);
        //[DllImport("USB2XXX.dll")]
        //public static extern Int32 CANFD_GetRelayData(Int32 DevHandle,ref CAN_RELAY_HEAD pCANRelayHead,ref CAN_RELAY_DATA pCANRelayData);

    }
}
