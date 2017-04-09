
#define GET_FIRMWARE_INFO
#define CAN_MODE_LOOPBACK
#define CAN_SEND_MSG
#define CAN_GET_STATUS
#define CAN_GET_MSG

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;  
using System.Linq;
using System.Diagnostics;
using System.Text;
using USB2XXX;




namespace USB2XXXCANTest
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            Int32 DevIndex = 0;
            Byte CANIndex = 0;
            bool state;
            Int32 DevNum, ret;
            //扫描查找设备
            DevNum = usb_device.USB_ScanDevice(null);
            if (DevNum <= 0)
            {
                Console.WriteLine("No device connected!");
                return;
            }
            else
            {
                Console.WriteLine("Have {0} device connected!", DevNum);
            }
            //打开设备
            state = usb_device.USB_OpenDevice(DevIndex);
            if (!state)
            {
                Console.WriteLine("Open device error!");
                return;
            }
            else
            {
                Console.WriteLine("Open device success!");
            }
            //获取固件信息
            StringBuilder FuncStr = new StringBuilder(256);
            state = usb_device.USB_GetDeviceInfo(DevIndex, ref DevInfo, FuncStr);
            if (!state)
            {
                Console.WriteLine("Get device infomation error!");
                return;
            }
            else
            {
                Console.WriteLine("Firmware Info:");
                Console.WriteLine("    Name:" + Encoding.Default.GetString(DevInfo.FirmwareName));
                Console.WriteLine("    Build Date:" + Encoding.Default.GetString(DevInfo.BuildDate));
                Console.WriteLine("    Firmware Version:v{0}.{1}.{2}", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF);
                Console.WriteLine("    Hardware Version:v{0}.{1}.{2}", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF);
                Console.WriteLine("    Functions:" + DevInfo.Functions.ToString("X8"));
                Console.WriteLine("    Functions String:" + FuncStr);
            }
            //初始化配置CAN
            USB2CAN.CAN_INIT_CONFIG CANConfig = new USB2CAN.CAN_INIT_CONFIG();
#if CAN_MODE_LOOPBACK
            CANConfig.CAN_Mode = 1;//环回模式
#else
            CANConfig.CAN_Mode = 0;//正常模式
#endif
            CANConfig.CAN_ABOM = 0;//禁止自动离线
            CANConfig.CAN_NART = 1;//禁止报文重传
            CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
            CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
            //配置波特率,波特率 = 100M/(BRP*(SJW+BS1+BS2))
            CANConfig.CAN_BRP = 25;
            CANConfig.CAN_BS1 = 2;
            CANConfig.CAN_BS2 = 1;
            CANConfig.CAN_SJW = 1;
            ret = USB2CAN.CAN_Init(DevIndex,CANIndex,ref CANConfig);
            if(ret != USB2CAN.CAN_SUCCESS){
                Console.WriteLine("Config CAN failed!");
                return;
            }else{
                Console.WriteLine("Config CAN Success!");
            }
            //配置过滤器，必须配置，否则可能无法收到数据
            USB2CAN.CAN_FILTER_CONFIG CANFilter = new USB2CAN.CAN_FILTER_CONFIG();
            CANFilter.Enable = 1;
            CANFilter.ExtFrame = 0;
            CANFilter.FilterIndex = 0;
            CANFilter.FilterMode = 0;
            CANFilter.MASK_IDE = 0;
            CANFilter.MASK_RTR = 0;
            CANFilter.MASK_Std_Ext = 0;
            USB2CAN.CAN_Filter_Init(DevIndex,CANIndex,ref CANFilter);
#if CAN_SEND_MSG//发送CAN帧
            USB2CAN.CAN_MSG[] CanMsg = new USB2CAN.CAN_MSG[5];
            for(int i=0;i<5;i++){
                CanMsg[i].ExternFlag = 0;
                CanMsg[i].RemoteFlag = 0;
                CanMsg[i].ID = (UInt32)i;
                CanMsg[i].DataLen = 8;
                CanMsg[i].Data = new Byte[CanMsg[i].DataLen];
                for(int j=0;j<CanMsg[i].DataLen;j++){
                    CanMsg[i].Data[j] = (Byte)j;
                }
            }
            int SendedNum = USB2CAN.CAN_SendMsg(DevIndex,CANIndex,CanMsg,(UInt32)CanMsg.Length);
            if(SendedNum >= 0){
                Console.WriteLine("Success send frames:{0}",SendedNum);
            }else{
                Console.WriteLine("Send CAN data failed!");
            }
#endif
#if CAN_GET_STATUS
            USB2CAN.CAN_STATUS CANStatus = new USB2CAN.CAN_STATUS();
            ret = USB2CAN.CAN_GetStatus(DevIndex,CANIndex,ref CANStatus);
            if(ret == USB2CAN.CAN_SUCCESS){
                Console.WriteLine("TSR = {0:X8}",CANStatus.TSR);
                Console.WriteLine("ESR = {0:X8}",CANStatus.ESR);
            }else{
                Console.WriteLine("Get CAN status error!\n");
            }
#endif
            //延时
            System.Threading.Thread.Sleep(100);
            
#if CAN_GET_MSG
            while (true)
            {
                USB2CAN.CAN_MSG[] CanMsgBuffer = new USB2CAN.CAN_MSG[10240];
                for (int i = 0; i < CanMsgBuffer.Length; i++)
                {
                    CanMsgBuffer[i] = new USB2CAN.CAN_MSG();
                    CanMsgBuffer[i].Data = new Byte[8];
                }
                int CanNum = USB2CAN.CAN_GetMsg(DevIndex, CANIndex, CanMsgBuffer);
                if (CanNum > 0)
                {
                    for (int i = 0; i < CanNum; i++)
                    {
                        Console.WriteLine("CanMsg[{0}].ID = {1}", i, CanMsgBuffer[i].ID);
                        Console.WriteLine("CanMsg[{0}].TimeStamp = {1}", i, CanMsgBuffer[i].TimeStamp);
                        Console.Write("CanMsg[{0}].Data = ", i);
                        for (int j = 0; j < CanMsgBuffer[i].DataLen; j++)
                        {
                            Console.Write("{0:X2} ", CanMsgBuffer[i].Data[j]);
                        }
                        Console.WriteLine("\n");
                    }
                }
                else if (CanNum < 0)
                {
                    Console.WriteLine("Get CAN data error!");
                }
                //延时
                System.Threading.Thread.Sleep(100);
            }
#endif
            //关闭设备
            usb_device.USB_CloseDevice(DevIndex);
        }
    }
}
