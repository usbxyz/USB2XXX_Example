using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class I2C_SNIFFER
    {
        //定义函数返回错误代码
        public const Int32 SUCCESS = (0);              //函数执行成功
        public const Int32 ERR_NOT_SUPPORT = (-1);     //适配器不支持该函数
        public const Int32 ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 ERR_USB_READ_FAIL = (-3);   //USB读数据失败
        public const Int32 ERR_CMD_FAIL = (-4);        //命令执行失败
        //定义I2C数据类型
        public const Byte IIC_DATA_TYPE_UNKNOWN   = 0;
        public const Byte IIC_DATA_TYPE_START   = 1;
        public const Byte IIC_DATA_TYPE_ADDR   = 2;
        public const Byte IIC_DATA_TYPE_DATA   = 3;
        public const Byte IIC_DATA_TYPE_STOP   = 4;
        //定义I2C数据
        public struct I2C_DATA
        {
            public Byte DataType;  
            public Byte ACK;    
            public Byte WriteRead;     
            public Byte Data; 
            public UInt16 Addr;
            public UInt64 Timestamp;
        }
        //抓取到数据之后的回调函数
        public delegate Int32 I2C_GET_DATA_HANDLE(Int32 DevHandle, Int32 Channel, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)]I2C_DATA[] pI2CData, Int32 I2CDataNum);//接收数据回掉函数

        [DllImport("USB2XXX.dll")]
        public static extern Int32 I2C_SnifferStart(Int32 DevHandle, Byte Channel, int SampleRateHz,I2C_GET_DATA_HANDLE pGetI2CDataHandle);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 I2C_SnifferStop(Int32 DevHandle);
  
    }
}
