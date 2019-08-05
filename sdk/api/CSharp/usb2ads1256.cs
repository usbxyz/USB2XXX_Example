using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2ADS1256
    {
        //定义函数返回错误代码
        public const Int32 ADS1256_SUCCESS = (0);              //函数执行成功
        public const Int32 ADS1256_ERR_NOT_SUPPORT = (-1);     //适配器不支持该函数
        public const Int32 ADS1256_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 ADS1256_ERR_USB_READ_FAIL = (-3);   //USB读数据失败
        public const Int32 ADS1256_ERR_CMD_FAIL = (-4);        //命令执行失败
        public const Int32 ADS1256_ERR_CH_NO_INIT    = (-5);   //该通道未初始化
        //定义ADS采样率
        public const Byte ADS1256_DRATE_30000SPS = 0xF0;
        public const Byte ADS1256_DRATE_15000SPS = 0xE0;
        public const Byte ADS1256_DRATE_7500SPS = 0xD0;
        public const Byte ADS1256_DRATE_3750SPS = 0xC0;
        public const Byte ADS1256_DRATE_2000SPS = 0xB0;
        public const Byte ADS1256_DRATE_1000SPS = 0xA1;
        public const Byte ADS1256_DRATE_500SPS  = 0x92;
        public const Byte ADS1256_DRATE_100SPS  = 0x82;
        public const Byte ADS1256_DRATE_60SPS   = 0x72;
        public const Byte ADS1256_DRATE_50SPS   = 0x63;
        public const Byte ADS1256_DRATE_30SPS   = 0x53;
        public const Byte ADS1256_DRATE_25SPS   = 0x43;
        public const Byte ADS1256_DRATE_15SPS   = 0x33;
        public const Byte ADS1256_DRATE_10SPS   = 0x23;
        public const Byte ADS1256_DRATE_5SPS    = 0x13;
        public const Byte ADS1256_DRATE_2_5SPS  = 0x03;
        //定义初始化ADS1256的数据类型
        public struct ADS1256_CONFIG
        {
            UInt16  SampleRate;     //采样率
            Byte   PGA;             //内部增益，2^n
            Byte   BufferEn;        //1：使能Buffer,0：禁止Buffer
            UInt32    SampleMode;   //两个bit控制一个通道，00：不使用该通道，01：该通道为单端模式，10：该通道为差分负极，11：该通道为差分正极
        }
        /**
          * @brief  初始化配置ADS1256
          * @param  DevHandle 设备句柄
          * @param  Channel 需要配置的ADS通道号，取值0或者1
          * @param  pConfig ADS1256配置参数指针
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 ADS1256_Init(Int32 DevHandle,Byte Channel,ref ADS1256_CONFIG pConfig);
        /**
          * @brief  启动ADS转换，调用该函数后，适配器会自动读取ADS转换数据
          * @param  DevHandle 设备句柄
          * @param  Channel 需要启动的ADS通道号，取值0或者1
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 ADS1256_Start(Int32 DevHandle,Byte Channel);
        /**
          * @brief  启动ADC连续转换模式
          * @param  DevHandle 设备句柄
          * @param  Channel 需要读取数据的ADS通道号，取值0或者1
          * @param  pData 数据存储缓冲区首地址
          * @retval 函数执行状态，小于0函数执行出错，大于0为实际采集到的ADC数据
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 ADS1256_Read(Int32 DevHandle,Byte Channel,Int32[] pData);
        /**
          * @brief  停止ADS转换，调用该函数后，适配器会自动停止ADS转换数据
          * @param  DevHandle 设备句柄
          * @param  Channel 需要启动的ADS通道号，取值0或者1
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 ADS1256_Stop(Int32 DevHandle,Byte Channel);
    }
}
