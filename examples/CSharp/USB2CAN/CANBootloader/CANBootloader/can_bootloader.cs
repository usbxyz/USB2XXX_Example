using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class CAN_Bootloader
    {
        //函数返回错误代码定义
        public const Int32 CAN_SUCCESS            = (0);   //函数执行成功
        public const Int32 CAN_ERR_NOT_SUPPORT    = (-1);  //适配器不支持该函数
        public const Int32 CAN_ERR_USB_WRITE_FAIL = (-2);  //USB写数据失败
        public const Int32 CAN_ERR_USB_READ_FAIL  = (-3);  //USB读数据失败
        public const Int32 CAN_ERR_CMD_FAIL       = (-4);  //命令执行失败

        public const Int32 CAN_BOOT_ERR_CONFIG      = (-30); //配置设备错误
        public const Int32 CAN_BOOT_ERR_SEND        = (-31); //发送数据出错
        public const Int32 CAN_BOOT_ERR_TIME_OUT    = (-32); //超时错误
        public const Int32 CAN_BOOT_ERR_CMD         = (-33); //执行命令失败
        public const Int32 CAN_BOOT_ERR_BAUD        = (-34); //波特率参数自动获取失败
        public const Int32 CAN_BOOT_ERR_BUFFER      = (-35); //从设备返回接收数据缓冲区大小为0
        public const Int32 CAN_BOOT_ERR_DSIZE       = (-36); //单次传输数据量大于了4093字节

        public const Byte CAN_BOOT_ERR_ERASE         = 1;   //固件擦除出错
        public const Byte CAN_BOOT_ERR_ERASE_IN_APP  = 2;   //当前模式为APP，不能擦除固件
        public const Byte CAN_BOOT_ERR_WRITE_OUTRANGE= 3;   //当前地址超出了正常的地址范围
        public const Byte CAN_BOOT_ERR_WRITE_IN_APP  = 4;   //当前模式不能写入固件数据
        public const Byte CAN_BOOT_ERR_WRITE         = 5;   //数据写入程序存储器出错
        public const Byte CAN_BOOT_ERR_WRITE_OUT_ADDR= 6;   //数据长度超出了程序存储器范围 
        public const Byte CAN_BOOT_ERR_WRITE_CRC     = 7;   //数据CRC校验出错

        //CAN Bootloader固件类型
        public const Byte FW_TYPE_BOOT    = 0x55;
        public const Byte FW_TYPE_APP     = 0xAA;

        //CAN Bootloader数据收发ID类型
        public const Byte ID_TYPE_EXT    = 0x01;
        public const Byte ID_TYPE_STD    = 0x00;
        //CAN Bootloader函数原型
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_Init(Int32 DevHandle,Int32 CANIndex,UInt32 SendDataID,UInt32 ReceiveDataID,Byte IDType,Int32 BaudRate);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_GetFWInfo(Int32 DevHandle,Int32 CANIndex,Byte NAD,Byte[] pFWType,UInt32[] pFWVersion,UInt32 TimeOutOfMs);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_EnterBootMode(Int32 DevHandle,Int32 CANIndex,Byte NAD);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_EraseApp(Int32 DevHandle,Int32 CANIndex,Byte NAD,UInt32 AppSizeOfByte,UInt32 TimeOutOfMs);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_SetAddrOffset(Int32 DevHandle,Int32 CANIndex,Byte NAD,UInt32 AddrOffset,UInt16[] pBufferSize,UInt32 TimeOutOfMs);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_SendAppData(Int32 DevHandle,Int32 CANIndex,Byte NAD,Byte[] pData,UInt16 DataLen,UInt16[] pCRC16);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_WriteAppData(Int32 DevHandle, Int32 CANIndex, Byte NAD, UInt16 CRC16, UInt32 TimeOutOfMs);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 CAN_BOOT_ExecuteApp(Int32 DevHandle,Int32 CANIndex,Byte NAD);
    }
}
