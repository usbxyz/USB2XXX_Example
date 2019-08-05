using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class USB2SPI
    {
        //定义SPI通道
        public const Int32 SPI1        = (0x00);
        public const Int32 SPI1_CS0    = (0x00);
        public const Int32 SPI1_CS1    = (0x10);
        public const Int32 SPI1_CS2    = (0x20);
        public const Int32 SPI1_CS3    = (0x30);
        public const Int32 SPI1_CS4    = (0x40);

        public const Int32 SPI2        = (0x01);
        public const Int32 SPI2_CS0    = (0x01);
        public const Int32 SPI2_CS1    = (0x11);
        public const Int32 SPI2_CS2    = (0x21);
        public const Int32 SPI2_CS3    = (0x31);
        public const Int32 SPI2_CS4    = (0x41);
        //定义工作模式
        public const Byte SPI_MODE_HARD_FDX = 0; //硬件控制（全双工模式）
        public const Byte SPI_MODE_HARD_HDX = 1; //硬件控制（半双工模式）
        public const Byte SPI_MODE_SOFT_HDX = 2; //软件控制（半双工模式）
        public const Byte SPI_MODE_SOFT_ONE_WIRE = 3; //单总线模式，数据线输入输出都为MOSI
        //定义主从机模式
        public const Byte SPI_MASTER = 1; //主机
        public const Byte SPI_SLAVE = 0; //从机
        //定义数据移位方式
        public const Byte SPI_MSB = 0; //高位在前
        public const Byte SPI_LSB = 1; //低位在前
        //定义片选输出极性
        public const Byte SPI_SEL_LOW = 0; //片选输出低电平
        public const Byte SPI_SEL_HIGH = 1; //片选输出高电平
        //定义函数返回错误代码
        public const Int32 SPI_SUCCESS             = (0);   //函数执行成功
        public const Int32 SPI_ERR_NOT_SUPPORT     = (-1);  //适配器不支持该函数
        public const Int32 SPI_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
        public const Int32 SPI_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
        public const Int32 SPI_ERR_CMD_FAIL        = (-4);  //命令执行失败
        public const Int32 SPI_ERR_PARAMETER       = (-5);  //参数错误
        //定义从机模式下连续读取数据的回调函数
        public delegate Int32 SPI_GET_DATA_HANDLE(Int32 DevIndex, Int32 SPIIndex, Byte[] pData, Int32 DataNum);//接收数据回掉函数
        //定义初始化SPI的数据类型
        public struct SPI_CONFIG
        {
            public Byte   Mode;            //SPI控制方式:0-硬件控制（全双工模式）,1-硬件控制（半双工模式），2-软件控制（半双工模式）,3-单总线模式，数据线输入输出都为MOSI
            public Byte   Master;          //主从选择控制:0-从机，1-主机
            public Byte   CPOL;            //时钟极性控制:0-SCK空闲时为低电平，1-SCK空闲时为高电平
            public Byte   CPHA;            //时钟相位控制:0-第一个SCK时钟采样，1-第二个SCK时钟采样
            public Byte   LSBFirst;        //数据移位方式:0-MSB在前，1-LSB在前
            public Byte   SelPolarity;     //片选信号极性:0-低电平选中，1-高电平选中
            public UInt32 ClockSpeedHz;    //SPI时钟频率:单位为HZ，硬件模式下最大50000000，最小390625，频率按2的倍数改变
        }
        //定义SPI Flash器件配置参数数据类型
        public struct SPI_FLASH_CONFIG
        {
            public Byte CMD_WriteEnable;          //使能写命令
            public Byte CMD_WriteDisable;         //禁止写命令
            public Byte CMD_WritePage;            //写数据命令
            public Byte WritePageAddressBytes;    //写数据时的地址宽度，单位为字节
            public Byte CMD_EraseSector;          //扇区擦出命令
            public Byte EraseSectorAddressBytes;  //扇区擦出的地址宽度，单位为字节
            public Byte CMD_EraseBulk;            //块擦出命令
            public Byte CMD_EraseChip;            //整片擦出命令
            public Byte CMD_ReadID;               //读芯片ID命令
            public Byte CMD_ReadData;             //读数据命令
            public Byte ReadDataAddressBytes;     //读数据时的地址宽度，单位为字节
            public Byte CMD_ReadFast;             //快速模式读数据命令
            public Byte ReadFastAddressBytes;     //快速读数据时的地址宽度，单位为字节
            public Byte CMD_ReadStatus;           //读取状态寄存器命令
            public Byte CMD_WriteStatus;          //写状态寄存器命令
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
            public Byte[] ID;                     //芯片ID存储数组
            public Byte ID_Length;                //ID长度，单位为字节
            public Int32 PageSize;                //页大小，单位为字节
            public Int32 NumPages;                //芯片总的页数
            public Int32 SectorSize;              //扇区大小，单位为字节
        }

        /**
          * @brief  SPI初始化配置函数
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pConfig SPI初始化配置结构体指针
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_Init(Int32 DevIndex,Int32 SPIIndex, ref SPI_CONFIG pConfig);

        /**
          * @brief  SPI发送数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 数据发送缓冲区首地址
          * @param  WriteLen 数据发送字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_WriteBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteLen);

        /**
          * @brief  SPI异步方式发送数据，函数调用之后不会等待数据发送完毕，该方式可以加大数据发送速率
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 数据发送缓冲区首地址
          * @param  WriteLen 数据发送字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_AsyncWriteBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteLen);

        /**
          * @brief  SPI接收数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadData 数据接收缓冲区首地址
          * @param  ReadLen 接收数据字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_ReadBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pReadData,Int32 ReadLen);

        /**
          * @brief  SPI发送接收数据，全双工模式下发送和接收同时进行，半双工模式下发送和接收分开进行，整个操作过程片选一直有效
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 发送数据缓冲区首地址
          * @param  WriteLen 发送数据字节数
          * @param  pReadData 数据接收缓冲区首地址
          * @param  ReadLen 接收数据字节数
          * @param  IntervalTime 半双工模式下，发送数据和接收数据之间的时间间隔，单位为微妙
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_WriteReadBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteLen,Byte[] pReadData,Int32 ReadLen,Int32 IntervalTimeUs);
        /**
            * @brief  等到有效事件后SPI发送数据
            * @param  DevIndex 设备索引号
            * @param  SPIIndex SPI通道号，取值0或者1
            * @param  pWriteData 数据发送缓冲区首地址
            * @param  WriteLen 数据发送字节数
            * @param  EventPin 等待事件的GPIO引脚
            * @param  EventType 事件类型，0x00-低电平，0x11-高电平，0x10-下降沿，0x01上升沿
            * @param  TimeOutOfMs 等待事件超时时间，单位为毫秒
            * @retval 函数执行状态，小于0函数执行出错
            */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_WriteBytesOfEvent(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteLen,Int32 EventPin,Byte EventType,Int32 TimeOutOfMs);


        /**
          * @brief  等到有效事件后SPI接收数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadData 数据接收缓冲区首地址
          * @param  ReadLen 接收数据字节数
          * @param  EventPin 等待事件的GPIO引脚
          * @param  EventType 事件类型，0x00-低电平，0x11-高电平，0x10-下降沿，0x01上升沿
          * @param  TimeOutOfMs 等待事件超时时间，单位为毫秒
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_ReadBytesOfEvent(Int32 DevIndex,Int32 SPIIndex,Byte[] pReadData,Int32 ReadLen,Int32 EventPin,Byte EventType,Int32 TimeOutOfMs);

        /**
          * @brief  等到有效事件后SPI发送接收数据，全双工模式下发送和接收同时进行，半双工模式下发送和接收分开进行，整个操作过程片选一直有效
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 发送数据缓冲区首地址
          * @param  WriteLen 发送数据字节数
          * @param  pReadData 数据接收缓冲区首地址
          * @param  ReadLen 接收数据字节数
          * @param  IntervalTime 半双工模式下，发送数据和接收数据之间的时间间隔，单位为微妙
          * @param  EventPin 等待事件的GPIO引脚
          * @param  EventType 事件类型，0x00-低电平，0x11-高电平，0x10-下降沿，0x01上升沿
          * @param  TimeOutOfMs 等待事件超时时间，单位为毫秒
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_WriteReadBytesOfEvent(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteLen,Byte[] pReadData,Int32 ReadLen,Int32 IntervalTimeUs,Int32 EventPin,Byte EventType,Int32 TimeOutOfMs);

        /**
          * @brief  二进制模式写数据，该函数可以发送任意bit数数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteBitStr 二进制数据字符串，如："100110011"
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_WriteBits(Int32 DevIndex,Int32 SPIIndex,StringBuilder pWriteBitStr);

        /**
          * @brief  二进制模式读数据，该函数可以读取任意bit数数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadBitStr 读取到的二进制数据字符串，如："100110011"
          * @param  ReadBitsNum 读取数据的二进制bit数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_ReadBits(Int32 DevIndex,Int32 SPIIndex,StringBuilder pReadBitStr,Int32 ReadBitsNum);

        /**
          * @brief  二进制模式写读数据，该函数可以写读取任意bit数数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteBitStr 写二进制数据字符串，如："100110011"
          * @param  pReadBitStr 读取到的二进制数据字符串，如："100110011"
          * @param  ReadBitsNum 读取数据的二进制bit数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_WriteReadBits(Int32 DevIndex,Int32 SPIIndex,StringBuilder pWriteBitStr,StringBuilder pReadBitStr,Int32 ReadBitsNum);

        /**
          * @brief  SPI从机模式下将数据写入数据发送缓冲区，等待主机的时钟信号再将数据发送出去
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 发送数据缓冲区首地址
          * @param  WriteLen 发送数据字节数
          * @param  TimeOutMs 等待数据发送完毕超时时间，若小于或者等于0则不会等待数据发送完毕，函数立即返回
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_SlaveWriteBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteLen,Int32 TimeOutMs);

        /**
          * @brief  SPI从机模式下获取接收数据缓冲区的数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadData 数据接收缓冲区首地址
          * @param  pReadLen 接收到的数据字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_SlaveReadBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pReadData,Int32 TimeOutMs);

        /**
          * @brief  SPI从机模式下连续读取数据,SPI在从机模式下接收到数据之后，通过回调函数传出数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pSlaveReadDataHandle 从机模式下接收到数据后的回调函数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_SlaveContinueRead(Int32 DevIndex,Int32 SPIIndex,SPI_GET_DATA_HANDLE pSlaveReadDataHandle);

        /**
          * @brief  SPI从机模式下获取接收数据缓冲区的数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadData 数据接收缓冲区首地址
          * @param  BufferSize 数据缓冲区大小
          * @retval 获取到的数据字节数
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_SlaveGetBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pReadData,Int32 BufferSize);
        /**
          * @brief  停止SPI从机模式下连续读取数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_SlaveContinueReadStop(Int32 DevIndex,Int32 SPIIndex);
        /**
          * @brief  读写SPI-Flash初始化配置
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  ClockSpeed SPI时钟信号频率，单位为Hz,最大为50MHz，依次成倍递减
          * @param  pConfig SPI-Flash相关参数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashInit(Int32 DevIndex,Int32 SPIIndex,Int32 ClockSpeed, ref SPI_FLASH_CONFIG pConfig);

        /**
          * @brief  读取SPI-Flash芯片的ID号
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pID 芯片ID号存储首地址，其长度由SPI_FLASH_CONFIG中的ID_Length决定
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashReadID(Int32 DevIndex,Int32 SPIIndex,Byte[] pID);

        /**
          * @brief  擦除SPI-Flash的扇区数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  StartSector 起始扇区号，注意是扇区号，不是具体的扇区地址
          * @param  NumSector 需要擦除的扇区数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashEraseSector(Int32 DevIndex,Int32 SPIIndex,Int32 StartSector,Int32 NumSector);

        /**
          * @brief  擦除SPI-Flash整个芯片的数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashEraseChip(Int32 DevIndex,Int32 SPIIndex);


        /**
          * @brief  向SPI-Flash写数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  StartAddr 写数据起始地址
          * @param  pWriteData 写数据缓冲区首地址
          * @param  WriteLen 写数据字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashWrite(Int32 DevIndex,Int32 SPIIndex,Int32 StartAddr,Byte[] pWriteData,Int32 WriteLen);

        /**
          * @brief  从SPI-Flash读数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  StartAddr 写数据起始地址
          * @param  pReadData 读数据缓冲区首地址
          * @param  ReadLen 读数据字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashRead(Int32 DevIndex,Int32 SPIIndex,Int32 StartAddr,Byte[] pReadData,Int32 ReadLen);

        /**
          * @brief  从SPI-Flash读数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  StartAddr 写数据起始地址
          * @param  pReadData 读数据缓冲区首地址
          * @param  ReadLen 读数据字节数
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_FlashReadFast(Int32 DevIndex,Int32 SPIIndex,Int32 StartAddr,Byte[] pReadData,Int32 ReadLen);

        /**
          * @brief  Block模式写数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 写数据起始地址
          * @param  BlockSize 每次写数据字节数
          * @param  BlockNum 写数据总次数
          * @param  IntervalTime 每次写数据之间的时间间隔，单位为微妙
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_BlockWriteBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 BlockSize,Int32 BlockNum,Int32 IntervalTimeUs);

        /**
          * @brief  Block模式读数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadData 读数据缓冲区首地址
          * @param  BlockSize 每次读数据字节数
          * @param  BlockNum 读数据总次数
          * @param  IntervalTime 每次写数据之间的时间间隔，单位为微妙
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_BlockReadBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pReadData,Int32 BlockSize,Int32 BlockNum,Int32 IntervalTimeUs);

        /**
          * @brief  Block模式写读数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 写数据起始地址
          * @param  WriteBlockSize 每次读数据字节数
          * @param  pReadData 读数据缓冲区首地址
          * @param  ReadBlockSize 每次读数据字节数
          * @param  BlockNum 读数据总次数
          * @param  IntervalTime 每次写数据之间的时间间隔，单位为微妙
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_BlockWriteReadBytes(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteBlockSize,Byte[] pReadData,Int32 ReadBlockSize,Int32 BlockNum,Int32 IntervalTimeUs);

        /**
          * @brief  Block模式写数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 写数据起始地址
          * @param  BlockSize 每次写数据字节数
          * @param  BlockNum 写数据总次数
          * @param  EventPin 等待事件的GPIO引脚
          * @param  EventType 事件类型，0x00-低电平，0x11-高电平，0x10-下降沿，0x01上升沿
          * @param  TimeOutOfMs 等待事件超时时间，单位为毫秒
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_BlockWriteBytesOfEvent(Int32 DevIndex, Int32 SPIIndex, Byte[] pWriteData, Int32 BlockSize, Int32 BlockNum, Int32 EventPin, Byte EventType, Int32 TimeOutOfMs);

        /**
          * @brief  Block模式读数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pReadData 读数据缓冲区首地址
          * @param  BlockSize 每次读数据字节数
          * @param  BlockNum 读数据总次数
          * @param  EventPin 等待事件的GPIO引脚
          * @param  EventType 事件类型，0x00-低电平，0x11-高电平，0x10-下降沿，0x01上升沿
          * @param  TimeOutOfMs 等待事件超时时间，单位为毫秒
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_BlockReadBytesOfEvent(Int32 DevIndex,Int32 SPIIndex,Byte[] pReadData,Int32 BlockSize,Int32 BlockNum,Int32 EventPin,Byte EventType,Int32 TimeOutOfMs);

        /**
          * @brief  Block模式写读数据
          * @param  DevIndex 设备索引号
          * @param  SPIIndex SPI通道号，取值0或者1
          * @param  pWriteData 写数据起始地址
          * @param  WriteBlockSize 每次读数据字节数
          * @param  pReadData 读数据缓冲区首地址
          * @param  ReadBlockSize 每次读数据字节数
          * @param  BlockNum 读数据总次数
          * @param  EventPin 等待事件的GPIO引脚
          * @param  EventType 事件类型，0x00-低电平，0x11-高电平，0x10-下降沿，0x01上升沿
          * @param  TimeOutOfMs 等待事件超时时间，单位为毫秒
          * @retval 函数执行状态，小于0函数执行出错
          */
        [DllImport("USB2XXX.dll")]
        public static extern Int32 SPI_BlockWriteReadBytesOfEvent(Int32 DevIndex,Int32 SPIIndex,Byte[] pWriteData,Int32 WriteBlockSize,Byte[] pReadData,Int32 ReadBlockSize,Int32 BlockNum,Int32 EventPin,Byte EventType,Int32 TimeOutOfMs);

    }
}
