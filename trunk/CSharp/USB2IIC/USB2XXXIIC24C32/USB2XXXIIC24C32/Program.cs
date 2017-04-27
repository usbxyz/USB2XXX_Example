#define GET_FIRMWARE_INFO
#define AUTO_GET_SLAVE_ADDR
//#define DISPLAY_READ_DATA
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;



namespace USB2XXXIIC24C32
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Int32 IICIndex = 2;
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
            state = usb_device.USB_GetDeviceInfo(DevHandle, ref DevInfo, FuncStr);
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
#if AUTO_GET_SLAVE_ADDR
            //获取总线上能应答的从机地址
            Int16[] SlaveAddrs = new Int16[128];  //地址存储缓冲区
            int SlaveAddrNum = 0;   //返回应答的地址个数
            SlaveAddrNum = USB2IIC.IIC_GetSlaveAddr(DevHandle,IICIndex,SlaveAddrs);
            if(SlaveAddrNum >= 0){
                Console.WriteLine("Get {0} slave address!",SlaveAddrNum);
                for(int i=0;i<SlaveAddrNum;i++){
                    Console.Write(SlaveAddrs[i].ToString("X2")+" ");
                }
                Console.WriteLine("");
            }else{
                Console.WriteLine("Get slave address error!");
                return;
            }
#endif
            //向芯片写数据
            Int16 SlaveAddr = 0x50;
            UInt16 PageSize = 32;
            UInt16 PageNum = 128;
            UInt16 PageAddr = 0;
            Byte[] WriteDataBuffer = new Byte[2 + PageSize];//前2字节存储写数据起始地址，每页只有32字节，所以每次只能写32字节数据
            for (int i = 0; i < PageNum; i++)
            {
                //前2字节为子地址
                WriteDataBuffer[0] = (Byte)(PageAddr >> 8);
                WriteDataBuffer[1] = (Byte)(PageAddr &0xFF);
                //填充要写的数据
                for (int j = 0; j < PageSize; j++)
                {
                    WriteDataBuffer[2 + j] = (Byte)j;
                }
                //调用写数据函数
                ret = USB2IIC.IIC_WriteBytes(DevHandle, IICIndex, SlaveAddr, WriteDataBuffer, WriteDataBuffer.Length, 1000);
                if (ret != USB2IIC.IIC_SUCCESS)
                {
                    Console.WriteLine("Write IIC failed!");
                    Console.WriteLine("Error Code:{0}", ret);
                    return;
                }
                PageAddr += PageSize;
                //延时10ms，否则可能会导致写数据失败
                System.Threading.Thread.Sleep(10);
            }
            Console.WriteLine("Write IIC success!");
            //从芯片读数据
            Byte[] ReadDataBuffer = new Byte[PageNum * PageSize];
            PageAddr = 0x00;//读数据起始地址
            WriteDataBuffer[0] = (Byte)(PageAddr >> 8);
            WriteDataBuffer[1] = (Byte)(PageAddr & 0xFF);
            //调用写读数据函数
            ret = USB2IIC.IIC_WriteReadBytes(DevHandle, IICIndex, SlaveAddr, WriteDataBuffer, 2, ReadDataBuffer, ReadDataBuffer.Length, 1000);
            if (ret != USB2IIC.IIC_SUCCESS)
            {
                Console.WriteLine("WriteRead IIC failed!");
                Console.WriteLine("Error Code:{0}", ret);
                return;
            }
            else
            {
                Console.WriteLine("WriteRead IIC Success!");
#if DISPLAY_READ_DATA
                Console.WriteLine("Read Data:");
                for (int i = 0; i < ReadDataBuffer.Length; i++)
                {
                    Console.Write(ReadDataBuffer[i].ToString("X2") + " ");
                }
                Console.WriteLine("");
#endif
            }
        }
    }
}
