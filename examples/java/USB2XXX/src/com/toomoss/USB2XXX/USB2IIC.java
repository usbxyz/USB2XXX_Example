package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2IIC extends Library {
	USB2IIC INSTANCE  = (USB2IIC)Native.loadLibrary("USB2XXX",USB2IIC.class);
    //定义函数返回错误代码
    public static int IIC_SUCCESS             = (0);   //函数执行成功
    public static int IIC_ERR_NOT_SUPPORT     = (-1) ; //适配器不支持该函数
    public static int IIC_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
    public static int IIC_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
    public static int IIC_ERR_CMD_FAIL        = (-4);  //命令执行失败
    public static int IIC_ERR_PARA_ERROR      = (-5);  //参数传入错误
    //定义IIC函数返回错误代码
    public static int IIC_ERROR_SUCCESS     = 0;  //操作成功
    public static int IIC_ERROR_CHANNEL     = 1;  //该通道不支持该函数
    public static int IIC_ERROR_BUSY        = 2;  //总线忙
    public static int IIC_ERROR_START_FAILD = 3;  //启动总线失败
    public static int IIC_ERROR_TIMEOUT     = 4;  //超时
    public static int IIC_ERROR_NACK        = 5;  //从机无应答
    public static int IIC_ERROR_PEC         = 6;  //PEC校验失败

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
    //USB2IIC相关函数定义
    int IIC_Init(int DevHandle,int IICIndex, IIC_CONFIG pConfig);
    int IIC_GetSlaveAddr(int DevHandle,int IICIndex,short[] pSlaveAddr);
    int IIC_WriteBytes(int DevHandle,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,int TimeOutMs);
    int IIC_ReadBytes(int DevHandle,int IICIndex,short SlaveAddr,byte[] pReadData,int ReadLen,int TimeOutMs);
    int IIC_WriteReadBytes(int DevHandle,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,byte[] pReadData,int ReadLen,int TimeOutMs);
    int IIC_SlaveWriteBytes(int DevHandle,int IICIndex,byte[] pWriteData,int WriteLen,int TimeOutMs);
    int IIC_SlaveReadBytes(int DevHandle,int IICIndex,byte[] pReadData,int TimeOutMs);
    int IIC_SlaveWriteRemain(int DevHandle,int IICIndex);

    int IIC_WriteBytesOfEvent(int DevHandle,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,int EventPin,byte EventType,int TimeOutOfEventMs);
    int IIC_ReadBytesOfEvent(int DevHandle,int IICIndex,short SlaveAddr,byte[] pReadData,int ReadLen,int EventPin,byte EventType,int TimeOutOfEventMs);
    int IIC_WriteReadBytesOfEvent(int DevHandle,int IICIndex,short SlaveAddr,byte[] pWriteData,int WriteLen,byte[] pReadData,int ReadLen,int EventPin,byte EventType,int TimeOutOfEventMs);

    int SMBUS_Init(int DevHandle,int IICIndex,int ClockSpeedHz,byte EnablePu);
    int SMBUS_QuickCommand(int DevHandle,int IICIndex,short SlaveAddr,byte WriteReadFlag);
    int SMBUS_WriteByte(int DevHandle,int IICIndex,short SlaveAddr,byte Data,byte WithPEC);
    int SMBUS_ReadByte(int DevHandle,int IICIndex,short SlaveAddr,byte[] pData,byte WithPEC);
    int SMBUS_WriteByteProtocol(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, byte Data,byte WithPEC);
    int SMBUS_WriteWordProtocol(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, short Data,byte WithPEC);
    int SMBUS_ReadByteProtocol(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pData,byte WithPEC);
    int SMBUS_ReadWordProtocol(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, short[] pData,byte WithPEC);
    int SMBUS_ProcessCall(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, short WriteData, short[] pReadData,byte WithPEC);
    int SMBUS_BlockWrite(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pWriteData,int WriteCount,byte WithPEC);
    int SMBUS_BlockRead(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pReadData,int[] pReadCount,byte WithPEC);
    int SMBUS_BlockProcessCall(int DevHandle,int IICIndex,short SlaveAddr,byte CommandCode, byte[] pWriteData,int WriteCount,byte[] pReadData,int[] pReadCount,byte WithPEC);

}
