
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
            /************************************主机写数据************************************/
            int MsgIndex = 0;
            Byte[] DataBuffer;
            USB2LIN_EX.LIN_EX_MSG[] LINMsg = new USB2LIN_EX.LIN_EX_MSG[5];
            USB2LIN_EX.LIN_EX_MSG[] LINOutMsg = new USB2LIN_EX.LIN_EX_MSG[10];
            //添加第一帧数据
            LINMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_BK;//只发送BREAK信号，一般用于唤醒休眠中的从设备
            LINMsg[MsgIndex].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
            MsgIndex++;
            //添加第二帧数据
            LINMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MW;//主机发送数据
            LINMsg[MsgIndex].DataLen = 8;//实际要发送的数据字节数
            LINMsg[MsgIndex].Timestamp = 10;//发送该帧数据之后的延时时间
            LINMsg[MsgIndex].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//增强校验
            LINMsg[MsgIndex].PID = 0x32;//可以只传入ID，校验位底层会自动计算
            LINMsg[MsgIndex].Data = new Byte[8];//必须分配8字节空间
            DataBuffer = new Byte[8] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };//根据自己实际情况修改数据
            for (int i = 0; i < LINMsg[MsgIndex].DataLen; i++)//循环填充8字节数据
            {
                LINMsg[MsgIndex].Data[i] = DataBuffer[i];
            }
            MsgIndex++;
            //添加第三帧数据
            LINMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MW;//主机发送数据
            LINMsg[MsgIndex].DataLen = 8;//实际要发送的数据字节数
            LINMsg[MsgIndex].Timestamp = 10;//发送该帧数据之后的延时时间
            LINMsg[MsgIndex].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//增强校验
            LINMsg[MsgIndex].PID = 0x32;//可以只传入ID，校验位底层会自动计算
            LINMsg[MsgIndex].Data = new Byte[8];//必须分配8字节空间
            DataBuffer = new Byte[8] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };//根据自己实际情况修改数据
            for (int i = 0; i < LINMsg[MsgIndex].DataLen; i++)//循环填充8字节数据
            {
                LINMsg[MsgIndex].Data[i] = DataBuffer[i];
            }
            MsgIndex++;
            /********************需要发送更多帧数据，请按照前面的方式继续添加********************/
            //Console.WriteLine("Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) = {0}", Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)));
            //将数组转换成指针
            IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) * MsgIndex);
            ret = USB2LIN_EX.LIN_EX_MasterSync(DevHandle, LINIndex, LINMsg, pt, MsgIndex);
            if (ret < USB2LIN_EX.LIN_EX_SUCCESS)
            {
                Console.WriteLine("MasterSync LIN failed!");
                //释放内存
                Marshal.FreeHGlobal(pt);
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
            /************************************主机读数据************************************/
            MsgIndex = 0;
            //添加第一帧数据
            LINMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_BK;//只发送BREAK信号，一般用于唤醒休眠中的从设备
            LINMsg[MsgIndex].Timestamp = 10;//发送该帧数据之后的延时时间，最小建议设置为1
            MsgIndex++;
            //添加第二帧数据
            LINMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MR;//主机读数据
            LINMsg[MsgIndex].Timestamp = 10;//发送该帧数据之后的延时时间
            LINMsg[MsgIndex].PID = 0x33;//可以只传入ID，校验位底层会自动计算
            MsgIndex++;
            //添加第三帧数据
            LINMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MR;//主机读数据
            LINMsg[MsgIndex].Timestamp = 10;//发送该帧数据之后的延时时间
            LINMsg[MsgIndex].PID = 0x33;//可以只传入ID，校验位底层会自动计算
            MsgIndex++;
            /********************需要发送更多帧数据，请按照前面的方式继续添加********************/
            //将数组转换成指针
            pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) * MsgIndex);
            ret = USB2LIN_EX.LIN_EX_MasterSync(DevHandle, LINIndex, LINMsg, pt, MsgIndex);
            if (ret < USB2LIN_EX.LIN_EX_SUCCESS)
            {
                Console.WriteLine("MasterSync LIN failed!");
                //释放内存
                Marshal.FreeHGlobal(pt);
                return;
            }
            else
            {
                //主机发送数据成功后，也会接收到发送出去的数据，通过接收回来的数据跟发送出去的数据对比，可以判断发送数据的时候，数据是否被冲突
                Console.WriteLine("MsgLen = {0}", ret);
                for (int i = 0; i < ret; i++)
                {
                    LINOutMsg[i] = (USB2LIN_EX.LIN_EX_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + i * Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG))), typeof(USB2LIN_EX.LIN_EX_MSG));
                    Console.Write("{0} SYNC[{1:X2}] PID[{2:X2}] ", MSGTypeStr[LINOutMsg[i].MsgType], LINOutMsg[i].Sync, LINOutMsg[i].PID);
                    for (int j = 0; j < LINOutMsg[i].DataLen; j++)//实际读取到的字节数据是LINOutMsg[i].DataLen的值
                    {
                        Console.Write("{0:X2} ", LINOutMsg[i].Data[j]);
                    }
                    Console.WriteLine("[{0}][{1:X2}] [{2}:{3}:{4}.{5}]", CKTypeStr[LINOutMsg[i].CheckType], LINOutMsg[i].Check, (LINOutMsg[i].Timestamp / 36000000) % 60, (LINOutMsg[i].Timestamp / 600000) % 60, (LINOutMsg[i].Timestamp / 10000) % 60, (LINOutMsg[i].Timestamp / 10) % 1000);
                }
            }
            //释放内存
            Marshal.FreeHGlobal(pt);
