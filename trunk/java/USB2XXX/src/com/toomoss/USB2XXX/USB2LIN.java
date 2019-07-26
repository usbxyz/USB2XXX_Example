package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2LIN extends Library {
    USB2LIN INSTANCE  = (USB2LIN) Native.loadLibrary("USB2XXX",USB2LIN.class);
	//初始化配置结构体
	public class LIN_CONFIG  extends Structure{
		
	    public static class ByReference extends LIN_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends LIN_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"BaudRate", "CheckMode", "MasterMode", "BreakBits"});
		}
	    
		public int    BaudRate;     //波特率,最大20K
		public byte   CheckMode;    //校验模式，0-标准校验模式，1-增强校验模式（包含PID）
		public byte   MasterMode;   //主从选择控制:0-从机，1-主机
		public byte   BreakBits;    //Break长度，0x00-10bit,0x20-11bit,主机模式无效，从机模式推荐设置为11bit
	}

	//LIN消息结构图定义
	public class LIN_MSG  extends Structure{
		
	    public static class ByReference extends LIN_MSG implements Structure.ByReference {}  
	    public static class ByValue extends LIN_MSG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"ID", "DataLen", "Data"});
		}
	    
		public byte   ID;           //ID，取值范围0~0x3F,不含校验位
		public byte   DataLen;      //发送数据时，代表发送数据的长度，不含校验数据，接收数据时，数据的长度，包含校验数据
		public byte[] Data = new byte[9];   //数据存储区,从机模式接收数据时，最后一个数据为校验数据
	}
    //定义函数返回错误代码
    public static int LIN_SUCCESS             = (0);   //函数执行成功
    public static int LIN_ERR_NOT_SUPPORT     = (-1);  //适配器不支持该函数
    public static int LIN_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
    public static int LIN_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
    public static int LIN_ERR_CMD_FAIL        = (-4);  //命令执行失败
    public static int LIN_ERR_CH_NO_INIT      = (-5);  //该通道未初始化
    public static int LIN_ERR_READ_DATA       = (-6);  //LIN读数据失败
    //LIN和校验模式
    public static byte LIN_CHECK_MODE_STD     = 0;
    public static byte LIN_CHECK_MODE_EXT     = 1;
    public static byte LIN_CHECK_MODE_NONE    = 2;
    //BREAK位数定义
    public static byte LIN_BREAK_BITS_10    = 0x00;
    public static byte LIN_BREAK_BITS_11    = 0x20;
    //定义主从模式
    public static byte LIN_MASTER          = 1;
    public static byte LIN_SLAVE           = 0;
    //定义从机操作模式
    public static byte LIN_SLAVE_WRITE     = 0;
    public static byte LIN_SLAVE_READ      = 1;
    //USB2LIN相关函数定义
    int LIN_Init(int DevHandle,byte LINIndex,LIN_CONFIG pConfig);
    int LIN_SendBreak(int DevHandle,byte LINIndex);
    int LIN_Write(int DevHandle,byte LINIndex,LIN_MSG[] pLINMsg,int Len);
    int LIN_Read(int DevHandle,byte LINIndex,LIN_MSG[] pLINMsg,int Len);
    int LIN_SlaveSetIDMode(int DevHandle,byte LINIndex,byte IDMode,LIN_MSG[] pLINMsg,int Len);
    int LIN_SlaveGetData(int DevHandle,byte LINIndex,LIN_MSG[] pLINMsg);
}
