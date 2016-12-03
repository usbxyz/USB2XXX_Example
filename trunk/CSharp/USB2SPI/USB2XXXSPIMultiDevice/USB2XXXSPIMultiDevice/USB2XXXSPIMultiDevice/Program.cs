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
            int[] DevSns = new int[10];
            Console.WriteLine("扫描设备...");
            int DevCnt = GetDevSns(DevSns);//获取设备连接数，同时获取每个设备的序号
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
                    Console.Write(DevSns[i].ToString("X8") + "  ");
                }
                Console.WriteLine("");
            }
            //根据设备序号获取设备索引号
            
            Console.WriteLine("根据设备序号获取设备索引号...");
            int[] DevIndex = new int[DevCnt];
            for (int i = 0; i < DevCnt; i++)
            {
                
                DevIndex[i] = GetDevIndex(DevSns[i]);
                if (DevIndex[i] < 0)
                {
                    Console.WriteLine("获取设备索引号失败！当前可能无设备连接或者当前序号的设备未连接！");
                    return;
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
                    t[i].Start(i.ToString());
                }
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i].Join();
                }
                Console.WriteLine("TestTimes = {0}", TestTimes);
            }
        }
        //获取设备序号列表
        static int GetDevSns(int[] DevSns)
        {
            bool state;
            int DevCnt = usb_device.USB_ScanDevice(null);
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            for (int i = 0; i < DevCnt; i++)
            {
                //打开设备
                state = usb_device.USB_OpenDevice(i);
                if (!state)
                {
                    Console.WriteLine("Open device error!");
                    return -1;
                }
                //获取固件信息
                state = usb_device.USB_GetDeviceInfo(i, ref DevInfo, null);
                if (!state)
                {
                    Console.WriteLine("Get device infomation error!");
                    return -1;
                }
                DevSns[i] = (int)DevInfo.SerialNumber[2];
            }
            return DevCnt;
        }
        //根据设备序号获取设备索引号
        static int GetDevIndex(int DevSn)
        {
            bool state;
            int DevCnt = usb_device.USB_ScanDevice(null);
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            for (int i = 0; i < DevCnt; i++)
            {
                //打开设备
                state = usb_device.USB_OpenDevice(i);
                if (!state)
                {
                    Console.WriteLine("Open device error!");
                    return -1;
                }
                //获取固件信息
                state = usb_device.USB_GetDeviceInfo(i, ref DevInfo, null);
                if (!state)
                {
                    Console.WriteLine("Get device infomation error!");
                    return -1;
                }
                if (DevSn == DevInfo.SerialNumber[2])
                {
                    return i;
                }
            }
            return -1;
        }
  
        public static void SPIWriteReadThread(object Index)
        {
            USB2SPI.SPI_CONFIG SPIConfig = new USB2SPI.SPI_CONFIG();
            int ret;
            int SPIIndex = 0;//使用SPI1
            int DevIndex = Convert.ToInt32(Index.ToString());
            //根据W25Q40配置USB2SPI适配器
            Console.WriteLine("{0}配置USB2SPI适配器...", DevIndex);
            SPIConfig.ClockSpeedHz = 25000000>>2;
            SPIConfig.CPHA = 0;
            SPIConfig.CPOL = 0;
            SPIConfig.LSBFirst = USB2SPI.SPI_MSB;
            SPIConfig.Master = USB2SPI.SPI_MASTER;
            SPIConfig.Mode = USB2SPI.SPI_MODE_HARD_HDX;
            SPIConfig.SelPolarity = USB2SPI.SPI_SEL_LOW;
            ret = USB2SPI.SPI_Init(DevIndex, SPIIndex, ref SPIConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}初始化设备错误!", DevIndex);
                return;
            }
            //写数据
            Console.WriteLine("{0}USB2SPI适配器写数据...", DevIndex);
            Byte[] WriteData = new Byte[10240];
            ret = USB2SPI.SPI_WriteBytes(DevIndex, SPIIndex, WriteData, WriteData.Length);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}写数据出错！", SPIIndex);
                while (true) ; 
            }
            //读数据
            Console.WriteLine("{0}USB2SPI适配器读数据...", DevIndex);
            Byte[] ReadData = new Byte[10240];
            ret = USB2SPI.SPI_ReadBytes(DevIndex, SPIIndex, ReadData, ReadData.Length);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}读数据出错！", SPIIndex);
                while (true) ; 
            }
        }
    }
}
