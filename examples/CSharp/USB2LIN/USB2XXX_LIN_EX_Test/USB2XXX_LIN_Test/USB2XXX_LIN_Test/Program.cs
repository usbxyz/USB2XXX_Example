
#define LIN_MASTER_TEST
//#define LIN_SLAVE_TEST

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Diagnostics;
using System.Text;
using USB2XXX;
/**
 * 注意：若运行程序提示找不到USB2XXX.dll文件，可以将工程目录下的USB2XXX.dll和libusb-1.0.dll文件拷贝到exe程序输出目录下即可（一般是./bin/Release或者./bin/Debug目录）
 */
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
            Int32 DevNum, ret=0;
            String[] MSGTypeStr = new String[10]{"UN","MW","MR","SW","SR","BK","SY","ID","DT","CK"};
            String[] CKTypeStr = new String[5]{"STD","EXT","USER","NONE","ERROR"};
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
#if LIN_MASTER_TEST
            ret = USB2LIN_EX.LIN_EX_Init(DevHandle, LINIndex, 19200, 1);
#else
            ret = USB2LIN_EX.LIN_EX_Init(DevHandle, LINIndex, 19200, 0);
#endif
            if (ret != USB2LIN_EX.LIN_EX_SUCCESS)
            {
                Console.WriteLine("Config LIN failed!");
                return;
            }
            else
            {
                Console.WriteLine("Config LIN Success!");
            }
#if LIN_MASTER_TEST
            //主机写数据
            USB2LIN_EX.LIN_EX_MSG[] LINMsg = new USB2LIN_EX.LIN_EX_MSG[5];
            USB2LIN_EX.LIN_EX_MSG[] LINOutMsg = new USB2LIN_EX.LIN_EX_MSG[10];
            LINMsg[0] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[0].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_BK;//只发送BREAK信号，一般用于唤醒休眠中的从设备
            LINMsg[0].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
            for(int f=1;f<LINMsg.Length;f++){
                LINMsg[f] = new USB2LIN_EX.LIN_EX_MSG();
                LINMsg[f].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MW;//主机发送数据
                LINMsg[f].DataLen = 8;
                LINMsg[f].Data = new Byte[8];
                for(int i=0;i<LINMsg[1].DataLen;i++){
                    LINMsg[f].Data[i] = (Byte)((f<<4)|i);
                }
                LINMsg[f].Timestamp = 10;//发送该帧数据之后的延时时间
                LINMsg[f].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//增强校验
                LINMsg[f].PID = (Byte)(f + 1);
            }
            //将数组转换成指针
            IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) * LINOutMsg.Length);

            ret = USB2LIN_EX.LIN_EX_MasterSync(DevHandle, LINIndex, LINMsg, pt, LINMsg.Length);
            if (ret < USB2LIN_EX.LIN_EX_SUCCESS)
            {
                Console.WriteLine("MasterSync LIN failed!");
                return;
            }else{
                //主机发送数据成功后，也会接收到发送出去的数据，通过接收回来的数据跟发送出去的数据对比，可以判断发送数据的时候，数据是否被冲突
                Console.WriteLine("MsgLen = {0}", ret);
                for(int i=0;i<ret;i++){
                    LINOutMsg[i] = (USB2LIN_EX.LIN_EX_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + i * Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG))), typeof(USB2LIN_EX.LIN_EX_MSG));
                    Console.Write("{0} SYNC[{1:X2}] PID[{2:X2}] ", MSGTypeStr[LINOutMsg[i].MsgType], LINOutMsg[i].Sync, LINOutMsg[i].PID);
                    for(int j=0;j<LINOutMsg[i].DataLen;j++){
                        Console.Write("{0:X2} ", LINOutMsg[i].Data[j]);
                    }
                    Console.WriteLine("[{0}][{1:X2}] [{2}:{3}:{4}.{5}]", CKTypeStr[LINOutMsg[i].CheckType], LINOutMsg[i].Check, (LINOutMsg[i].Timestamp / 36000000) % 60, (LINOutMsg[i].Timestamp / 600000) % 60, (LINOutMsg[i].Timestamp / 10000) % 60, (LINOutMsg[i].Timestamp / 10) % 1000);
                }
            }
            //释放内存
            Marshal.FreeHGlobal(pt);
#else
            Console.WriteLine("Start Get LIN Data...");
            //设置ID为LIN_EX_MSG_TYPE_SW模式，这样主机就可以读取到数据
            USB2LIN_EX.LIN_EX_MSG[] LINSlaveMsg = new USB2LIN_EX.LIN_EX_MSG[10];
            for(int i=0;i<10;i++){
                LINSlaveMsg[i] = new USB2LIN_EX.LIN_EX_MSG();
                LINSlaveMsg[i].Data = new Byte[8];
                LINSlaveMsg[i].PID = (Byte)i;
                LINSlaveMsg[i].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;
                LINSlaveMsg[i].DataLen = 7;
                for(int j=0;j<LINSlaveMsg[i].DataLen;j++){
                    LINSlaveMsg[i].Data[j]=(Byte)((i<<4)|j);
                }
                LINSlaveMsg[i].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_SW;//从机发送数据模式
            }
            //设置从机模式下所有ID都为从接收数据模式，这样就可以获取到主机发送过来的所有数据,初始化配置为从机后，默认所有ID都为接收数据模式，所以若是监听LIN总线数据，这个函数可以不用调用
            ret = USB2LIN_EX.LIN_EX_SlaveSetIDMode(DevHandle,LINIndex,LINSlaveMsg,10);
            if(ret != USB2LIN_EX.LIN_EX_SUCCESS){
                Console.WriteLine("Config LIN ID Mode failed!");
                return;
            }else{
                Console.WriteLine("Config LIN ID Mode Success!");
            }
            //循环获取接收到的数据，该操作可以用作LIN总线数据监控
            Console.WriteLine("Start get data...");
            while(true)
            {
                USB2LIN_EX.LIN_EX_MSG[] LINMsg = new USB2LIN_EX.LIN_EX_MSG[1024];//缓冲区尽量大一点，防止益处
                //将数组转换成指针
                IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) * LINMsg.Length);

                ret = USB2LIN_EX.LIN_EX_SlaveGetData(DevHandle, LINIndex, pt);
                for(int i=0;i<ret;i++){
                    LINMsg[i] = (USB2LIN_EX.LIN_EX_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + i * Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG))), typeof(USB2LIN_EX.LIN_EX_MSG));
                    Console.WriteLine("{0} SYNC[{1:X2}] PID[{2:X2}] ", MSGTypeStr[LINMsg[i].MsgType], LINMsg[i].Sync, LINMsg[i].PID);
                    for(int j=0;j<LINMsg[i].DataLen;j++){
                        Console.Write("{0:X2} ", LINMsg[i].Data[j]);
                    }
                    Console.WriteLine("[{0}][{1:X2}] [{2}]:{3}]:{4}].{5}]]\n", CKTypeStr[LINMsg[i].CheckType], LINMsg[i].Check, (LINMsg[i].Timestamp / 36000000) % 60, (LINMsg[i].Timestamp / 600000) % 60, (LINMsg[i].Timestamp / 10000) % 60, (LINMsg[i].Timestamp / 10) % 1000);
                }
                System.Threading.Thread.Sleep(100);
            }
            //释放内存
            Marshal.FreeHGlobal(pt);
#endif
            Console.WriteLine("Close Device!");
            //关闭设备
            usb_device.USB_CloseDevice(DevHandle);
            return;
        }
    }
}