#else
            /************************************从机发送数据************************************/
            int MsgIndex = 0;
            Byte[] DataBuffer;
            //设置ID为LIN_EX_MSG_TYPE_SW模式，从机接收到主机发送的帧头后就会返回预先定义好的数据
            USB2LIN_EX.LIN_EX_MSG[] LINSlaveMsg = new USB2LIN_EX.LIN_EX_MSG[3];
            //配置第一帧数据
            LINSlaveMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINSlaveMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_SW;//从机发送数据模式
            LINSlaveMsg[MsgIndex].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//配置为增强校验
            LINSlaveMsg[MsgIndex].PID = 0x34;//可以只传入ID，校验位底层会自动计算
            LINSlaveMsg[MsgIndex].Data = new Byte[8];//必须分配8字节空间
            LINSlaveMsg[MsgIndex].DataLen = 8;//实际要发送的数据字节数
            DataBuffer = new Byte[8] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };//从机返回的数据，根据自己实际情况修改数据
            for (int i = 0; i < LINSlaveMsg[MsgIndex].DataLen; i++)//循环填充8字节数据
            {
                LINSlaveMsg[MsgIndex].Data[i] = DataBuffer[i];
            }
            MsgIndex++;
            //配置第二帧数据
            LINSlaveMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINSlaveMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_SW;//从机发送数据模式
            LINSlaveMsg[MsgIndex].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//配置为增强校验
            LINSlaveMsg[MsgIndex].PID = 0x35;//可以只传入ID，校验位底层会自动计算
            LINSlaveMsg[MsgIndex].Data = new Byte[8];//必须分配8字节空间
            LINSlaveMsg[MsgIndex].DataLen = 8;//实际要发送的数据字节数
            DataBuffer = new Byte[8] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };//从机返回的数据，根据自己实际情况修改数据
            for (int i = 0; i < LINSlaveMsg[MsgIndex].DataLen; i++)//循环填充8字节数据
            {
                LINSlaveMsg[MsgIndex].Data[i] = DataBuffer[i];
            }
            MsgIndex++;
            //配置第三帧数据
            LINSlaveMsg[MsgIndex] = new USB2LIN_EX.LIN_EX_MSG();
            LINSlaveMsg[MsgIndex].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_SW;//从机发送数据模式
            LINSlaveMsg[MsgIndex].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//配置为增强校验
            LINSlaveMsg[MsgIndex].PID = 0x36;//可以只传入ID，校验位底层会自动计算
            LINSlaveMsg[MsgIndex].Data = new Byte[8];//必须分配8字节空间
            LINSlaveMsg[MsgIndex].DataLen = 8;//实际要发送的数据字节数
            DataBuffer = new Byte[8] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };//从机返回的数据，根据自己实际情况修改数据
            for (int i = 0; i < LINSlaveMsg[MsgIndex].DataLen; i++)//循环填充8字节数据
            {
                LINSlaveMsg[MsgIndex].Data[i] = DataBuffer[i];
            }
            MsgIndex++;
            /********************需要配置更多帧，请按照前面的方式继续添加********************/
            //默认所有ID都是从机接收数据模式，该模式下可以用于数据的监听
            //调用该函数后，只会修改传入对应ID的模式，其他的不会被改变
            ret = USB2LIN_EX.LIN_EX_SlaveSetIDMode(DevHandle, LINIndex, LINSlaveMsg, MsgIndex);
            if(ret != USB2LIN_EX.LIN_EX_SUCCESS){
                Console.WriteLine("Config LIN ID Mode failed!");
                return;
            }else{
                Console.WriteLine("Config LIN ID Mode Success!");
            }
            //循环获取接收到的数据，该操作可以用作LIN总线数据监控
            Console.WriteLine("Start Get LIN Data...");
            int RunTimeMs = 10000;//只运行10秒钟，10秒钟后结束数据监听，可自行修改
            while (RunTimeMs > 0)
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
                //释放内存
                Marshal.FreeHGlobal(pt);
                System.Threading.Thread.Sleep(10);
                RunTimeMs -= 10;
            }
#endif
            Console.WriteLine("Close Device!");
            //关闭设备
            usb_device.USB_CloseDevice(DevHandle);
            return;
        }
    }
}
