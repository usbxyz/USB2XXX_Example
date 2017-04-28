using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;


namespace USB2XXXSPITest
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            USB2SPI.SPI_CONFIG SPIConfig = new USB2SPI.SPI_CONFIG();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Int32 SPIIndex = 0;
            bool state;
            Int32 DevNum,ret;
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
                Console.WriteLine("    Build Date:"+Encoding.Default.GetString(DevInfo.BuildDate));
                Console.WriteLine("    Firmware Version:v{0}.{1}.{2}", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF);
                Console.WriteLine("    Hardware Version:v{0}.{1}.{2}", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF);
                Console.WriteLine("    Functions:"+ DevInfo.Functions.ToString("X8"));
                Console.WriteLine("    Functions String:"+ FuncStr);
                Console.WriteLine("    Serial Number:" + DevInfo.SerialNumber[0].ToString("X8") + DevInfo.SerialNumber[1].ToString("X8") + DevInfo.SerialNumber[2].ToString("X8"));
            }

            //配置SPI总线相关参数
            SPIConfig.Mode = USB2SPI.SPI_MODE_HARD_HDX;
            SPIConfig.ClockSpeedHz = 50000000;
            SPIConfig.CPHA = 0;
            SPIConfig.CPOL = 0;
            SPIConfig.LSBFirst = USB2SPI.SPI_MSB;
            SPIConfig.Master = USB2SPI.SPI_MASTER;
            SPIConfig.SelPolarity = USB2SPI.SPI_SEL_LOW;
            ret = USB2SPI.SPI_Init(DevHandle, SPIIndex, ref SPIConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("Initialize device error!");
                return;
            }
            //SPI发送数据
            for(int i=0;i<WriteBuffer.Length;i++){
                WriteBuffer[i] = (Byte)i;
            }
            ret = USB2SPI.SPI_WriteBytes(DevHandle, SPIIndex, WriteBuffer, WriteBuffer.Length);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI write data error!");
                return;
            }
            //SPI异步发送数据，调用该函数后会立即返回，但是SPI数据不一定发送完毕，但是在下一次发送数据之前会保证数据发送完毕
            for(int i=0;i<64;i++){
                ret = USB2SPI.SPI_AsyncWriteBytes(DevHandle, SPIIndex, WriteBuffer, WriteBuffer.Length);
                if (ret != USB2SPI.SPI_SUCCESS)
                {
                    Console.WriteLine("SPI async write data error!");
                    return;
                }
            }
            //SPI接收数据
            ret = USB2SPI.SPI_ReadBytes(DevHandle, SPIIndex, ReadBuffer, 32);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI read data error!");
                return;
            }else{
                Console.WriteLine("Read Data:");
                for(int i=0;i<32;i++){
                    Console.Write(ReadBuffer[i].ToString("X2")+" ");
                    if(((i+1)%16)==0){
                        Console.WriteLine("");
                    }
                }
                Console.WriteLine("");
            }
            //SPI先发送数据，再接收数据，整个过程片选信号一直有效
            int IntervalTime = 10;//发送和接收数据之间的时间间隔，单位为us
            ret = USB2SPI.SPI_WriteReadBytes(DevHandle, SPIIndex, WriteBuffer, WriteBuffer.Length, ReadBuffer, 32, IntervalTime);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI write read data error!");
                return;
            }else{
                Console.WriteLine("Read Data:");
                for(int i=0;i<32;i++){
                    Console.Write(ReadBuffer[i].ToString("X2")+" ");
                    if(((i+1)%16)==0){
                        Console.WriteLine("");
                    }
                }
                Console.WriteLine("");
            }
            
            Console.WriteLine("Test SPI_SUCCESS!");
            return;
        }
    }
}
