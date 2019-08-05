package com.toomoss.USB2XXX;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;
public interface USB2OW extends Library  {
    USB2OW INSTANCE  = (USB2OW) Native.loadLibrary("USB2XXX",USB2OW.class);
    //定义函数返回错误代码
    public static int OW_SUCCESS             =(0);   //函数执行成功
    public static int OW_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
    public static int OW_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
    public static int OW_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
    public static int OW_ERR_CMD_FAIL        =(-4);  //命令执行失败
    public static int OW_ERR_NO_ACK          =(-5);  //无从设备应答

    //通道定义
    public static int OW_CH0  =(0x01<<0);
    public static int OW_CH1  =(0x01<<1);
    public static int OW_CH2  =(0x01<<2);
    public static int OW_CH3  =(0x01<<3);
    public static int OW_CH4  =(0x01<<4);
    public static int OW_CH5  =(0x01<<5);
    public static int OW_CH6  =(0x01<<6);
    public static int OW_CH7  =(0x01<<7);
    public static int OW_CH8  =(0x01<<8);
    public static int OW_CH9  =(0x01<<9);
    public static int OW_CH10 =(0x01<<10);
    public static int OW_CH11 =(0x01<<11);
    public static int OW_CH12 =(0x01<<12);
    public static int OW_CH13 =(0x01<<13);
    public static int OW_CH14 =(0x01<<14);
    public static int OW_CH15 =(0x01<<15);
    //内部上拉电阻定义
    byte OW_INTERNAL_NOPULL   =0x00;    //没有上拉下拉
    byte OW_INTERNAL_PULL     =0x01;    //使能上拉,芯片内部上拉很弱，可能会无法正常工作，建议使用外部上拉

    int OW_Init(int DevHandle, int ChannelMask, int ResetTimeUs,byte EnInternalPull);
    int OW_Reset(int DevHandle, int ChannelMask);
    int OW_WriteBytes(int DevHandle, int ChannelMask,byte[] pWriteData,int WriteDataLen);
    int OW_ReadBytes(int DevHandle, int ChannelMask,byte[] pReadData,int ReadDataLen);
    int OW_WriteReadBytes(int DevHandle, int ChannelMask,byte[] pWriteData,int WriteDataLen,byte[] pReadData,int ReadDataLen,int IntervalTimeUs);

}
