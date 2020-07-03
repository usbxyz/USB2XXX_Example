using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2DAC
    {
        //定义函数返回错误代码
        public const Int32 DAC_SUCCESS = (0);   //函数执行成功
        public const Int32 DAC_ERR_NOT_SUPPORT = (-1);  //适配器不支持该函数
        public const Int32 DAC_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 DAC_ERR_USB_READ_FAIL = (-3);  //USB读数据失败
        public const Int32 DAC_ERR_CMD_FAIL = (-4);  //命令执行失败
        public const Int32 DAC_ERR_CH_NO_INIT = (-5);  //参数传入错误

        /**
          * @brief  初始化配置DAC并输出信号
          * @param  DevHandle 设备索引号
          * @param  CycleUs 数据输出时间间隔，单位为0.25us，数据输出总时间=0.25*CycleUs*WaveDataNum us
          * @param  pWaveData 待输出的数据缓冲区指针，数据最大值为0xFFF,对应电压为3.3V
          * @param  WaveDataNum 输出的数据点数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 DAC_Init(Int32 DevHandle,UInt16 CycleUs,UInt16[] pWaveData,UInt16 WaveDataNum);
        /**
          * @brief  启动DAC并输出信号
          * @param  DevHandle 设备索引号
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 DAC_Start(Int32 DevHandle);
        /**
          * @brief  停止DAC输出信号
          * @param  DevHandle 设备索引号
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 DAC_Stop(Int32 DevHandle);
    }
}
