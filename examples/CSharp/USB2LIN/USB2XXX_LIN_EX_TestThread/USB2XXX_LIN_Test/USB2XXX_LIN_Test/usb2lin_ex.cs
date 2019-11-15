using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
//注意：使用这些函数需要1.5.30及以上的固件才支持
namespace USB2XXX
{
    class USB2LIN_EX
    {
        //定义函数返回错误代码
        public const Int32 LIN_EX_SUCCESS            = (0);   //函数执行成功
        public const Int32 LIN_EX_ERR_NOT_SUPPORT    = (-1);  //适配器不支持该函数
        public const Int32 LIN_EX_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 LIN_EX_ERR_USB_READ_FAIL  = (-3);  //USB读数据失败
        public const Int32 LIN_EX_ERR_CMD_FAIL       = (-4);  //命令执行失败
        public const Int32 LIN_EX_ERR_CH_NO_INIT     = (-5);  //该通道未初始化
        public const Int32 LIN_EX_ERR_READ_DATA      = (-6);  //LIN读数据失败
        //LIN和校验模式
        public const Byte LIN_EX_CHECK_STD    = 0;	//标准校验，不含PID
        public const Byte LIN_EX_CHECK_EXT    = 1;	//增强校验，包含PID
        public const Byte LIN_EX_CHECK_USER   = 2;  //自定义校验类型，需要用户自己计算并传入Check，不进行自动校验
        public const Byte LIN_EX_CHECK_NONE   = 3;  //接收数据校验错误
        public const Byte LIN_EX_CHECK_ERROR  = 4;  //接收数据校验错误
        //定义主从模式
        public const Byte LIN_EX_MASTER       = 1;//主机
        public const Byte LIN_EX_SLAVE        = 0;//从机

        public const Byte LIN_EX_MSG_TYPE_UN    =   0;  //未知类型
        public const Byte LIN_EX_MSG_TYPE_MW    =	1;	//主机向从机发送数据
        public const Byte LIN_EX_MSG_TYPE_MR	=	2;	//主机从从机读取数据
        public const Byte LIN_EX_MSG_TYPE_SW	=	3;	//从机发送数据
        public const Byte LIN_EX_MSG_TYPE_SR	=	4;	//从机接收数据
        public const Byte LIN_EX_MSG_TYPE_BK	=	5;	//只发送BREAK信号，若是反馈回来的数据，表明只检测到BREAK信号
        public const Byte LIN_EX_MSG_TYPE_SY	=	6;	//表明检测到了BREAK，SYNC信号
        public const Byte LIN_EX_MSG_TYPE_ID	=	7;	//表明检测到了BREAK，SYNC，PID信号
        public const Byte LIN_EX_MSG_TYPE_DT	=	8;	//表明检测到了BREAK，SYNC，PID,DATA信号
        public const Byte LIN_EX_MSG_TYPE_CK	=	9;	//表明检测到了BREAK，SYNC，PID,DATA,CHECK信号

        //LIN数据帧格式定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]  
        public struct LIN_EX_MSG
        {
            [MarshalAs(UnmanagedType.U4)]
            public UInt32  Timestamp;    //时间戳
            [MarshalAs(UnmanagedType.U1)]
            public Byte MsgType;	    //帧类型
            [MarshalAs(UnmanagedType.U1)]
            public Byte CheckType;    //校验类型
            [MarshalAs(UnmanagedType.U1)]
            public Byte DataLen;	    //LIN数据段有效数据字节数
            [MarshalAs(UnmanagedType.U1)]
            public Byte Sync;			//固定值，0x55
            [MarshalAs(UnmanagedType.U1)]
            public Byte PID;			//帧ID		
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8, ArraySubType = UnmanagedType.U1)]
            public Byte[] Data;	    //数据
            [MarshalAs(UnmanagedType.U1)]
            public Byte Check;		//校验,只有校验数据类型为LIN_EX_CHECK_USER的时候才需要用户传入数据
            [MarshalAs(UnmanagedType.U1)]
            public Byte BreakBits;  //该帧的BRAK信号位数，有效值为10到26，若设置为其他值则默认为13位
            [MarshalAs(UnmanagedType.U1)]
            public Byte Reserve1;
        }
        //USB2LIN相关函数定义
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_Init(Int32 DevHandle,Byte LINIndex,Int32 BaudRate,Byte MasterMode);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_EX_MasterSync(Int32 DevHandle, Byte LINIndex, LIN_EX_MSG[] pInMsg, IntPtr pOutMsg, Int32 MsgLen);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_SlaveSetIDMode(Int32 DevHandle,Byte LINIndex,LIN_EX_MSG[] pLINMsg,Int32 MsgLen);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_SlaveGetIDMode(Int32 DevHandle,Byte LINIndex,IntPtr pLINMsg);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_EX_SlaveGetData(Int32 DevHandle, Byte LINIndex, IntPtr pLINMsg);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_CtrlPowerOut(Int32 DevHandle,Byte State);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_GetVbatValue(Int32 DevHandle,Int16[] pBatValue);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_MasterStartSch(Int32 DevHandle,Byte LINIndex,LIN_EX_MSG[] pLINMsg,Int32 MsgLen);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_MasterStopSch(Int32 DevHandle,Byte LINIndex);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_MasterGetSch(Int32 DevHandle,Byte LINIndex,IntPtr pLINMsg);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_MasterOfflineSch(Int32 DevHandle,Byte LINIndex,Int32 BaudRate,LIN_EX_MSG[] pLINMsg,Int32 MsgLen);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_DecodeListFile(string pFileName,Byte CheckType,Int32 BaudRate,Byte[] pReadDataList,Byte ReadDataListLen,Byte[] pCheckTypeList,Byte CheckTypeListLen);
        [DllImport("USB2XXX.dll")]
        public static extern Int32  LIN_EX_GetListFileMsg(Int32 MsgIndex,Int32 MsgLen,IntPtr pLINMsg);
    }
}
