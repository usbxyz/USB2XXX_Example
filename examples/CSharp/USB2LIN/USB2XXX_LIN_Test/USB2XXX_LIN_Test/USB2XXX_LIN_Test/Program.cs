#define LIN_MASTER

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Diagnostics;
using System.Text;
using USB2XXX;

namespace USB2XXX_LIN_Test
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Byte LINIndex = 0;
            bool state;
            Int32 DevNum, ret;
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
            //初始化配置LIN
            USB2LIN.LIN_CONFIG LINConfig = new USB2LIN.LIN_CONFIG();
            LINConfig.BaudRate = 19200;
            LINConfig.BreakBits = USB2LIN.LIN_BREAK_BITS_11;
            LINConfig.CheckMode = USB2LIN.LIN_CHECK_MODE_EXT;
#if LIN_MASTER
            LINConfig.MasterMode = USB2LIN.LIN_MASTER;
#else
            LINConfig.MasterMode = USB2LIN.LIN_SLAVE;
#endif
            ret = USB2LIN.LIN_Init(DevHandle, LINIndex, ref LINConfig);
            if (ret != USB2LIN.LIN_SUCCESS)
            {
                Console.WriteLine("Config LIN failed!");
                return;
            }
            else
            {
                Console.WriteLine("Config LIN Success!");
            }
#if LIN_MASTER
            //主机写数据
            for (Byte ID = 0; ID < 10; ID++)
            {
                USB2LIN.LIN_MSG[] msg = new USB2LIN.LIN_MSG[1];
                msg[0].Data = new Byte[9];
                for (Byte i = 0; i < 8; i++)
                {
                    msg[0].Data[i] = (Byte)(ID + i);
                }
                msg[0].DataLen = 8;
                msg[0].ID = ID;
                ret = USB2LIN.LIN_Write(DevHandle, LINIndex, msg, 1);
                if (ret != USB2LIN.LIN_SUCCESS)
                {
                    Console.WriteLine("LIN write data failed!\n");
                    return;
                }
                else
                {
                    Console.WriteLine("LIN write data success!\n");
                }
                //延时
                System.Threading.Thread.Sleep(10);
            }
            //主机读数据
            for (Byte ID = 0; ID < 10; ID++)
            {
                USB2LIN.LIN_MSG[] msg = new USB2LIN.LIN_MSG[1];
                msg[0].Data = new Byte[9];
                msg[0].ID = ID;

                IntPtr[] ptArray = new IntPtr[1];
                ptArray[0] = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN.LIN_MSG)) * msg.Length);
                IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN.LIN_MSG)));
                Marshal.Copy(ptArray, 0, pt, 1);
                //将数组中的数据拷贝到指针所指区域
                for (int k = 0; k < msg.Length; k++)
                {
                    Marshal.StructureToPtr(msg[k], (IntPtr)((UInt32)pt + k * Marshal.SizeOf(typeof(USB2LIN.LIN_MSG))), true);
                }

                ret = USB2LIN.LIN_Read(DevHandle, LINIndex, pt, 1);
                if (ret < USB2LIN.LIN_SUCCESS)
                {
                    Console.WriteLine("LIN read data failed!\n");
                    return;
                }
                else
                {
                    msg[0] = (USB2LIN.LIN_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + 0 * Marshal.SizeOf(typeof(USB2LIN.LIN_MSG))), typeof(USB2LIN.LIN_MSG));
                    Console.Write("Master LIN Read: ID = 0x{0:X2} Data = ", msg[0].ID);
                    for (int i = 0; i < msg[0].DataLen; i++)
                    {
                        Console.Write("0x{0:X2} ", msg[0].Data[i]);
                    }
                    Console.WriteLine("");
                }
            }
#else
            //设置从机模式下所有ID都为从接收数据模式，这样就可以获取到主机发送过来的所有数据,初始化配置为从机后，默认所有ID都为接收数据模式，所以若是监听LIN总线数据，这个函数可以不用调用
            USB2LIN.LIN_MSG[] LINSlaveData = new USB2LIN.LIN_MSG[64];
	        for(Byte i=0;i<64;i++){
                LINSlaveData[i] = new USB2LIN.LIN_MSG();
		        LINSlaveData[i].DataLen = 9;//最大8Byte数据+1Byte和校验
		        LINSlaveData[i].ID = i;		//ID值
                LINSlaveData[i].Data = new Byte[9];
	        }
            ret = USB2LIN.LIN_SlaveSetIDMode(DevHandle, LINIndex, USB2LIN.LIN_SLAVE_READ, LINSlaveData, 64);
            if (ret != USB2LIN.LIN_SUCCESS)
            {
                Console.WriteLine("Set LIN operation mode failed!");
                return;
            }else{
                Console.WriteLine("Set LIN operation mode success!");
            }
            //从机接收数据,若要连续不断的监控LIN总线数据，可以循环调用LIN_SlaveGetData函数
            USB2LIN.LIN_MSG[] LINSlaveDataBuffer = new USB2LIN.LIN_MSG[1024];//为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
            for (int i = 0; i < LINSlaveDataBuffer.Length; i++)
            {
                LINSlaveDataBuffer[i] = new USB2LIN.LIN_MSG();
                LINSlaveDataBuffer[i].Data = new Byte[9];
            }
            while (true)
            {
                IntPtr[] ptArray = new IntPtr[2];
                ptArray[0] = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN.LIN_MSG)) * LINSlaveDataBuffer.Length);
                IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN.LIN_MSG)));
                Marshal.Copy(ptArray, 0, pt, 1);
                ret = USB2LIN.LIN_SlaveGetData(DevHandle, LINIndex, pt);
                if (ret < USB2LIN.LIN_SUCCESS)
                {
                    Console.WriteLine("LIN slave read data error!");
                    return;
                }
                else if (ret == 0)
                {
                    //Console.WriteLine("LIN slave read no data!");
                }
                else
                {
                    Console.WriteLine("LIN slave read data:");
                    for (int i = 0; i < ret; i++)
                    {
                        LINSlaveDataBuffer[i] = (USB2LIN.LIN_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + i * Marshal.SizeOf(typeof(USB2LIN.LIN_MSG))), typeof(USB2LIN.LIN_MSG));
                        Console.Write("Data[{0}]: ", i);
                        Console.Write("ID = 0x{0:X2} ", LINSlaveDataBuffer[i].ID);
                        Console.Write("Data = ");
                        for (int j = 0; j < LINSlaveDataBuffer[i].DataLen; j++)
                        {
                            Console.Write("0x{0:X2} ", LINSlaveDataBuffer[i].Data[j]);
                        }
                        Console.WriteLine("");
                    }
                }
                //延时
                System.Threading.Thread.Sleep(10);
            }
#endif

        }
    }
}
