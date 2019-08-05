using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using USB2XXX;

//若提示找不到USB2XXX.dll文件，把工程目录下的USB2XXX.dll和libusb-1.0.dll文件拷贝到程序输出目录下即可
namespace USB2XXX_I2C_SnifferTest
{
    class Program
    {
        public static Int32 GetI2CDataHandle(Int32 DevHandle, Int32 Channel, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)]I2C_SNIFFER.I2C_DATA[] pI2CData, Int32 I2CDataNum)
        {
            int i = 0;
            for (i = 0; i < I2CDataNum; i++)
            {
                if (pI2CData[i].DataType == I2C_SNIFFER.IIC_DATA_TYPE_START)
                {
                    Console.Write("S");
                }
                else if (pI2CData[i].DataType == I2C_SNIFFER.IIC_DATA_TYPE_ADDR)
                {
                    Console.Write("<{0:02X}>", pI2CData[i].Addr);
                    if((pI2CData[i].WriteRead & 0x01)!=0)
                    {
                        Console.Write("<R>");
                    }
                    else
                    {
                        Console.Write("<W>");
                    }
                }
                else if (pI2CData[i].DataType == I2C_SNIFFER.IIC_DATA_TYPE_DATA)
                {
                    Console.Write("{0:02X} ", pI2CData[i].Data);
                }
                else if (pI2CData[i].DataType == I2C_SNIFFER.IIC_DATA_TYPE_STOP)
                {
                    Console.WriteLine("P");
                }
            }
            return 0;
        }
        static I2C_SNIFFER.I2C_GET_DATA_HANDLE get_data_callback = GetI2CDataHandle;
        static void Main(string[] args)
        {
            USB_DEVICE.DEVICE_INFO DevInfo = new USB_DEVICE.DEVICE_INFO();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            bool state;
            Int32 DevNum, ret;
            //扫描查找设备
            DevNum = USB_DEVICE.USB_ScanDevice(DevHandles);
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
            state = USB_DEVICE.USB_OpenDevice(DevHandle);
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
            state = USB_DEVICE.DEV_GetDeviceInfo(DevHandle, ref DevInfo, FuncStr);
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
            //启动I2C Sniffer
            GC.KeepAlive(get_data_callback);
            ret = I2C_SNIFFER.I2C_SnifferStart(DevHandles[0], 0, 5000000, get_data_callback);
            if (ret != I2C_SNIFFER.SUCCESS)
            {
                Console.WriteLine("Start i2c sniffer faild!");
                return;
            }
            Console.ReadLine();
            I2C_SNIFFER.I2C_SnifferStop(DevHandles[0]);
            USB_DEVICE.USB_CloseDevice(DevHandles[0]);
        }
    }
}
