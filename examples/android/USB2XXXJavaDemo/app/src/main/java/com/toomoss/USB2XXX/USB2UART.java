package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2UART extends Library {
	USB2UART INSTANCE  = (USB2UART)Native.loadLibrary("USB2XXX",USB2UART.class); 
	//定义函数返回错误代码
	public static int UART_SUCCESS             =(0);   //函数执行成功
	public static int UART_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
	public static int UART_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
	public static int UART_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
	public static int UART_ERR_CMD_FAIL        =(-4);  //命令执行失败
	//数据位宽
	public static byte UART_WORD_LENGTH_8BIT     =0;
	public static byte UART_WORD_LENGTH_9BIT     =1;
	//停止位
	public static byte UART_STOP_BITS_1          =0;
	public static byte UART_STOP_BITS_05         =1;
	public static byte UART_STOP_BITS_2          =2;
	public static byte UART_STOP_BITS_15         =3;
	//奇偶校验位
	public static byte UART_PARITY_NO            =0;
	public static byte UART_PARITY_EVEN          =4;
	public static byte UART_PARITY_ODD           =6;
	//TE控制信号输出
	public static byte UART_TE_DISEN             =0x00;
	public static byte UART_TE_EN_LOW            =(byte) 0x80;
	public static byte UART_TE_EN_HIGH           =(byte) 0x81;
	//使能内部上拉电阻，CAN&LIN适配器支持
	public static byte UART_PPR_DISABLE          =0x40;
	public static byte UART_PPR_ENABLE           =0x41;
	public class UART_CONFIG  extends Structure{
		
	    class ByReference extends UART_CONFIG implements Structure.ByReference {}  
	    class ByValue extends UART_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"BaudRate","WordLength","StopBits","Parity","TEPolarity"});
		}

		int  BaudRate;     //波特率
		byte WordLength;   //数据位宽，0-8bit,1-9bit
		byte StopBits;     //停止位宽，0-1bit,1-0.5bit,2-2bit,3-1.5bit
		byte Parity;       //奇偶校验，0-No,4-Even,6-Odd
		byte TEPolarity;   //TE输出控制，0x80-输出TE信号，且低电平有效，0x81-输出TE信号，且高电平有效，0x00不输出TE信号
	};
	
	int  UART_Init(int DevHandle, byte Channel, UART_CONFIG pConfig);
	int  UART_WriteBytes(int DevHandle,byte Channel,byte[] pWriteData,int DataSize);
	int  UART_WriteBytesAsync(int DevHandle,byte Channel,byte[] pWriteData,int DataSize);
	int  UART_ReadBytes(int DevHandle,byte Channel,byte[] pReadData,int TimeOutMs);
	int  UART_ClearData(int DevHandle,byte Channel);
}
