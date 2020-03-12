package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2LIN_EX extends Library {
	USB2LIN_EX INSTANCE  = (USB2LIN_EX) Native.loadLibrary("USB2XXX",USB2LIN_EX.class);
	//定义LIN消息帧结构体
	public class LIN_EX_MSG  extends Structure{
		
	    public static class ByReference extends LIN_EX_MSG implements Structure.ByReference {}  
	    public static class ByValue extends LIN_EX_MSG implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"Timestamp", "MsgType", "CheckType", "DataLen","Sync","PID","Data","Check","BreakBits","Reserve1"});
		}
	    
		public int  Timestamp;    			//从机接收数据时代表时间戳，单位为0.1ms;主机读写数据时，表示数据读写后的延时时间，单位为ms
	    public byte MsgType;      			//帧类型
	    public byte CheckType;    			//校验类型
	    public byte DataLen;      			//LIN数据段有效数据字节数
	    public byte Sync;         			//固定值，0x55
	    public byte PID;          			//帧ID
	    public byte[] Data = new byte[8];	//数据
	    public byte Check;        			//校验,只有校验数据类型为LIN_EX_CHECK_USER的时候才需要用户传入数据
	    public byte BreakBits;    			//该帧的BRAK信号位数，有效值为10到26，若设置为其他值则默认为13位
	    public byte Reserve1;				//保留字节
	}

	//定义函数返回错误代码
	public static int LIN_EX_SUCCESS           =  (0);   //函数执行成功
	public static int LIN_EX_ERR_NOT_SUPPORT   =  (-1);  //适配器不支持该函数
	public static int LIN_EX_ERR_USB_WRITE_FAIL=  (-2);  //USB写数据失败
	public static int LIN_EX_ERR_USB_READ_FAIL =  (-3);  //USB读数据失败
	public static int LIN_EX_ERR_CMD_FAIL      =  (-4);  //命令执行失败
	public static int LIN_EX_ERR_CH_NO_INIT    =  (-5);  //该通道未初始化
	public static int LIN_EX_ERR_READ_DATA     =  (-6);  //LIN读数据失败
	public static int LIN_EX_ERR_PARAMETER     =  (-7);  //函数参数传入有误
	//校验类型
	public static byte LIN_EX_CHECK_STD    = 0;  //标准校验，不含PID
	public static byte LIN_EX_CHECK_EXT    = 1;  //增强校验，包含PID
	public static byte LIN_EX_CHECK_USER   = 2;  //自定义校验类型，需要用户自己计算并传入Check，不进行自动校验
	public static byte LIN_EX_CHECK_NONE   = 3;  //不计算校验数据
	public static byte LIN_EX_CHECK_ERROR  = 4;  //接收数据校验错误
	//主从机定义
	public static byte LIN_EX_MASTER       = 1;  //主机
	public static byte LIN_EX_SLAVE        = 0;  //从机
	//帧类型
	public static byte LIN_EX_MSG_TYPE_UN  = 0;  //未知类型
	public static byte LIN_EX_MSG_TYPE_MW  = 1;  //主机向从机发送数据
	public static byte LIN_EX_MSG_TYPE_MR  = 2;  //主机从从机读取数据
	public static byte LIN_EX_MSG_TYPE_SW  = 3;  //从机发送数据
	public static byte LIN_EX_MSG_TYPE_SR  = 4;  //从机接收数据
	public static byte LIN_EX_MSG_TYPE_BK  = 5;  //只发送BREAK信号，若是反馈回来的数据，表明只检测到BREAK信号
	public static byte LIN_EX_MSG_TYPE_SY  = 6;  //表明检测到了BREAK，SYNC信号
	public static byte LIN_EX_MSG_TYPE_ID  = 7;  //表明检测到了BREAK，SYNC，PID信号
	public static byte LIN_EX_MSG_TYPE_DT  = 8;  //表明检测到了BREAK，SYNC，PID,DATA信号
	public static byte LIN_EX_MSG_TYPE_CK  = 9;  //表明检测到了BREAK，SYNC，PID,DATA,CHECK信号
	
    //USB2LIN相关函数定义
	int LIN_EX_Init(int DevHandle,byte LINIndex,int BaudRate,byte MasterMode);
	int LIN_EX_MasterSync(int DevHandle,byte LINIndex,LIN_EX_MSG[] pInMsg,LIN_EX_MSG[] pOutMsg, int MsgLen);
    int LIN_EX_MasterWrite(int DevHandle,byte LINIndex,byte PID,byte[] pData,byte DataLen,byte CheckType);
    int LIN_EX_MasterRead(int DevHandle,byte LINIndex,byte PID,byte[] pData);
	int LIN_EX_SlaveSetIDMode(int DevHandle,byte LINIndex,LIN_EX_MSG[] pLINMsg, int MsgLen);
	int LIN_EX_SlaveGetIDMode(int DevHandle,byte LINIndex,LIN_EX_MSG[] pLINMsg);
	int LIN_EX_SlaveGetData(int DevHandle,byte LINIndex,LIN_EX_MSG[] pLINMsg);
	int LIN_EX_CtrlPowerOut(int DevHandle,byte State);
	int LIN_EX_GetVbatValue(int DevHandle,short[] pBatValue);

	int LIN_EX_MasterStartSch(int DevHandle,byte LINIndex,LIN_EX_MSG[] pLINMsg,int MsgLen);
	int LIN_EX_MasterStopSch(int DevHandle,byte LINIndex);
	int LIN_EX_MasterGetSch(int DevHandle,byte LINIndex,LIN_EX_MSG[] pLINMsg);
	
}
