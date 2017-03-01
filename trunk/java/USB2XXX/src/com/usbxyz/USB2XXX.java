package com.usbxyz;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;  
import com.sun.jna.Native;
import com.sun.jna.Structure;


public interface USB2XXX  extends Library{
	USB2XXX INSTANCE  = (USB2XXX)Native.loadLibrary("USB2XXX",USB2XXX.class); 
	
	public class DEVICE_INFO  extends Structure{
		
	    public static class ByReference extends DEVICE_INFO implements Structure.ByReference {}  
	    public static class ByValue extends DEVICE_INFO implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"FirmwareName", "BuildDate", "HardwareVersion", "FirmwareVersion", "SerialNumber", "Functions"});
		}
	    
		public byte[]   FirmwareName = new byte[32];   //固件名称字符串
		public byte[]   BuildDate = new byte[32];      //固件编译时间字符串
		public int     	HardwareVersion;    //硬件版本号
		public int     	FirmwareVersion;    //固件版本号
		public int[]    SerialNumber = new int[3];    //适配器序列号
		public int     	Functions;          //适配器当前具备的功能
	}
	/**
	  * @brief  初始化USB设备，并扫描设备连接数，必须调用
	  * @param  pDevNum 每个设备的设备号存储地址，若不需要设备号，可以传入NULL
	  * @retval 扫描到的设备数量
	  */
	int  USB_ScanDevice(int[] pDevNum);
	/**
	  * @brief  打开设备，必须调用
	  * @param  DevIndex 设备索引号
	  * @retval 打开设备的状态
	  */
	boolean USB_OpenDevice(int DevIndex);

	/**
	  * @brief  关闭设备
	  * @param  DevIndex 设备索引号
	  * @retval 关闭设备的状态
	  */
	boolean USB_CloseDevice(int DevIndex);

	/**
	  * @brief  获取设备信息，比如设备名称，固件版本号，设备序号，设备功能说明字符串等
	  * @param  DevIndex 设备索引号
	  * @param  pDevInfo 设备信息存储结构体指针
	  * @param  pFunctionStr 设备功能说明字符串
	  * @retval 获取设备信息的状态
	  */
	boolean USB_GetDeviceInfo(int DevIndex,DEVICE_INFO pDevInfo,byte[] pFunctionStr);

	/**
	  * @brief  擦出用户区数据
	  * @param  DevIndex 设备索引号
	  * @retval 用户区数据擦出状态
	  */
	boolean USB_EraseUserData(int DevIndex);

	/**
	  * @brief  向用户区域写入用户自定义数据，写入数据之前需要调用擦出函数将数据擦出
	  * @param  DevIndex 设备索引号
	  * @param  OffsetAddr 数据写入偏移地址，起始地址为0x00，用户区总容量为0x10000字节，也就是64KBye
	  * @param  pWriteData 用户数据缓冲区首地址
	  * @param  DataLen 待写入的数据字节数
	  * @retval 写入用户自定义数据状态
	  */
	boolean USB_WriteUserData(int DevIndex,int OffsetAddr,byte[] pWriteData,int DataLen);

	/**
	  * @brief  从用户自定义数据区读出数据
	  * @param  DevIndex 设备索引号
	  * @param  OffsetAddr 数据写入偏移地址，起始地址为0x00，用户区总容量为0x10000字节，也就是64KBye
	  * @param  pReadData 用户数据缓冲区首地址
	  * @param  DataLen 待读出的数据字节数
	  * @retval 读出用户自定义数据的状态
	  */
	boolean USB_ReadUserData(int DevIndex,int OffsetAddr,byte[] pReadData,int DataLen);

	
	public class IIC_CONFIG  extends Structure{
		
	    public static class ByReference extends IIC_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends IIC_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"ClockSpeedHz", "OwnAddr", "Master", "AddrBits", "EnablePu"});
		}
	    
		public int    ClockSpeedHz; //IIC时钟频率:单位为Hz
		public short  OwnAddr;      //USB2XXX为从机时自己的地址
		public byte   Master;       //主从选择控制:0-从机，1-主机
		public byte   AddrBits;     //从机地址模式，7-7bit模式，10-10bit模式
		public byte   EnablePu;     //使能引脚芯片内部上拉电阻，若不使能，则I2C总线上必须接上拉电阻
	}
    //定义函数返回错误代码
    public static int IIC_SUCCESS             = (0);   //函数执行成功
    public static int IIC_ERR_NOT_SUPPORT     = (-1) ; //适配器不支持该函数
    public static int IIC_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
    public static int IIC_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
    public static int IIC_ERR_CMD_FAIL        = (-4);  //命令执行失败
    public static int IIC_ERR_PARA_ERROR      = (-5);  //参数传入错误
    //定义IIC函数返回错误代码
    public static int IIC_ERROR_SUCCESS     = 0;  //操作成功
    public static int IIC_ERROR_CHANNEL     = 1;   //该通道不支持该函数
    public static int IIC_ERROR_BUSY        = 2;   //总线忙
    public static int IIC_ERROR_START_FAILD = 3;   //启动总线失败
    public static int IIC_ERROR_TIMEOUT     = 4;   //超时
    public static int IIC_ERROR_NACK        = 5;   //从机无应答
    //USB2IIC相关函数定义
    int IIC_Init(int DevIndex,int IICIndex, IIC_CONFIG pConfig);
    int IIC_GetSlaveAddr(int DevIndex,int IICIndex,short[] pSlaveAddr);
    int IIC_WriteBytes(int DevIndex,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,int TimeOutMs);
    int IIC_ReadBytes(int DevIndex,int IICIndex,short SlaveAddr,byte[] pReadData,int ReadLen,int TimeOutMs);
    int IIC_WriteReadBytes(int DevIndex,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,byte[] pReadData,int ReadLen,int TimeOutMs);
    int IIC_SlaveWriteBytes(int DevIndex,int IICIndex,byte[] pWriteData,int WriteLen,int TimeOutMs);
    int IIC_SlaveReadBytes(int DevIndex,int IICIndex,byte[] pReadData,int TimeOutMs);
    int IIC_SlaveWriteRemain(int DevIndex,int IICIndex);

    int IIC_WriteBytesOfEvent(int DevIndex,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,int EventPin,byte EventType,int TimeOutOfEventMs);
    int IIC_ReadBytesOfEvent(int DevIndex,int IICIndex,short SlaveAddr,byte[] pReadData,int ReadLen,int EventPin,byte EventType,int TimeOutOfEventMs);
    int IIC_WriteReadBytesOfEvent(int DevIndex,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,byte[] pReadData,int ReadLen,int EventPin,byte EventType,int TimeOutOfEventMs);

    int SMBUS_Init(int DevIndex,int IICIndex,int ClockSpeedHz,byte EnablePu);
    int SMBUS_QuickCommand(int DevIndex,int IICIndex,short SlaveAddr,byte WriteReadFlag);
    int SMBUS_WriteByte(int DevIndex,int IICIndex,short SlaveAddr,byte Data,byte WithPEC);
    int SMBUS_ReadByte(int DevIndex,int IICIndex,short SlaveAddr,byte[] pData,byte WithPEC);
    int SMBUS_WriteByteProtocol(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, byte Data,byte WithPEC);
    int SMBUS_WriteWordProtocol(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, short Data,byte WithPEC);
    int SMBUS_ReadByteProtocol(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pData,byte WithPEC);
    int SMBUS_ReadWordProtocol(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, short[] pData,byte WithPEC);
    int SMBUS_ProcessCall(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, short WriteData, short[] pReadData,byte WithPEC);
    int SMBUS_BlockWrite(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pWriteData,int WriteCount,byte WithPEC);
    int SMBUS_BlockRead(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pReadData,int[] pReadCount,byte WithPEC);
    int SMBUS_BlockProcessCall(int DevIndex,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pWriteData,int WriteCount,byte[] pReadData,int[] pReadCount,byte WithPEC);

    
    //定义SPI通道
    public static int SPI1        = (0x00);
    public static int SPI1_CS0    = (0x00);
    public static int SPI1_CS1    = (0x10);
    public static int SPI1_CS2    = (0x20);
    public static int SPI1_CS3    = (0x30);
    public static int SPI1_CS4    = (0x40);

    public static int SPI2        = (0x01);
    public static int SPI2_CS0    = (0x01);
    public static int SPI2_CS1    = (0x11);
    public static int SPI2_CS2    = (0x21);
    public static int SPI2_CS3    = (0x31);
    public static int SPI2_CS4    = (0x41);
    //定义工作模式
    public static int SPI_MODE_HARD_FDX       = 0; //硬件控制（全双工模式）
    public static int SPI_MODE_HARD_HDX       = 1; //硬件控制（半双工模式）
    public static int SPI_MODE_SOFT_HDX       = 2; //软件控制（半双工模式）
    public static int SPI_MODE_SOFT_ONE_WIRE  = 3; //单总线模式，数据线输入输出都为MOSI
    //定义主从机模式
    public static int SPI_MASTER      = 1; //主机
    public static int SPI_SLAVE       = 0; //从机
    //定义数据移位方式
    public static int SPI_MSB         = 0; //高位在前
    public static int SPI_LSB         = 1; //低位在前
    //定义片选输出极性
    public static int SPI_SEL_LOW     = 0; //片选输出低电平
    public static int SPI_SEL_HIGH    = 1; //片选输出高电平
    //定义EVENT引脚,注意EVENT引脚不要跟SPI通信引脚冲突
    public static int SPI_EVENT_P0    = (1<<0);
    public static int SPI_EVENT_P1    = (1<<1);
    public static int SPI_EVENT_P2    = (1<<2);
    public static int SPI_EVENT_P3    = (1<<3);
    public static int SPI_EVENT_P4    = (1<<4);
    public static int SPI_EVENT_P5    = (1<<5);
    public static int SPI_EVENT_P6    = (1<<6);
    public static int SPI_EVENT_P7    = (1<<7);
    public static int SPI_EVENT_P8    = (1<<8);
    public static int SPI_EVENT_P9    = (1<<9);
    public static int SPI_EVENT_P10    = (1<<10);
    public static int SPI_EVENT_P11    = (1<<11);
    public static int SPI_EVENT_P12    = (1<<12);
    public static int SPI_EVENT_P13    = (1<<13);
    public static int SPI_EVENT_P14    = (1<<14);
    public static int SPI_EVENT_P15    = (1<<15);

    //定义事件类型
    public static int EVENT_TYPE_LOW      = 0x00;
    public static int EVENT_TYPE_HIGH     = 0x11;
    public static int EVENT_TYPE_RISING   = 0x01;
    public static int EVENT_TYPE_FALLING  = 0x10;
    
    //定义函数返回错误代码
    public static int SPI_SUCCESS             = (0);   //函数执行成功
    public static int SPI_ERR_NOT_SUPPORT     = (-1);  //适配器不支持该函数
    public static int SPI_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
    public static int SPI_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
    public static int SPI_ERR_CMD_FAIL        = (-4);  //命令执行失败
    public static int SPI_ERR_PARAMETER       = (-5);  //参数错误
    //定义从机模式下连续读取数据的回调函数
    public interface SPI_GET_DATA_HANDLE{
        int SPI_GetDataHandle(int DevIndex,int SPIIndex,byte[] pData,int DataNum);//接收数据回掉函数
    }
    
    //定义初始化SPI的数据类型
	public class SPI_CONFIG  extends Structure{
		
	    public static class ByReference extends SPI_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends SPI_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"Mode","Master","CPOL","CPHA","LSBFirst","SelPolarity","ClockSpeedHz"});
		}
        public byte   Mode;            //SPI控制方式:0-硬件控制（全双工模式）,1-硬件控制（半双工模式），2-软件控制（半双工模式）,3-单总线模式，数据线输入输出都为MOSI
        public byte   Master;          //主从选择控制:0-从机，1-主机
        public byte   CPOL;            //时钟极性控制:0-SCK空闲时为低电平，1-SCK空闲时为高电平
        public byte   CPHA;            //时钟相位控制:0-第一个SCK时钟采样，1-第二个SCK时钟采样
        public byte   LSBFirst;        //数据移位方式:0-MSB在前，1-LSB在前
        public byte   SelPolarity;     //片选信号极性:0-低电平选中，1-高电平选中
        public int ClockSpeedHz;       //SPI时钟频率:单位为HZ，硬件模式下最大50000000，最小390625，频率按2的倍数改变
    }

    //定义SPI Flash器件配置参数数据类型
	public class SPI_FLASH_CONFIG  extends Structure{
		
	    public static class ByReference extends SPI_FLASH_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends SPI_FLASH_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(
					new String[]{"CMD_WriteEnable","CMD_WriteDisable","CMD_WritePage","WritePageAddressBytes","CMD_EraseSector",
							"EraseSectorAddressBytes","CMD_EraseBulk","CMD_EraseChip","CMD_ReadID","CMD_ReadData","ReadDataAddressBytes",
							"CMD_ReadFast","ReadFastAddressBytes","CMD_ReadStatus","CMD_WriteStatus","ID","ID_Length","PageSize","NumPages","SectorSize"});
		}
        public byte CMD_WriteEnable;          //使能写命令
        public byte CMD_WriteDisable;         //禁止写命令
        public byte CMD_WritePage;            //写数据命令
        public byte WritePageAddressBytes;    //写数据时的地址宽度，单位为字节
        public byte CMD_EraseSector;          //扇区擦出命令
        public byte EraseSectorAddressBytes;  //扇区擦出的地址宽度，单位为字节
        public byte CMD_EraseBulk;            //块擦出命令
        public byte CMD_EraseChip;            //整片擦出命令
        public byte CMD_ReadID;               //读芯片ID命令
        public byte CMD_ReadData;             //读数据命令
        public byte ReadDataAddressBytes;     //读数据时的地址宽度，单位为字节
        public byte CMD_ReadFast;             //快速模式读数据命令
        public byte ReadFastAddressBytes;     //快速读数据时的地址宽度，单位为字节
        public byte CMD_ReadStatus;           //读取状态寄存器命令
        public byte CMD_WriteStatus;          //写状态寄存器命令
        public byte[] ID = new byte[16];      //芯片ID存储数组
        public byte ID_Length;                //ID长度，单位为字节
        public int PageSize;                  //页大小，单位为字节
        public int NumPages;                  //芯片总的页数
        public int SectorSize;                //扇区大小，单位为字节
    }
	//USB2SPI相关函数
	/**
	  * @brief  SPI初始化配置函数
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pConfig SPI初始化配置结构体指针
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_Init(int DevIndex,int SPIIndex, SPI_CONFIG pConfig);

	/**
	  * @brief  SPI发送数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pWriteData 数据发送缓冲区首地址
	  * @param  WriteLen 数据发送字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_WriteBytes(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteLen);

	/**
	  * @brief  SPI异步方式发送数据，函数调用之后不会等待数据发送完毕，该方式可以加大数据发送速率
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pWriteData 数据发送缓冲区首地址
	  * @param  WriteLen 数据发送字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_WriteBytesAsync(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteLen);

	/**
	  * @brief  SPI接收数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pReadData 数据接收缓冲区首地址
	  * @param  ReadLen 接收数据字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_ReadBytes(int DevIndex,int SPIIndex,byte[] pReadData,int ReadLen);

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
	int  SPI_WriteReadBytes(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteLen,byte[] pReadData,int ReadLen,int IntervalTimeUs);

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
	int  SPI_WriteBytesOfEvent(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteLen,int EventPin,byte EventType,int TimeOutOfMs);


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
	int  SPI_ReadBytesOfEvent(int DevIndex,int SPIIndex,byte[] pReadData,int ReadLen,int EventPin,byte EventType,int TimeOutOfMs);

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
	int  SPI_WriteReadBytesOfEvent(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteLen,byte[] pReadData,int ReadLen,int IntervalTimeUs,int EventPin,byte EventType,int TimeOutOfMs);


	/**
	  * @brief  二进制模式写数据，该函数可以发送任意bit数数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pWriteBitStr 二进制数据字符串，如："100110011"
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_WriteBits(int DevIndex,int SPIIndex,byte[] pWriteBitStr);

	/**
	  * @brief  二进制模式读数据，该函数可以读取任意bit数数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pReadBitStr 读取到的二进制数据字符串，如："100110011"
	  * @param  ReadBitsNum 读取数据的二进制bit数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_ReadBits(int DevIndex,int SPIIndex,byte[] pReadBitStr,int ReadBitsNum);

	/**
	  * @brief  二进制模式写读数据，该函数可以写读取任意bit数数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pWriteBitStr 写二进制数据字符串，如："100110011"
	  * @param  pReadBitStr 读取到的二进制数据字符串，如："100110011"
	  * @param  ReadBitsNum 读取数据的二进制bit数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_WriteReadBits(int DevIndex,int SPIIndex,byte[] pWriteBitStr,byte[] pReadBitStr,int ReadBitsNum);

	/**
	  * @brief  SPI从机模式下将数据写入数据发送缓冲区，等待主机的时钟信号再将数据发送出去
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pWriteData 发送数据缓冲区首地址
	  * @param  WriteLen 发送数据字节数
	  * @param  TimeOutMs 等待数据发送完毕超时时间，若小于或者等于0则不会等待数据发送完毕，函数立即返回
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_SlaveWriteBytes(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteLen,int TimeOutMs);

	/**
	  * @brief  SPI从机模式下获取接收数据缓冲区的数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pReadData 数据接收缓冲区首地址
	  * @param  pReadLen 接收到的数据字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_SlaveReadBytes(int DevIndex,int SPIIndex,byte[] pReadData,int TimeOutMs);

	/**
	  * @brief  SPI从机模式下连续读取数据,SPI在从机模式下接收到数据之后，通过回调函数传出数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pSlaveReadDataHandle 从机模式下接收到数据后的回调函数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_SlaveContinueRead(int DevIndex,int SPIIndex,SPI_GET_DATA_HANDLE pSlaveReadDataHandle);

	/**
	  * @brief  SPI从机模式下获取接收数据缓冲区的数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pReadData 数据接收缓冲区首地址
	  * @param  BufferSize 数据缓冲区大小
	  * @retval 获取到的数据字节数
	  */
	int  SPI_SlaveGetBytes(int DevIndex,int SPIIndex,byte[] pReadData,int BufferSize);
	/**
	  * @brief  停止SPI从机模式下连续读取数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_SlaveContinueReadStop(int DevIndex,int SPIIndex);
	/**
	  * @brief  读写SPI-Flash初始化配置
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  ClockSpeed SPI时钟信号频率，单位为Hz,最大为50MHz，依次成倍递减
	  * @param  pConfig SPI-Flash相关参数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashInit(int DevIndex,int SPIIndex,int ClockSpeed, SPI_FLASH_CONFIG pConfig);

	/**
	  * @brief  读取SPI-Flash芯片的ID号
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  pID 芯片ID号存储首地址，其长度由SPI_FLASH_CONFIG中的ID_Length决定
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashReadID(int DevIndex,int SPIIndex,byte[] pID);

	/**
	  * @brief  擦除SPI-Flash的扇区数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  StartSector 起始扇区号，注意是扇区号，不是具体的扇区地址
	  * @param  NumSector 需要擦除的扇区数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashEraseSector(int DevIndex,int SPIIndex,int StartSector,int NumSector);

	/**
	  * @brief  擦除SPI-Flash整个芯片的数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashEraseChip(int DevIndex,int SPIIndex);


	/**
	  * @brief  向SPI-Flash写数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  StartAddr 写数据起始地址
	  * @param  pWriteData 写数据缓冲区首地址
	  * @param  WriteLen 写数据字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashWrite(int DevIndex,int SPIIndex,int StartAddr,byte[] pWriteData,int WriteLen);

	/**
	  * @brief  从SPI-Flash读数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  StartAddr 写数据起始地址
	  * @param  pReadData 读数据缓冲区首地址
	  * @param  ReadLen 读数据字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashRead(int DevIndex,int SPIIndex,int StartAddr,byte[] pReadData,int ReadLen);

	/**
	  * @brief  从SPI-Flash读数据
	  * @param  DevIndex 设备索引号
	  * @param  SPIIndex SPI通道号，取值0或者1
	  * @param  StartAddr 写数据起始地址
	  * @param  pReadData 读数据缓冲区首地址
	  * @param  ReadLen 读数据字节数
	  * @retval 函数执行状态，小于0函数执行出错
	  */
	int  SPI_FlashReadFast(int DevIndex,int SPIIndex,int StartAddr,byte[] pReadData,int ReadLen);

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
	int  SPI_BlockWriteBytes(int DevIndex,int SPIIndex,byte[] pWriteData,int BlockSize,int BlockNum,int IntervalTimeUs);

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
	int  SPI_BlockReadBytes(int DevIndex,int SPIIndex,byte[] pReadData,int BlockSize,int BlockNum,int IntervalTimeUs);

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
	int  SPI_BlockWriteReadBytes(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteBlockSize,byte[] pReadData,int ReadBlockSize,int BlockNum,int IntervalTimeUs);

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
	int  SPI_BlockWriteBytesOfEvent(int DevIndex,int SPIIndex,byte[] pWriteData,int BlockSize,int BlockNum,int EventPin,byte EventType,int TimeOutOfMs);

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
	int  SPI_BlockReadBytesOfEvent(int DevIndex,int SPIIndex,byte[] pReadData,int BlockSize,int BlockNum,int EventPin,byte EventType,int TimeOutOfMs);

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
	int  SPI_BlockWriteReadBytesOfEvent(int DevIndex,int SPIIndex,byte[] pWriteData,int WriteBlockSize,byte[] pReadData,int ReadBlockSize,int BlockNum,int EventPin,byte EventType,int TimeOutOfMs);

	
    //0.函数返回错误代码定义
    public static int   CAN_SUCCESS             =(0);   //函数执行成功
    public static int   CAN_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
    public static int   CAN_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
    public static int   CAN_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
    public static int   CAN_ERR_CMD_FAIL        =(-4);  //命令执行失败
    public static int   CAN_BL_ERR_CONFIG		=(-20); //配置设备错误
    public static int	CAN_BL_ERR_SEND			=(-21); //发送数据出错
    public static int	CAN_BL_ERR_TIME_OUT		=(-22); //超时错误
    public static int	CAN_BL_ERR_CMD			=(-23); //执行命令失败

    public static int   CAN_BL_BOOT   =  0x55555555;
    public static int   CAN_BL_APP    =  0xAAAAAAAA;
    //1.CAN信息帧的数据类型定义
	public class CAN_MSG  extends Structure{
		
	    public static class ByReference extends CAN_MSG implements Structure.ByReference {}  
	    public static class ByValue extends CAN_MSG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"ID","TimeStamp","RemoteFlag","ExternFlag","DataLen","Data"});
		}

        public int  ID;			//报文ID。
        public int  TimeStamp;	//接收到信息帧时的时间标识，从CAN 控制器初始化开始计时。
        public byte RemoteFlag;	//是否是远程帧
        public byte	ExternFlag;	//是否是扩展帧
        public byte	DataLen;	//数据长度(<=8)，即Data 的长度。
        public byte[]	Data = new byte[8];	//报文的数据。
    }

    //2.初始化CAN的数据类型定义
	public class CAN_INIT_CONFIG  extends Structure{
		
	    public static class ByReference extends CAN_INIT_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends CAN_INIT_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"CAN_BRP","CAN_SJW","CAN_BS1","CAN_BS2","CAN_Mode","CAN_ABOM","CAN_NART","CAN_RFLM","CAN_TXFP"});
		}
        //CAN波特率 = 100MHz/(CAN_BRP)/(CAN_SJW+CAN_BS1+CAN_BS2)
        public int	CAN_BRP;	//取值范围1~1024
        public byte	CAN_SJW;	//取值范围1~4
        public byte	CAN_BS1;	//取值范围1~16
        public byte	CAN_BS2;	//取值范围1~8
        public byte	CAN_Mode;	//CAN工作模式，0-正常模式，1-环回模式，2-静默模式，3-静默环回模式
        public byte	CAN_ABOM;	//自动离线管理，0-禁止，1-使能
        public byte	CAN_NART;	//报文重发管理，0-使能报文重传，1-禁止报文重传
        public byte	CAN_RFLM;	//FIFO锁定管理，0-新报文覆盖旧报文，1-丢弃新报文
        public byte	CAN_TXFP;	//发送优先级管理，0-标识符决定，1-发送请求顺序决定
    }


    //3.CAN 滤波器设置数据类型定义
	public class CAN_FILTER_CONFIG  extends Structure{
		
	    public static class ByReference extends CAN_FILTER_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends CAN_FILTER_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"Enable","FilterIndex","FilterMode","ExtFrame","ID_Std_Ext","ID_IDE","ID_RTR","MASK_Std_Ext","MASK_IDE","MASK_RTR"});
		}
        public byte	Enable;			//使能该过滤器，1-使能，0-禁止
        public byte	FilterIndex;	//过滤器索引号，取值范围为0到13
        public byte	FilterMode;		//过滤器模式，0-屏蔽位模式，1-标识符列表模式
        public byte	ExtFrame;		//过滤的帧类型标志，为1 代表要过滤的为扩展帧，为0 代表要过滤的为标准帧。
        public int	ID_Std_Ext;		//验收码ID
        public int	ID_IDE;			//验收码IDE
        public int	ID_RTR;			//验收码RTR
        public int	MASK_Std_Ext;	//屏蔽码ID，该项只有在过滤器模式为屏蔽位模式时有用
        public int	MASK_IDE;		//屏蔽码IDE，该项只有在过滤器模式为屏蔽位模式时有用
        public int	MASK_RTR;		//屏蔽码RTR，该项只有在过滤器模式为屏蔽位模式时有用
    }
    //4.CAN总线状态数据类型定义
	public class CAN_STATUS  extends Structure{
	    public static class ByReference extends CAN_STATUS implements Structure.ByReference {}  
	    public static class ByValue extends CAN_STATUS implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"TSR","ESR","RECounter","TECounter","LECode"});
		}
        public int     TSR;
        public int     ESR;
        public byte    RECounter;	//CAN 控制器接收错误寄存器。
        public byte    TECounter;	//CAN 控制器发送错误寄存器。
        public byte    LECode;     //最后的错误代码
    }
    //5.定义CAN Bootloader命令列表
	public class CBL_CMD_LIST  extends Structure{
	    public static class ByReference extends CBL_CMD_LIST implements Structure.ByReference {}  
	    public static class ByValue extends CBL_CMD_LIST implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"Erase","WriteInfo","Write","Check","SetBaudRate","Excute","CmdSuccess","CmdFaild"});
		}
        //Bootloader相关命令
        public byte   Erase;            //擦出APP储存扇区数据
        public byte   WriteInfo;        //设置多字节写数据相关参数（写起始地址，数据量）
        public byte   Write;            //以多字节形式写数据
        public byte   Check;            //检测节点是否在线，同时返回固件信息
        public byte   SetBaudRate;    //设置节点波特率
        public byte   Excute;            //执行固件
        //节点返回状态
        public byte   CmdSuccess;        //命令执行成功
        public byte   CmdFaild;        //命令执行失败
    }
	//USB2CAN函数定义
	int  CAN_Init(int DevIndex, byte CANIndex, CAN_INIT_CONFIG pCanConfig);
	int  CAN_Filter_Init(int DevIndex, byte CANIndex, CAN_FILTER_CONFIG pFilterConfig);
	int  CAN_SendMsg(int DevIndex, byte CANIndex, CAN_MSG pCanSendMsg,int SendMsgNum);
	int  CAN_GetMsg(int DevIndex, byte CANIndex, CAN_MSG pCanGetMsg);
	int  CAN_GetStatus(int DevIndex, byte CANIndex, CAN_STATUS pCANStatus);

	int  CAN_BL_Init(int DevIndex,int CANIndex,CAN_INIT_CONFIG pInitConfig,CBL_CMD_LIST pCmdList);
	int  CAN_BL_NodeCheck(int DevIndex,int CANIndex,short NodeAddr,int[] pVersion,int[] pType,int TimeOut);
	int  CAN_BL_Erase(int DevIndex,int CANIndex,short NodeAddr,int FlashSize,int TimeOut);
	int  CAN_BL_Write(int DevIndex,int CANIndex,short NodeAddr,int AddrOffset,byte[] pData,int DataNum,int TimeOut);
	int  CAN_BL_Excute(int DevIndex,int CANIndex,short NodeAddr,int Type);
	int  CAN_BL_SetNewBaudRate(int DevIndex,int CANIndex,short NodeAddr,CAN_INIT_CONFIG pInitConfig,int NewBaudRate,int TimeOut);

}
