using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;


namespace USB2XXXSPITest
{
    class Program
    {
        public static Int32 SPI_SlaveReadDataHandle(Int32 DevIndex, Int32 SPIIndex, Byte[] pData, Int32 DataNum)
        {
            Console.WriteLine("Slave Read Data Count: " + DataNum.ToString());
            return 0;
        }
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            USB2SPI.SPI_CONFIG SPIConfig = new USB2SPI.SPI_CONFIG();
            Int32 DevIndex = 0;
            Int32 SPIIndex = 0;
            bool state;
            Int32 DevNum, ret;
            Byte[] WriteBuffer = new Byte[64];
            Byte[] ReadBuffer = new Byte[20480];
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

            //配置SPI总线相关参数
            Console.Write("Please input SPI channel(0 or 1):");
            SPIIndex = Convert.ToInt32(Console.ReadLine());
            Console.Write("Please input CPHA(0 or 1):");
            SPIConfig.CPHA = Convert.ToByte(Console.ReadLine());
            Console.Write("Please input CPOL(0 or 1):");
            SPIConfig.CPOL = Convert.ToByte(Console.ReadLine());
            Console.WriteLine("SPIConfig.CPHA = " + SPIConfig.CPHA.ToString());
            Console.WriteLine("SPIConfig.CPOL = " + SPIConfig.CPOL.ToString());
            //配置SPI总线相关参数(配置为从机模式)
            SPIConfig.Mode = USB2SPI.SPI_MODE_HARD_FDX;
            SPIConfig.ClockSpeedHz = 50000000;
            //SPIConfig.CPHA = 0;
            //SPIConfig.CPOL = 1;
            SPIConfig.LSBFirst = USB2SPI.SPI_MSB;
            SPIConfig.Master = USB2SPI.SPI_MASTER;
            SPIConfig.SelPolarity = USB2SPI.SPI_SEL_LOW;
            ret = USB2SPI.SPI_Init(DevIndex, SPIIndex, ref SPIConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("Initialize device error!");
                return;
            }
            for (int i = 0; i < WriteBuffer.Length; i++)
            {
                WriteBuffer[i] = (Byte)i;
            }
            /*
            while(true){
                ret = USB2SPI.SPI_SlaveWriteBytes(DevIndex, SPIIndex, WriteBuffer, 16, 5000);
                if (ret != USB2SPI.SPI_SUCCESS)
                {
                    Console.WriteLine("SPI slave write data error!");
                    return;
                }
                Console.ReadLine();
            }
            */
            Console.WriteLine("Start receive data to file,the file name is data.txt");
            Console.WriteLine("Press any key to exit the data reception!");
            USB2SPI.SPI_SlaveContinueRead(DevIndex, SPIIndex, SPI_SlaveReadDataHandle);
            Console.ReadLine();
            USB2SPI.SPI_SlaveContinueReadStop(DevIndex, SPIIndex);
            Console.WriteLine("Test SPI_SUCCESS!");
            return;
        }
    }
}
