using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;

namespace USB2XXXIICADS1115
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            Int32 DevIndex = 0;
            Int32 IICIndex = 0;
            bool state;
            Int32 DevNum, ret;
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
            //初始化配置I2C
            USB2IIC.IIC_CONFIG IICConfig = new USB2IIC.IIC_CONFIG();
            IICConfig.AddrBits = 7;            //7bit地址模式
            IICConfig.ClockSpeedHz = 400000;     //时钟频率400KHz
            IICConfig.Master = 1;              //主机模式
            ret = USB2IIC.IIC_Init(DevIndex, IICIndex, ref IICConfig);
            if (ret != USB2IIC.IIC_SUCCESS)
            {
                Console.WriteLine("Config IIC failed!");
                return;
            }
            else
            {
                Console.WriteLine("Config IIC IIC_SUCCESS!");
            }
            //配置芯片
            UInt16 Config = (1 << 15)|    //Begin a single conversion(when in power-down mode)
                            (0 << 12)|    //AINP=AIN0 and AINN=AIN1(default)
                            (1 << 9)|     //FS=±4.096V
                            (0 << 8)|     //0:Continuous conversion mode 1:Power-down single-shot mode
                            (7 << 5)|     //860SPS
                            (0 << 4)|     //Traditional comparator with hysteresis
                            (0 << 3)|     //Active low
                            (1 << 2) |     //Latching comparator Non-latching comparator
                            (0 << 0);     //Assert after one conversion
            byte[] WriteDataBuffer = new byte[3];
            WriteDataBuffer[0] = 0x01;
            WriteDataBuffer[1] = (byte)(Config >> 8);
            WriteDataBuffer[2] = (byte)(Config & 0xFF);
            Int16 SlaveAddr = 0x48;
            //调用写数据函数
            ret = USB2IIC.IIC_WriteBytes(DevIndex, IICIndex, SlaveAddr, WriteDataBuffer, WriteDataBuffer.Length, 10);
            if (ret != USB2IIC.IIC_SUCCESS)
            {
                Console.WriteLine("Write IIC failed!");
                Console.WriteLine("Error Code:{0}", ret);
                return;
            }
            else
            {
                Console.WriteLine("Config ADS1115 success!");
            }
            //循环读取数据
            int ReadDataNum = 10;
            byte[] ReadDataBuffer = new byte[2];
            for (int i = 0; i < ReadDataNum; i++)
            {
                WriteDataBuffer[0] = 0x00;
                ret = USB2IIC.IIC_WriteReadBytesOfEvent(DevIndex, IICIndex, SlaveAddr, WriteDataBuffer, 1, ReadDataBuffer, ReadDataBuffer.Length,1<<2,0x10,200);
                //ret = USB2IIC.IIC_WriteReadBytes(DevIndex, IICIndex, SlaveAddr, WriteDataBuffer, 1, ReadDataBuffer, ReadDataBuffer.Length, 10);
                if (ret != USB2IIC.IIC_SUCCESS)
                {
                    Console.WriteLine("WriteRead IIC failed!");
                    Console.WriteLine("Error Code:{0}", ret);
                    return;
                }
                else
                {
                    Int16 ADCData = (Int16)((ReadDataBuffer[0] << 8) | (ReadDataBuffer[1]));
                    Console.WriteLine(String.Format("ADCData[{0:D}] = {1:F6} V",i,ADCData*4.096/32768.0));
                }
                System.Threading.Thread.Sleep(100);
            }
        }
    }
}
