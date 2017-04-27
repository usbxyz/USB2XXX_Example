using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using USB2XXX;

namespace USB2XXXW25Q40
{
    class Program
    {
        static void Main(string[] args)
        {
            Int32[] DevHandles = new Int32[20];
            Console.WriteLine("扫描设备...");
            int DevCnt = usb_device.USB_ScanDevice(DevHandles);//获取设备连接数，同时获取每个设备的序号
            if (DevCnt <= 0)
            {
                Console.WriteLine("获取设备列表失败！当前可能无设备连接！");
                return;
            }
            else
            {
                Console.WriteLine("扫描到{0}个设备,每个设备的序号分别为：",DevCnt);
                for (int i = 0; i < DevCnt; i++)
                {
                    Console.Write(DevHandles[i].ToString("X8") + "  ");
                }
                Console.WriteLine("");
            }
            //循环打开所有设备
            Console.WriteLine("循环打开所有设备...");
            for (int i = 0; i < DevCnt; i++)
            {
                //打开设备
                bool state = usb_device.USB_OpenDevice(DevHandles[i]);
                if (!state)
                {
                    Console.WriteLine("Open device({0}) error!", DevHandles[i]);
                }
            }
            
            //启动自动烧写数据线程
            int TestTimes = 100000;
            while (TestTimes > 0)
            {
                TestTimes--;
                Thread[] t = new Thread[DevCnt];
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i] = new Thread(new ParameterizedThreadStart(SPIWriteReadThread));
                    t[i].IsBackground = true;
                    t[i].Start(DevHandles[i].ToString());
                }
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i].Join();
                }
                Console.WriteLine("TestTimes = {0}", TestTimes);
            }
        }
      
        public static void SPIWriteReadThread(object DevHandleStr)
        {
            USB2SPI.SPI_CONFIG SPIConfig = new USB2SPI.SPI_CONFIG();
            int ret;
            int SPIIndex = 0;//使用SPI1
            int DevHandle = Convert.ToInt32(DevHandleStr.ToString());
            //根据W25Q40配置USB2SPI适配器
            Console.WriteLine("{0}配置USB2SPI适配器...", DevHandle);
            SPIConfig.ClockSpeedHz = 25000000>>2;
            SPIConfig.CPHA = 0;
            SPIConfig.CPOL = 0;
            SPIConfig.LSBFirst = USB2SPI.SPI_MSB;
            SPIConfig.Master = USB2SPI.SPI_MASTER;
            SPIConfig.Mode = USB2SPI.SPI_MODE_HARD_HDX;
            SPIConfig.SelPolarity = USB2SPI.SPI_SEL_LOW;
            ret = USB2SPI.SPI_Init(DevHandle, SPIIndex, ref SPIConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}初始化设备错误!", DevHandle);
                return;
            }
            //写数据
            Console.WriteLine("{0}USB2SPI适配器写数据...", DevHandle);
            Byte[] WriteData = new Byte[10240];
            ret = USB2SPI.SPI_WriteBytes(DevHandle, SPIIndex, WriteData, WriteData.Length);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}写数据出错！", SPIIndex);
                while (true) ; 
            }
            //读数据
            Console.WriteLine("{0}USB2SPI适配器读数据...", DevHandle);
            Byte[] ReadData = new Byte[10240];
            ret = USB2SPI.SPI_ReadBytes(DevHandle, SPIIndex, ReadData, ReadData.Length);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}读数据出错！", SPIIndex);
                while (true) ; 
            }
        }
    }
}
