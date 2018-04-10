package com.usbxyz.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB_Device extends Library{
	USB_Device INSTANCE  = (USB_Device)Native.loadLibrary("USB2XXX",USB_Device.class); 
	//定义设备的PID和VID
	int DEV_PID = 0x7918;
	int DEV_VID = 0x0483;
	public class DEVICE_INFO  extends Structure{
		
	    public static class ByReference extends DEVICE_INFO implements Structure.ByReference {}  
	    public static class ByValue extends DEVICE_INFO implements Structure.ByValue {}  
	  
		@Override
		protected List getFieldOrder() {
			// TODO Auto-generated method stub
			return Arrays.asList(new String[]{"FirmwareName", "BuildDate", "HardwareVersion", "FirmwareVersion", "SerialNumber", "Functions"});
		}
	    
		public byte[]   FirmwareName = new byte[32];   	//固件名称字符串
		public byte[]   BuildDate = new byte[32];      	//固件编译时间字符串
		public int     	HardwareVersion;    			//硬件版本号
		public int     	FirmwareVersion;    			//固件版本号
		public int[]    SerialNumber = new int[3];    	//适配器序列号
		public int     	Functions;          			//适配器当前具备的功能
	}
	/**
	  * @brief  初始化USB设备，并扫描设备连接数，必须调用
	  * @param  pDevNum 每个设备的设备号存储地址，若不需要设备号，可以传入NULL
	  * @retval 扫描到的设备数量
	  */
	int  USB_ScanDevice(int[] pDevHandle,int fd);
	/**
	  * @brief  打开设备，必须调用
	  * @param  DevHandle 设备句柄
	  * @retval 打开设备的状态
	  */
	boolean USB_OpenDevice(int DevHandle);

	/**
	  * @brief  关闭设备
	  * @param  DevHandle 设备句柄
	  * @retval 关闭设备的状态
	  */
	boolean USB_CloseDevice(int DevHandle);

	/**
	  * @brief  获取设备信息，比如设备名称，固件版本号，设备序号，设备功能说明字符串等
	  * @param  DevHandle 设备句柄
	  * @param  pDevInfo 设备信息存储结构体指针
	  * @param  pFunctionStr 设备功能说明字符串
	  * @retval 获取设备信息的状态
	  */
	boolean DEV_GetDeviceInfo(int DevHandle,DEVICE_INFO pDevInfo,byte[] pFunctionStr);

	/**
	  * @brief  擦出用户区数据
	  * @param  DevHandle 设备句柄
	  * @retval 用户区数据擦出状态
	  */
	boolean DEV_EraseUserData(int DevHandle);

	/**
	  * @brief  向用户区域写入用户自定义数据，写入数据之前需要调用擦出函数将数据擦出
	  * @param  DevHandle 设备句柄
	  * @param  OffsetAddr 数据写入偏移地址，起始地址为0x00，用户区总容量为0x10000字节，也就是64KBye
	  * @param  pWriteData 用户数据缓冲区首地址
	  * @param  DataLen 待写入的数据字节数
	  * @retval 写入用户自定义数据状态
	  */
	boolean DEV_WriteUserData(int DevHandle,int OffsetAddr,byte[] pWriteData,int DataLen);

	/**
	  * @brief  从用户自定义数据区读出数据
	  * @param  DevHandle 设备句柄
	  * @param  OffsetAddr 数据写入偏移地址，起始地址为0x00，用户区总容量为0x10000字节，也就是64KBye
	  * @param  pReadData 用户数据缓冲区首地址
	  * @param  DataLen 待读出的数据字节数
	  * @retval 读出用户自定义数据的状态
	  */
	boolean DEV_ReadUserData(int DevHandle,int OffsetAddr,byte[] pReadData,int DataLen);

	/**
	 * @brief  设置可变电压输出引脚输出电压值
	 * @param  DevHandle 设备索引号
	 * @param  PowerLevel 输出电压值，0-1.8V，1-3.3V，2-不输出
	 * @retval 设置输出电压状态
	 */
	boolean DEV_SetPowerLevel(int DevHandle,char PowerLevel);
}
