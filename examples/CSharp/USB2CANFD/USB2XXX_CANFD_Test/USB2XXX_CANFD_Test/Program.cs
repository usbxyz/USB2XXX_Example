using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Diagnostics;
using System.Text;
using USB2XXX;
//若运行程序提示无法加载或者找不到USB2XXX.dll文件，将工程目录下的USB2XXX.dll和libusb-1.0.dll文件拷贝到exe程序输出目录下即可
namespace USB2XXX_CANFD_Test
{
    class Program
    {
        static void Main(string[] args)
        {
            USB_DEVICE.DEVICE_INFO DevInfo = new USB_DEVICE.DEVICE_INFO();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Byte CANIndex = 0;
            bool state;
            Int32 DevNum, ret;
            //扫描查找设备
            DevNum = USB_DEVICE.USB_ScanDevice(DevHandles);
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
            state = USB_DEVICE.USB_OpenDevice(DevHandle);
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
            state = USB_DEVICE.DEV_GetDeviceInfo(DevHandle, ref DevInfo, FuncStr);
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
            //初始化配置CANFD设备
            USB2CANFD.CANFD_INIT_CONFIG CANFDConfig = new USB2CANFD.CANFD_INIT_CONFIG();
            CANFDConfig.Mode = 0;        //0-正常模式，1-自发自收模式
            CANFDConfig.RetrySend = 1;   //使能自动重传
            CANFDConfig.ISOCRCEnable = 1;//使能ISOCRC
            CANFDConfig.ResEnable = 1;   //使能内部终端电阻（若总线上没有终端电阻，则必须使能终端电阻才能正常传输数据）
            //波特率参数可以用TCANLINPro软件里面的波特率计算工具计算
            //仲裁段波特率参数,波特率=40M/NBT_BRP*(1+NBT_SEG1+NBT_SEG2)
            CANFDConfig.NBT_BRP = 1;
            CANFDConfig.NBT_SEG1 = 63;
            CANFDConfig.NBT_SEG2 = 16;
            CANFDConfig.NBT_SJW = 16;
            //数据域波特率参数,波特率=40M/DBT_BRP*(1+DBT_SEG1+DBT_SEG2)
            CANFDConfig.DBT_BRP = 1;
            CANFDConfig.DBT_SEG1 = 15;
            CANFDConfig.DBT_SEG2 = 4;
            CANFDConfig.DBT_SJW = 4;
            ret = USB2CANFD.CANFD_Init(DevHandle, CANIndex, ref CANFDConfig);
            if (ret != USB2CANFD.CANFD_SUCCESS)
            {
                Console.WriteLine("Config CANFD failed!");
                return;
            }
            else
            {
                Console.WriteLine("Config CANFD Success!");
            }
            //启动CAN数据接收
            ret = USB2CANFD.CANFD_StartGetMsg(DevHandle, CANIndex);
            if (ret != USB2CANFD.CANFD_SUCCESS)
            {
                Console.WriteLine("Start receive CANFD failed!");
                return;
            }
            else
            {
                Console.WriteLine("Start receive CANFD Success!");
            }
            //发送CAN数据
            USB2CANFD.CANFD_MSG[] CanMsg = new USB2CANFD.CANFD_MSG[5];
            IntPtr pCanSendMsg = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG))*CanMsg.Length);//申请缓冲区
            for (int i = 0; i < 5; i++)
            {
                CanMsg[i] = new USB2CANFD.CANFD_MSG();
                CanMsg[i].Flags = 0;//bit[0]-BRS,bit[1]-ESI,bit[2]-FDF,bit[6..5]-Channel,bit[7]-RXD
                CanMsg[i].DLC = 8;
                CanMsg[i].ID = (UInt32)i;
                CanMsg[i].Data = new Byte[64];
                for (int j = 0; j < CanMsg[i].DLC; j++)
                {
                    CanMsg[i].Data[j] = (Byte)j;
                }
                //将数组中的数据复制到数据缓冲区中
                IntPtr pPonitor = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG)));
                Marshal.StructureToPtr(CanMsg[i], pPonitor, true);
                byte[] buffer = new byte[Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG))];
                Marshal.Copy(pPonitor, buffer, 0, Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG)));
                Marshal.Copy(buffer, 0, (IntPtr)((UInt32)pCanSendMsg + i * Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG))), Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG)));
                Marshal.FreeHGlobal(pPonitor);//释放缓冲区
            }
            int SendedMsgNum = USB2CANFD.CANFD_SendMsg(DevHandle, CANIndex, pCanSendMsg, CanMsg.Length);
            if (SendedMsgNum >= 0)
            {
                Console.WriteLine("Success send frames:{0}", SendedMsgNum);
            }
            else
            {
                Console.WriteLine("Send CAN data failed!");
            }
            Marshal.FreeHGlobal(pCanSendMsg);//释放缓冲区
            //延时
            System.Threading.Thread.Sleep(100);
            //读取接收数据缓冲中的数据
            USB2CANFD.CANFD_MSG[] CanMsgBuffer = new USB2CANFD.CANFD_MSG[10240];
            for (int i = 0; i < CanMsgBuffer.Length; i++)
            {
                CanMsgBuffer[i] = new USB2CANFD.CANFD_MSG();
                CanMsgBuffer[i].Data = new Byte[64];
            }
            //将缓冲区转换成指针
            IntPtr pCanGetMsg = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG)) * CanMsgBuffer.Length);//申请缓冲区
            int GetMsgNum = USB2CANFD.CANFD_GetMsg(DevHandle, CANIndex, pCanGetMsg, CanMsgBuffer.Length);
            if (GetMsgNum > 0)
            {
                for (int i = 0; i < GetMsgNum; i++)
                {
                    CanMsgBuffer[i] = (USB2CANFD.CANFD_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pCanGetMsg + i * Marshal.SizeOf(typeof(USB2CANFD.CANFD_MSG))), typeof(USB2CANFD.CANFD_MSG));
                    Console.WriteLine("CanMsg[{0}].ID = {1}", i, CanMsgBuffer[i].ID & USB2CANFD.CANFD_MSG_FLAG_ID_MASK);
                    Console.WriteLine("CanMsg[{0}].TimeStamp = {1}", i, CanMsgBuffer[i].TimeStamp);
                    Console.Write("CanMsg[{0}].Data = ", i);
                    for (int j = 0; j < CanMsgBuffer[i].DLC; j++)
                    {
                        Console.Write("{0:X2} ", CanMsgBuffer[i].Data[j]);
                    }
                    Console.WriteLine("\n");
                }
            }
            else if (GetMsgNum < 0)
            {
                Console.WriteLine("Get CAN data error!");
            }
            Marshal.FreeHGlobal(pCanGetMsg);//释放缓冲区
            //停止接收数据
            ret = USB2CANFD.CANFD_StopGetMsg(DevHandle, CANIndex);
            if (ret != USB2CANFD.CANFD_SUCCESS)
            {
                Console.WriteLine("Stop receive CANFD failed!");
                return;
            }
            else
            {
                Console.WriteLine("Stop receive CANFD Success!");
            }
        }
    }
}
