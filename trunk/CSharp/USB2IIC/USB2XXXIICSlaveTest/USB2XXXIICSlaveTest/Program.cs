using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;

namespace USB2XXXIICSlaveTest
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Int32 IICIndex = 0;
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
                Console.WriteLine("    Serial Number:" + DevInfo.SerialNumber[0].ToString("X8") + DevInfo.SerialNumber[1].ToString("X8") + DevInfo.SerialNumber[2].ToString("X8"));
            }
            //初始化配置I2C
            USB2IIC.IIC_CONFIG IICConfig = new USB2IIC.IIC_CONFIG();
            IICConfig.AddrBits = 7;            //7bit地址模式
            IICConfig.ClockSpeedHz = 400000;     //时钟频率400KHz
            IICConfig.Master = 0;              //从机模式
            IICConfig.OwnAddr = 0x71;          //从机地址
            ret = USB2IIC.IIC_Init(DevHandle, IICIndex, ref IICConfig);
            if (ret != USB2IIC.IIC_SUCCESS)
            {
                Console.WriteLine("Config IIC failed!");
                return;
            }
            else
            {
                Console.WriteLine("Config IIC IIC_SUCCESS!");
            }
            while (true)
            {
                //IIC 从机模式读数据
                Byte[] SlaveReadBuffer = new Byte[20480];
                ret = USB2IIC.IIC_SlaveReadBytes(DevHandle, IICIndex, SlaveReadBuffer, 1000);//超时时间设置为1000ms，超时之后，不管是否读到主机发送过来的数据，函数都返回
                if (ret < 0)
                {
                    Console.WriteLine("Slave read data error!");
                    break;
                }
                else if (ret > 0)
                {
                    Console.WriteLine("Slave read data IIC_SUCCESS! have {0} byte data have read", ret);
                    for (int i = 0; i < ret; i++)
                    {
                        Console.Write(SlaveReadBuffer[i].ToString("X2")+" ");
                    }
                    Console.WriteLine("");
                    //读到数据之后，将读到的数据发送到适配器内部数据缓冲区，等待主机来读取
                    //IIC 从机模式写数据
                    ret = USB2IIC.IIC_SlaveWriteBytes(DevHandle, IICIndex, SlaveReadBuffer, ret, 3000);//超时时间设置为1000ms，超时之后，不管主机是否读取数据，函数都返回
                    if (ret < 0)
                    {
                        Console.WriteLine("Slave write data error!");
                        break;
                    }
                    else if (ret == 0)
                    {
                        Console.WriteLine("Slave write data IIC_SUCCESS!");
                    }
                    else if (ret > 0)
                    {
                        Console.WriteLine("Slave write data IIC_SUCCESS! have {0} byte data remain", ret);
                    }
                }
            }
        }
    }
}
