using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2GPIO
    {
        //定义函数返回错误代码
        public const Int32 GPIO_SUCCESS = (0);   //函数执行成功
        public const Int32 GPIO_ERR_NOT_SUPPORT = (-1);  //适配器不支持该函数
        public const Int32 GPIO_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 GPIO_ERR_USB_READ_FAIL = (-3);  //USB读数据失败
        public const Int32 GPIO_ERR_CMD_FAIL = (-4);  //命令执行失败
        /**
          * @brief  将GPIO引脚设置为输入模式
          * @param  DevHandle 设备句柄
          * @param  PinMask 需要设置为输入模式的引脚，每个bit位代表一个引脚，对应bit位为1时改引脚对设置有效，最低位为P0
          * @param  PuPd 0-浮空输入，无上拉或者下拉，1-上拉输入，2-下拉输入
          * @retval 函数执行状态，小于0函数执行出错
          */
          [DllImport("USB2XXX.dll")]
          public static extern Int32 GPIO_SetInput(Int32 DevHandle,UInt32 PinMask, Byte PuPd);
        /**
          * @brief  将GPIO引脚设置为输出模式
          * @param  DevHandle 设备句柄
          * @param  PinMask 需要设置为输出模式的引脚，每个bit位代表一个引脚，对应bit位为1时改引脚对设置有效，最低位对应P0
          * @param  PuPd 0-推挽输出，无上拉或者下拉，1-上拉输出，2-下拉输出
          * @retval 函数执行状态，小于0函数执行出错
          */
          [DllImport("USB2XXX.dll")]
          public static extern Int32 GPIO_SetOutput(Int32 DevHandle,UInt32 PinMask, Byte PuPd);
        /**
          * @brief  将GPIO引脚设置为开漏模式，该模式下可作为双向引脚
          * @param  DevHandle 设备句柄
          * @param  PinMask 需要设置为开漏模式的引脚，每个bit位代表一个引脚，对应bit位为1时改引脚对设置有效，最低位对应P0
          * @param  PuPd 0-内部无上拉或者下拉，1-使能上拉，2-使能下拉
          * @retval 函数执行状态，小于0函数执行出错
          */
          [DllImport("USB2XXX.dll")]
          public static extern Int32 GPIO_SetOpenDrain(Int32 DevHandle,UInt32 PinMask, Byte PuPd);
        /**
          * @brief  设置GPIO引脚的输出状态
          * @param  DevHandle 设备句柄
          * @param  PinMask 需要输出状态的引脚，每个bit位代表一个引脚，对应bit位为1时改引脚对设置有效，最低位对应P0
          * @param  PinValue 对应引脚的状态，每个bit位代表一个引脚，对应bit位为1输出高电平，为0输出低电平，最低位对应P0
          * @retval 函数执行状态，小于0函数执行出错
          */
          [DllImport("USB2XXX.dll")]
          public static extern Int32 GPIO_Write(Int32 DevHandle,UInt32 PinMask, UInt32 PinValue);
        /**
          * @brief  获取GPIO引脚的输入状态
          * @param  DevHandle 设备句柄
          * @param  PinMask 需要获取状态的引脚，每个bit位代表一个引脚，对应bit位为1时改引脚对设置有效，最低位对应P0
          * @param  pPinValue 对应引脚的状态，每个bit位代表一个引脚，对应bit位为1引脚为高电平，为0引脚为低电平，最低位对应P0
          * @retval 函数执行状态，小于0函数执行出错
          */
          [DllImport("USB2XXX.dll")]
          public static extern Int32 GPIO_Read(Int32 DevHandle,UInt32 PinMask, ref UInt32 pPinValue);
    }
}
