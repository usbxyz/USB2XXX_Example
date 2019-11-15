#define GET_FIRMWARE_INFO   //获取固件信息

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Diagnostics;
using System.Text;
using USB2XXX;
using System.Threading;
//若运行程序提示无法加载或者找不到USB2XXX.dll文件，将工程目录下的USB2XXX.dll和libusb-1.0.dll文件拷贝到exe程序输出目录下即可
//将CAN1和CAN2对接即可测试
namespace USB2XXX_CAN_MultiThreadTest
{
    class Program
    {
        public static Int32 DevHandle = 0;
        public static Byte CANSendIndex = 0;
        public static Byte CANReadIndex = 1;
        public static object locker = new object();
        public static bool CANReadMsgFlag = false;
        public static bool CANSendMsgFlag = false;
        public static Int32 SendMsgNum = 0;
        public static void SendMsgThread()
        {
            while (CANSendMsgFlag)
            {
                USB2CAN.CAN_MSG[] CanMsg = new USB2CAN.CAN_MSG[10];
                for (int i = 0; i < 10; i++)
                {
                    CanMsg[i] = new USB2CAN.CAN_MSG();
                    CanMsg[i].ExternFlag = 0;
                    CanMsg[i].RemoteFlag = 0;
                    CanMsg[i].ID = (UInt32)i;
                    CanMsg[i].DataLen = 8;
                    CanMsg[i].Data = new Byte[CanMsg[i].DataLen];
                    for (int j = 0; j < CanMsg[i].DataLen; j++)
                    {
                        CanMsg[i].Data[j] = (Byte)j;
                    }
                }

                int SendedNum = USB2CAN.CAN_SendMsg(DevHandle, CANSendIndex, CanMsg, (UInt32)CanMsg.Length);
                if (SendedNum >= 0)
                {
                    SendMsgNum += SendedNum;
                    Console.WriteLine("Success send frames:{0}", SendMsgNum);
                }
                else
                {
                    Console.WriteLine("Send CAN data failed!");
                    break;
                }
                Thread.Sleep(10);
            }
        }
        public static void ReadMsgThread()
        {
            while (CANReadMsgFlag)
            {
                USB2CAN.CAN_MSG[] CanMsgBuffer = new USB2CAN.CAN_MSG[10240];
                for (int i = 0; i < CanMsgBuffer.Length; i++)
                {
                    CanMsgBuffer[i] = new USB2CAN.CAN_MSG();
                    CanMsgBuffer[i].Data = new Byte[8];
                }
                //申请数据缓冲区
                IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2CAN.CAN_MSG)) * CanMsgBuffer.Length);

                int CanNum = USB2CAN.CAN_GetMsg(DevHandle, CANReadIndex, pt);
                if (CanNum > 0)
                {
                    for (int i = 0; i < CanNum; i++)
                    {
                        CanMsgBuffer[i] = (USB2CAN.CAN_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + i * Marshal.SizeOf(typeof(USB2CAN.CAN_MSG))), typeof(USB2CAN.CAN_MSG));
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
                    break;
                }
                //释放数据缓冲区，必须释放，否则程序运行一段时间后会报内存不足
                Marshal.FreeHGlobal(pt);
                Thread.Sleep(10);
            }
        }
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            Int32[] DevHandles = new Int32[20];
            bool state;
            Int32 DevNum, ret;

            Thread CANSendMsgThread = new Thread(new ThreadStart(SendMsgThread));
            Thread CANReadMsgThread = new Thread(new ThreadStart(ReadMsgThread));

            //扫描查找设备
            DevNum = usb_device.USB_ScanDevice(DevHandles);
            if (DevNum <= 0)
            {
                Console.WriteLine("No device connected!");
                return;
            }
            else
            {
                Console.WriteLine("Have {0} device connected!", DevNum);
            }
            DevHandle = DevHandles[0];
            //打开设备
            state = usb_device.USB_OpenDevice(DevHandle);
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
            state = usb_device.DEV_GetDeviceInfo(DevHandle, ref DevInfo, FuncStr);
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
            CANConfig.CAN_Mode = 0x80;//正常模式
            CANConfig.CAN_ABOM = 0;//禁止自动离线
            CANConfig.CAN_NART = 1;//禁止报文重传
            CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
            CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
            //配置波特率,波特率 = 100M/(BRP*(SJW+BS1+BS2))
            CANConfig.CAN_BRP = 15;
            CANConfig.CAN_BS1 = 5;
            CANConfig.CAN_BS2 = 2;
            CANConfig.CAN_SJW = 1;
            ret = USB2CAN.CAN_Init(DevHandle, CANSendIndex, ref CANConfig);
            if (ret != USB2CAN.CAN_SUCCESS)
            {
                Console.WriteLine("Config CAN failed!");
                return;
            }
            else
            {
                Console.WriteLine("Config CAN Success!");
            }
            ret = USB2CAN.CAN_Init(DevHandle, CANReadIndex, ref CANConfig);
            if (ret != USB2CAN.CAN_SUCCESS)
            {
                Console.WriteLine("Config CAN failed!");
                return;
            }
            else
            {
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
            USB2CAN.CAN_Filter_Init(DevHandle, CANReadIndex, ref CANFilter);
            //启动接收数据线程
            CANReadMsgFlag = true;
            CANReadMsgThread.Start();
            //启动发送数据线程
            CANSendMsgFlag = true;
            CANSendMsgThread.Start();
            //按下回车后结束发送接收线程
            Console.ReadLine();
            //结束线程
            CANSendMsgFlag = false;
            CANReadMsgFlag = false;
            CANSendMsgThread.Join();
            CANReadMsgThread.Join();
            //关闭设备
            usb_device.USB_CloseDevice(DevHandle);
        }
    }
}
