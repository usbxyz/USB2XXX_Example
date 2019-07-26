package com.toomoss.USB2XXX;
import java.util.Arrays;
import java.util.List;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Structure;
public interface USB2OW extends Library  {
    USB2OW INSTANCE  = (USB2OW) Native.loadLibrary("USB2XXX",USB2OW.class);
    //瀹氫箟鍑芥暟杩斿洖閿欒浠ｇ爜
    int OW_SUCCESS             =(0);   //鍑芥暟鎵ц鎴愬姛
    int OW_ERR_NOT_SUPPORT     =(-1);  //閫傞厤鍣ㄤ笉鏀寔璇ュ嚱鏁�
    int OW_ERR_USB_WRITE_FAIL  =(-2);  //USB鍐欐暟鎹け璐�
    int OW_ERR_USB_READ_FAIL   =(-3);  //USB璇绘暟鎹け璐�
    int OW_ERR_CMD_FAIL        =(-4);  //鍛戒护鎵ц澶辫触
    int OW_ERR_NO_ACK          =(-5);  //鏃犱粠璁惧搴旂瓟

    //閫氶亾瀹氫箟
    int OW_CH0             =(0x01<<0);
    int OW_CH1             =(0x01<<1);
    int OW_CH2             =(0x01<<2);
    int OW_CH3             =(0x01<<3);
    int OW_CH4             =(0x01<<4);
    int OW_CH5             =(0x01<<5);
    int OW_CH6             =(0x01<<6);
    int OW_CH7             =(0x01<<7);
    int OW_CH8             =(0x01<<8);
    int OW_CH9             =(0x01<<9);
    int OW_CH10             =(0x01<<10);
    int OW_CH11             =(0x01<<11);
    int OW_CH12             =(0x01<<12);
    int OW_CH13            =(0x01<<13);
    int OW_CH14             =(0x01<<14);
    int OW_CH15             =(0x01<<15);
    //鍐呴儴涓婃媺鐢甸樆瀹氫箟
    byte OW_INTERNAL_NOPULL        =0x00;    //娌℃湁涓婃媺涓嬫媺
    byte OW_INTERNAL_PULL          =0x01;    //浣胯兘涓婃媺,鑺墖鍐呴儴涓婃媺寰堝急锛屽彲鑳戒細鏃犳硶姝ｅ父宸ヤ綔锛屽缓璁娇鐢ㄥ閮ㄤ笂鎷�

    int OW_Init(int DevHandle, int ChannelMask, int ResetTimeUs,byte EnInternalPull);
    int OW_Reset(int DevHandle, int ChannelMask);
    int OW_WriteBytes(int DevHandle, int ChannelMask,byte[] pWriteData,int WriteDataLen);
    int OW_ReadBytes(int DevHandle, int ChannelMask,byte[] pReadData,int ReadDataLen);
    int OW_WriteReadBytes(int DevHandle, int ChannelMask,byte[] pWriteData,int WriteDataLen,byte[] pReadData,int ReadDataLen,int IntervalTimeUs);

}
