package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2CNT extends Library {
	USB2CNT INSTANCE  = (USB2CNT)Native.loadLibrary("USB2XXX",USB2CNT.class); 
	//定义函数返回错误代码
	public static int CNT_SUCCESS             =(0);   //函数执行成功
	public static int CNT_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
	public static int CNT_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
	public static int CNT_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
	public static int CNT_ERR_CMD_FAIL        =(-4);  //命令执行失败
	//计数器模式
	public static byte COUNTER_MODE_UP     =0;
	public static byte COUNTER_MODE_DOWN   =1;
	//计数器触发条件
	public static byte COUNTER_POL_RISING  =0;
	public static byte COUNTER_POL_FALLING =1;
	public static byte COUNTER_POL_BOTHEDGE=2;
	//计数器宽度
	public static byte COUNTER_BITS16      =16;
	public static byte COUNTER_BITS32      =32;
	//计数器输入引脚模式
	public static byte  COUNTER_PIN_NOPULL =0x00;
	public static byte  COUNTER_PIN_UP     =0x01;
	public static byte  COUNTER_PIN_DOWN   =0x02;
	//计数器通道定义
	public static byte COUNTER_CH0         =0x01;
	public static byte COUNTER_CH1         =0x02;
	public static byte COUNTER_CH2         =0x04;
	public static byte COUNTER_CH3         =0x08;
	
	//定义初始化计数器的数据类型
	public class CNT_CONFIG  extends Structure{
		
	    public static class ByReference extends CNT_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends CNT_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"CounterMode","CounterPolarity","CounterBitWide","CounterPinMode"});
		}

		byte   CounterMode;      //计数模式，0-Up,1-Down
		byte   CounterPolarity;  //计数极性，0-Rising，1-Falling，2-BothEdge
		byte   CounterBitWide;   //计数位宽，16-16bit位宽，32-32bit位宽
		byte   CounterPinMode;   //计数器输入引脚模式，0-浮空输入，1-上拉输入，2-下拉输入
	};
	
	int  CNT_Init(int DevHandle, byte ChannelMask, CNT_CONFIG pConfig);
	int  CNT_Start(int DevHandle, byte ChannelMask);
	int  CNT_Stop(int DevHandle, byte ChannelMask);
	int  CNT_SetValue(int DevHandle, byte ChannelMask,int[] pCntValues);
	int  CNT_GetValue(int DevHandle, byte ChannelMask,int[] pCntValues);
}
