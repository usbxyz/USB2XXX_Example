package com.toomoss.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2CAN extends Library {
	USB2CAN INSTANCE  = (USB2CAN)Native.loadLibrary("USB2XXX",USB2CAN.class); 
	
    //0.函数返回错误代码定义
    int CAN_SUCCESS             =(0);   //函数执行成功
    int CAN_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
    int CAN_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
    int CAN_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
    int CAN_ERR_CMD_FAIL        =(-4);  //命令执行失败
    int CAN_BL_ERR_CONFIG		=(-20); //配置设备错误
    int	CAN_BL_ERR_SEND			=(-21); //发送数据出错
    int	CAN_BL_ERR_TIME_OUT		=(-22); //超时错误
    int	CAN_BL_ERR_CMD			=(-23); //执行命令失败

    int CAN_BL_BOOT   =  0x55555555;
    int CAN_BL_APP    =  0xAAAAAAAA;
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
	/**
	 * 初始化配置USB2CAN
	 * @param DevHandle 设备句柄
	 * @param CANIndex CAN通道号
	 * @param pCanConfig CAN通信配置参数结构体
	 * @return
	 */
	int  CAN_Init(int DevHandle, byte CANIndex, CAN_INIT_CONFIG pCanConfig);
	
	/**
	 * 初始化配置CAN过滤器
	 * @param DevHandle 设备句柄
	 * @param CANIndex CAN通道号
	 * @param pFilterConfig 过滤器配置参数结构体
	 * @return
	 */
	int  CAN_Filter_Init(int DevHandle, byte CANIndex, CAN_FILTER_CONFIG pFilterConfig);
	
	/**
	 * 发送CAN总线数据帧
	 * @param DevHandle 设备句柄
	 * @param CANIndex CAN通道号
	 * @param pCanSendMsg 待发送的CAN数据数组
	 * @param SendMsgNum 待发送的CAN数据帧数
	 * @return
	 */
	int  CAN_SendMsg(int DevHandle, byte CANIndex, CAN_MSG[] pCanSendMsg,int SendMsgNum);
	
	/**
	 * 从USB2CAN适配器读取接收到的CAN数据帧
	 * @param DevHandle 设备句柄
	 * @param CANIndex CAN通道号
	 * @param pCanGetMsg 存储CAN数据帧数组
	 * @return
	 */
	int  CAN_GetMsg(int DevHandle, byte CANIndex, CAN_MSG[] pCanGetMsg);
	
	/**
	 * 获取USB2CAN适配器内部状态
	 * @param DevHandle 设备句柄
	 * @param CANIndex CAN通道号
	 * @param pCANStatus CAN状态结构体
	 * @return
	 */
	int  CAN_GetStatus(int DevHandle, byte CANIndex, CAN_STATUS pCANStatus);

	int  CAN_BL_Init(int DevHandle,int CANIndex,CAN_INIT_CONFIG pInitConfig,CBL_CMD_LIST pCmdList);
	int  CAN_BL_NodeCheck(int DevHandle,int CANIndex,short NodeAddr,int[] pVersion,int[] pType,int TimeOut);
	int  CAN_BL_Erase(int DevHandle,int CANIndex,short NodeAddr,int FlashSize,int TimeOut);
	int  CAN_BL_Write(int DevHandle,int CANIndex,short NodeAddr,int AddrOffset,byte[] pData,int DataNum,int TimeOut);
	int  CAN_BL_Excute(int DevHandle,int CANIndex,short NodeAddr,int Type);
	int  CAN_BL_SetNewBaudRate(int DevHandle,int CANIndex,short NodeAddr,CAN_INIT_CONFIG pInitConfig,int NewBaudRate,int TimeOut);
}
