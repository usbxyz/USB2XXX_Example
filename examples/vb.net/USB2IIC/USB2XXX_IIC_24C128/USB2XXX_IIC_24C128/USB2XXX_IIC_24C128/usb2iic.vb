Imports System.Runtime.InteropServices
Module usb2iic
    '定义初始化IIC的数据类型
    Public Structure IIC_CONFIG
        Dim ClockSpeedHz As UInt32  'IIC时钟频率:单位为Hz
        Dim OwnAddr As UInt16		'USB2XXX为从机时自己的地址
        Dim Master As Byte			'主从选择控制:0-从机，1-主机
        Dim AddrBits As Byte		'从机地址模式，7-7bit模式，10-10bit模式
		Dim EnablePu As Byte		'使能引脚芯片内部上拉电阻，若不使能，则I2C总线上必须接上拉电阻
    End Structure

    '定义函数返回错误代码
    Public Const IIC_SUCCESS As Int32 = (0)             '函数执行成功
    Public Const IIC_ERR_NOT_SUPPORT As Int32 = (-1)    '适配器不支持该函数
    Public Const IIC_ERR_USB_WRITE_FAIL As Int32 = (-2) 'USB写数据失败
    Public Const IIC_ERR_USB_READ_FAIL As Int32 = (-3)  'USB读数据失败
    Public Const IIC_ERR_CMD_FAIL As Int32 = (-4)       '命令执行失败
    Public Const IIC_ERR_PARA_ERROR As Int32 = (-5)     '参数传入错误

    '定义IIC函数返回错误代码
    Public Const IIC_ERROR_SUCCESS As Byte = 0		'操作成功
    Public Const IIC_ERROR_CHANNEL As Byte = 1		'该通道不支持该函数
    Public Const IIC_ERROR_BUSY As Byte = 2			'总线忙,SDA或者SCL为低电平
    Public Const IIC_ERROR_START_FAILD As Byte = 3	'动总线失败
    Public Const IIC_ERROR_TIMEOUT As Byte = 4		'超时时间内没有检测到ACK
    Public Const IIC_ERROR_NACK As Byte = 5			'从机无应答
    Public Const IIC_ERROR_PEC As Byte = 6			'PEC校验失败

	Declare Function IIC_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32, ByRef pConfig As IIC_CONFIG) As Int32
	Declare Function IIC_GetSlaveAddr Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,<Out()> ByVal pSlaveAddr As UInt16()) As Int32
	Declare Function IIC_WriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<[In]()> ByVal pWriteData As Byte(),ByVal WriteLen As Int32,ByVal TimeOutMs As Int32) As Int32
	Declare Function IIC_ReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<Out()> ByVal pReadData As Byte(),ByVal ReadLen As Int32,ByVal TimeOutMs As Int32) As Int32
	Declare Function IIC_WriteReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<[In]()> ByVal pWriteData As Byte(),ByVal WriteLen As Int32,<Out()> ByVal pReadData As Byte(),ByVal ReadLen As Int32,ByVal TimeOutMs As Int32) As Int32
	Declare Function IIC_SlaveWriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,<[In]()> ByVal pWriteData As Byte(),ByVal WriteLen As Int32,ByVal TimeOutMs As Int32) As Int32
	Declare Function IIC_SlaveReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,<Out()> ByVal pReadData As Byte(),ByVal TimeOutMs As Int32) As Int32
	Declare Function IIC_SlaveWriteRemain Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32) As Int32

	Declare Function IIC_WriteBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<[In]()> ByVal pWriteData As Byte(),ByVal WriteLen As Int32,ByVal EventPin As Int32,ByVal EventType As Byte,ByVal TimeOutOfEventMs As Int32) As Int32
	Declare Function IIC_ReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<Out()> ByVal pReadData As Byte(),ByVal ReadLen As Int32,ByVal EventPin As Int32,ByVal EventType As Byte,ByVal TimeOutOfEventMs As Int32) As Int32
	Declare Function IIC_WriteReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<[In]()> ByVal pWriteData As Byte(),ByVal WriteLen As Int32,<Out()> ByVal pReadData As Byte(),ByVal ReadLen As Int32,ByVal EventPin As Int32,ByVal EventType As Byte,ByVal TimeOutOfEventMs As Int32) As Int32

	Declare Function SMBUS_Init Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal ClockSpeedHz As Int32,ByVal EnablePu As Byte) As Int32
	Declare Function SMBUS_QuickCommand Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal WriteReadFlag As Byte) As Int32
	Declare Function SMBUS_WriteByte Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal Data As Byte,ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_ReadByte Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,<Out()> ByVal pData As Byte(),ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_WriteByteProtocol Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, ByVal Data As Byte,ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_WriteWordProtocol Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, ByVal Data As UInt16,ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_ReadByteProtocol Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, <Out()> ByVal pData As Byte(),ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_ReadWordProtocol Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, <Out()> ByVal pData As UInt16(),ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_ProcessCall Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, ByVal WriteData As UInt16, <Out()> ByVal pReadData As UInt16(),ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_BlockWrite Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, <[In]()> ByVal pWriteData As Byte(),ByVal WriteCount As Int32,ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_BlockRead Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, <Out()> ByVal pReadData As Byte(),<Out()> ByVal pReadCount As Int32(),ByVal WithPEC As Byte) As Int32
	Declare Function SMBUS_BlockProcessCall Lib "USB2XXX.dll" (ByVal DevHandle As UInt32,ByVal IICIndex As Int32,ByVal SlaveAddr As UInt16,ByVal CommandCode As Byte, <[In]()> ByVal pWriteData As Byte(),ByVal WriteCount As Int32, <Out()> ByVal pReadData As Byte(),<Out()> ByVal pReadCount As Int32(),ByVal WithPEC As Byte) As Int32


End Module
