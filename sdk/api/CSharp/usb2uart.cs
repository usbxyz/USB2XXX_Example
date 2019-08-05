using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2UART
    {
        //定义函数返回错误代码
        public const Int32 UART_SUCCESS = (0);              //函数执行成功
        public const Int32 UART_ERR_NOT_SUPPORT = (-1);     //适配器不支持该函数
        public const Int32 UART_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 UART_ERR_USB_READ_FAIL = (-3);   //USB读数据失败
        public const Int32 UART_ERR_CMD_FAIL = (-4);        //命令执行失败
        //数据位宽
        public const Byte UART_WORD_LENGTH_8BIT   = 0;
        public const Byte UART_WORD_LENGTH_9BIT   = 1;
        //停止位
        public const Byte UART_STOP_BITS_1        = 0;
        public const Byte UART_STOP_BITS_05       = 1;
        public const Byte UART_STOP_BITS_2        = 2;
        public const Byte UART_STOP_BITS_15       = 3;
        //奇偶校验位
        public const Byte UART_PARITY_NO          = 0;
        public const Byte UART_PARITY_EVEN        = 4;
        public const Byte UART_PARITY_ODD         = 6;
        //TE控制信号输出
        public const Byte UART_TE_DISEN           = 0x00;
        public const Byte UART_TE_EN_LOW          = 0x80;
        public const Byte UART_TE_EN_HIGH         = 0x81;
        //定义初始化UART初始化数据类型
        public struct UART_CONFIG
        {
            public UInt32 BaudRate;  //波特率
            public Byte WordLength;   //数据位宽，0-8bit,1-9bit
            public Byte StopBits;     //停止位宽，0-1bit,1-0.5bit,2-2bit,3-1.5bit
            public Byte Parity;       //奇偶校验，0-No,4-Even,6-Odd
            public Byte TEPolarity;   //TE输出控制，0x80-输出TE信号，且低电平有效，0x81-输出TE信号，且高电平有效，0x00不输出TE信号
        }
        [DllImport("USB2XXX.dll")]
        public static extern Int32 UART_Init(Int32 DevHandle, Byte Channel, ref UART_CONFIG pConfig);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 UART_WriteBytes(Int32 DevHandle,Byte Channel,Byte[] pWriteData,Int32 DataSize);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 UART_WriteBytesAsync(Int32 DevHandle,Byte Channel,Byte[] pWriteData,Int32 DataSize);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 UART_ReadBytes(Int32 DevHandle, Byte Channel, Byte[] pReadData, Int32 TimeOutMs);
    }
}
