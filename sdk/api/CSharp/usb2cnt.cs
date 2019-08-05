using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2CNT
    {
        //定义函数返回错误代码
        public const Int32 CNT_SUCCESS = (0);              //函数执行成功
        public const Int32 CNT_ERR_NOT_SUPPORT = (-1);     //适配器不支持该函数
        public const Int32 CNT_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 CNT_ERR_USB_READ_FAIL = (-3);   //USB读数据失败
        public const Int32 CNT_ERR_CMD_FAIL = (-4);        //命令执行失败
        //计数器模式
        public const Byte COUNTER_MODE_UP      = 0;
        public const Byte COUNTER_MODE_DOWN    = 1;
        //计数器触发条件
        public const Byte COUNTER_POL_RISING   = 0;
        public const Byte COUNTER_POL_FALLING  = 1;
        public const Byte COUNTER_POL_BOTHEDGE = 2;
        //计数器宽度
        public const Byte COUNTER_BITS16      = 16;
        public const Byte COUNTER_BITS32      = 32;
        //计数器输入引脚模式
        public const Byte  COUNTER_PIN_NOPULL   = 0x00;
        public const Byte  COUNTER_PIN_UP       = 0x01;
        public const Byte  COUNTER_PIN_DOWN     = 0x02;
        //计数器通道定义
        public const Byte COUNTER_CH0           = 0x01;
        public const Byte COUNTER_CH1           = 0x02;
        public const Byte COUNTER_CH2           = 0x04;
        public const Byte COUNTER_CH3           = 0x08;
        //定义初始化UART初始化数据类型
        public struct CNT_CONFIG
        {
            Byte   CounterMode;      //计数模式，0-Up,1-Down
            Byte   CounterPolarity;  //计数极性，0-Rising，1-Falling，2-BothEdge
            Byte   CounterBitWide;   //计数位宽，16-16bit位宽，32-32bit位宽
            Byte   CounterPinMode;   //计数器输入引脚模式，0-浮空输入，1-上拉输入，2-下拉输入
        }
        [DllImport("USB2XXX.dll")]
        public const Int32 CNT_Init(Int32 DevHandle, Byte ChannelMask, ref CNT_CONFIG pConfig);
        [DllImport("USB2XXX.dll")]
        public const Int32 CNT_Start(Int32 DevHandle, Byte ChannelMask);
        [DllImport("USB2XXX.dll")]
        public const Int32 CNT_Stop(Int32 DevHandle, Byte ChannelMask);
        [DllImport("USB2XXX.dll")]
        public const Int32 CNT_SetValue(Int32 DevHandle, Byte ChannelMask,ref pCntValues);
        [DllImport("USB2XXX.dll")]
        public const Int32 CNT_GetValue(Int32 DevHandle, Byte ChannelMask,ref pCntValues);
    }
}
