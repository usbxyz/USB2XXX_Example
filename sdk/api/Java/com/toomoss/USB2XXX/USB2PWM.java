package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2PWM extends Library {
	USB2PWM INSTANCE  = (USB2PWM)Native.loadLibrary("USB2XXX",USB2PWM.class); 
	//定义函数返回错误代码
	public static int PWM_SUCCESS             =(0);   //函数执行成功
	public static int PWM_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
	public static int PWM_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
	public static int PWM_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
	public static int PWM_ERR_CMD_FAIL        =(-4);  //命令执行失败
	
	public class PWM_CONFIG  extends Structure{
		
	    public static class ByReference extends PWM_CONFIG implements Structure.ByReference {}  
	    public static class ByValue extends PWM_CONFIG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"Prescaler","Precision","Pulse","Polarity","ChannelMask"});
		}

		short[] Prescaler = new short[8];  //预分频器
		short[] Precision = new short[8];  //占空比调节精度,实际频率 = 200MHz/(Prescaler*Precision)
		short[] Pulse = new short[8];      //占空比，实际占空比=(Pulse/Precision)*100%
		byte[]  Polarity = new byte[8];    //波形极性，取值0或者1
		byte  	ChannelMask;    		   //通道号，若要使能某个通道，则对应位为1，最低位对应通道0
	};
	
	int  PWM_Init(int DevHandle, PWM_CONFIG pConfig);
	int  PWM_Start(int DevHandle,byte ChannelMask,int RunTimeUs);
	int  PWM_SetPulse(int DevHandle,byte ChannelMask,short[] pPulse);
	int  PWM_SetPhase(int DevHandle,byte ChannelMask,short[] pPhase);
	int  PWM_SetFrequency(int DevHandle,byte ChannelMask,short[] pPrescaler,short[] pPrecision);
	int  PWM_Stop(int DevHandle,byte ChannelMask);
}
