using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2LIN
    {
        //定义函数返回错误代码
        public const Int32 LIN_SUCCESS            = (0);   //函数执行成功
        public const Int32 LIN_ERR_NOT_SUPPORT    = (-1);  //适配器不支持该函数
        public const Int32 LIN_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 LIN_ERR_USB_READ_FAIL  = (-3);  //USB读数据失败
        public const Int32 LIN_ERR_CMD_FAIL       = (-4);  //命令执行失败
        public const Int32 LIN_ERR_CH_NO_INIT     = (-5);  //该通道未初始化
        public const Int32 LIN_ERR_READ_DATA      = (-6);  //LIN读数据失败
        //LIN和校验模式
        public const Byte LIN_CHECK_MODE_STD    = 0;
        public const Byte LIN_CHECK_MODE_EXT    = 1;
        public const Byte LIN_CHECK_MODE_NONE   = 2;
        //BREAK位数定义
        public const Byte LIN_BREAK_BITS_10   = 0x00;
        public const Byte LIN_BREAK_BITS_11   = 0x20;
        //定义主从模式
        public const Byte LIN_MASTER        =  1;
        public const Byte LIN_SLAVE         =  0;
        //定义从机操作模式
        public const Byte LIN_SLAVE_WRITE   =  0;
        public const Byte LIN_SLAVE_READ    =  1;

        //定义初始化LIN的数据类型
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]  
        public struct LIN_CONFIG
        {
            public UInt32  BaudRate;     //波特率,最大20K
            public Byte CheckMode;    //校验模式，0-标准校验模式，1-增强校验模式（包含PID）
            public Byte MasterMode;   //主从模式，0-从模式，1-主模式
            public Byte BreakBits;    //Break长度，0x00-10bit,0x20-11bit
        }

        //LIN数据收发帧格式定义
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]  
        public struct LIN_MSG
        {
            [MarshalAs(UnmanagedType.U1)]
            public Byte ID;		//ID，取值范围0~0x3F
            [MarshalAs(UnmanagedType.U1)]
            public Byte DataLen;	//发送数据时，代表发送数据的长度，不含校验数据，接收数据时，数据的长度，包含校验数据
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 9, ArraySubType = UnmanagedType.I1)]
            public Byte[] Data;	//数据存储区
        }
        //USB2LIN相关函数定义
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_Init(Int32 DevHandle,Byte LINIndex,ref LIN_CONFIG pConfig);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_Write(Int32 DevHandle,Byte LINIndex,LIN_MSG[] pLINMsg,Int32 Len);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_Read(Int32 DevHandle, Byte LINIndex, IntPtr pLINMsg, Int32 Len);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_SlaveSetIDMode(Int32 DevHandle,Byte LINIndex,Byte IDMode,LIN_MSG[] pLINMsg,Int32 Len);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LIN_SlaveGetData(Int32 DevHandle,Byte LINIndex,IntPtr pLINMsg);

    }
}
