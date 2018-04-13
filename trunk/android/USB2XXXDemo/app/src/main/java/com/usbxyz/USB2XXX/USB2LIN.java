package com.usbxyz.USB2XXX;

import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;

public interface USB2LIN extends Library {
    USB2LIN INSTANCE  = (USB2LIN) Native.loadLibrary("USB2XXX",USB2LIN.class);

    //定义函数返回错误代码
    int LIN_SUCCESS             =(0) ;  //函数执行成功
    int LIN_ERR_NOT_SUPPORT     =(-1);  //适配器不支持该函数
    int LIN_ERR_USB_WRITE_FAIL  =(-2);  //USB写数据失败
    int LIN_ERR_USB_READ_FAIL   =(-3);  //USB读数据失败
    int LIN_ERR_CMD_FAIL        =(-4);  //命令执行失败
    int LIN_ERR_CH_NO_INIT      =(-5);  //该通道未初始化
    int LIN_ERR_READ_DATA       =(-6);  //LIN读数据失败
    //LIN和校验模式
    byte LIN_CHECK_MODE_STD     =0;
    byte LIN_CHECK_MODE_EXT     =1;
    byte LIN_CHECK_MODE_NONE    =2;
    //BREAK位数定义
    byte LIN_BREAK_BITS_10    =0x00;
    byte LIN_BREAK_BITS_11    =0x20;
    //定义主从模式
    byte LIN_MASTER          =1;
    byte LIN_SLAVE           =0;
    //定义从机操作模式
    byte LIN_SLAVE_WRITE     =0;
    byte LIN_SLAVE_READ      =1;

    public class LIN_CONFIG  extends Structure{

        public static class ByReference extends LIN_CONFIG implements Structure.ByReference {}
        public static class ByValue extends LIN_CONFIG implements Structure.ByValue {}

        @Override
        protected List getFieldOrder() {
            // TODO Auto-generated method stub
            return Arrays.asList(new String[]{"BaudRate", "CheckMode", "MasterMode", "BreakBits"});
        }

        public int  BaudRate;     //波特率,最大20K
        public byte CheckMode;    //校验模式，0-标准校验模式，1-增强校验模式（包含PID）
        public byte MasterMode;   //主从模式，0-从模式，1-主模式
        public byte BreakBits;    //Break长度，0x00-10bit,0x20-11bit
    }

    public class SLAVE_LIN_DATA  extends Structure{

        public static class ByReference extends SLAVE_LIN_DATA implements Structure.ByReference {}
        public static class ByValue extends SLAVE_LIN_DATA implements Structure.ByValue {}

        @Override
        protected List getFieldOrder() {
            // TODO Auto-generated method stub
            return Arrays.asList(new String[]{"DataLen", "WorkMode", "Data"});
        }

        public byte DataLen;  //从模式下发送数据的长度或者从模式接收数据的长度，不含校验字节
        public byte WorkMode; //0-从模式发送数据，1-从模式接收数据
        public byte[] Data = new byte[9];  //从模式下接收到的数据字节或者待发送的数据字节
    }

    int LIN_Init(int DevHandle,byte Channel,LIN_CONFIG pConfig);
    int LIN_WriteData(int DevHandle,byte Channel,byte ID,byte[] pWriteData,byte WriteLen);
    int LIN_ReadData(int DevHandle,byte Channel,byte ID,byte[] pReadData);
    int LIN_SlaveSetIDOperation(int DevHandle,byte Channel,byte ID,SLAVE_LIN_DATA SlaveOperationData);
    int LIN_SlaveGetData(int DevHandle,byte Channel,SLAVE_LIN_DATA[] pSlaveData);
}
