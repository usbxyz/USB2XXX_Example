using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using USB2XXX;

namespace USB2XXXUARTTest
{
    class Program
    {
        static private System.Object lockThis = new System.Object();
        public class DevArg
        {
            public Int32 DevHandle;
            public Int32 UARTIndex;
            public Int32 DevIndex;
        }
        static private bool[] RunFlag = new bool[20];

        static void Main(string[] args)
        {
            Int32[] DevHandles = new Int32[20];
            Int32 ret;
            Console.WriteLine("扫描设备...");
            int DevCnt = usb_device.USB_ScanDevice(DevHandles);//获取设备连接数，同时获取每个设备的序号
            if (DevCnt <= 0)
            {
                Console.WriteLine("获取设备列表失败！当前可能无设备连接！");
                return;
            }
            else
            {
                Console.Write("扫描到{0}个设备,每个设备的序号分别为：", DevCnt);
                for (int i = 0; i < DevCnt; i++)
                {
                    Console.Write(DevHandles[i].ToString("X8") + "  ");
                }
                Console.WriteLine("");
                for (int i = 0; i < DevCnt; i++)
                {
                    //打开设备
                    bool state = usb_device.USB_OpenDevice(DevHandles[i]);
                    if (!state)
                    {
                        Console.WriteLine("Open device error!" + DevHandles[i].ToString("X8"));
                    }
                }
            }
            //配置UART总线相关参数
            USB2UART.UART_CONFIG UARTConfig = new USB2UART.UART_CONFIG();
            UARTConfig.BaudRate = 115200;
            UARTConfig.Parity = USB2UART.UART_PARITY_NO;
            UARTConfig.StopBits = USB2UART.UART_STOP_BITS_1;
            UARTConfig.WordLength = USB2UART.UART_WORD_LENGTH_8BIT;
            for (int i = 0; i < DevCnt; i++)
            {
                ret = USB2UART.UART_Init(DevHandles[i], 0, ref UARTConfig);
                if (ret != USB2UART.UART_SUCCESS)
                {
                    Console.WriteLine("Initialize device error!\n");
                    return;
                }
            }
            //启动接收线程
            Thread[] t = new Thread[DevCnt];
            for (int j = 0; j < DevCnt; j++)
            {
                RunFlag[j] = true;
                t[j] = new Thread(new ParameterizedThreadStart(UARTReadDataThread));
                t[j].IsBackground = true;
                t[j].Start(new DevArg { DevHandle = DevHandles[j], UARTIndex = 0, DevIndex = j});
            }
            //发送数据
            Byte[] WriteBuffer = new Byte[1024];
            for (int i = 0; i < WriteBuffer.Length; i++)
            {
                WriteBuffer[i] = (Byte)i;
            }
            for (int j = 0; j < DevCnt; j++)
            {
                lock (lockThis)
                {
                    ret = USB2UART.UART_WriteBytes(DevHandles[j], 0, WriteBuffer, WriteBuffer.Length);
                }
                if (ret != USB2UART.UART_SUCCESS)
                {
                    Console.WriteLine("Write data error!\n");
                }
            }
            //输入任意字符结束数据接收
            Console.ReadLine();
            for (int j = 0; j < DevCnt; j++)
            {
                RunFlag[j] = false;
            }
            //等待线程自动结束
            for (int j = 0; j < DevCnt; j++)
            {
                t[j].Join();
            }
            //关闭设备
            for (int j = 0; j < DevCnt; j++)
            {
                usb_device.USB_CloseDevice(DevHandles[j]);
            }
        }
        //读数据线程
        public static void UARTReadDataThread(object arg)
        {
            int ret;
            DevArg devArg = arg as DevArg;
            Byte UARTIndex = (Byte)devArg.UARTIndex;
            int DevIndex = devArg.DevIndex;
            int DevHandle = devArg.DevHandle;
            byte[] ReadBuffer = new byte[20 * 1024];
            while (RunFlag[DevIndex])
            {
                lock (lockThis)
                {
                    ret = USB2UART.UART_ReadBytes(DevHandle, UARTIndex, ReadBuffer, 100);
                }
                if (ret < 0)
                {
                    return;
                }
                else if (ret > 0)
                {
                    Console.WriteLine("[{0}]Read Data:", DevHandle.ToString("X8"));
                    for (int i = 0; i < ret; i++)
                    {
                        Console.Write(ReadBuffer[i].ToString("X2") + " ");
                        if (((i + 1) % 16) == 0)
                        {
                            Console.WriteLine("");
                        }
                    }
                    Console.WriteLine("");
                }
                System.Threading.Thread.Sleep(10);
            }
        }
    }
}
