
#define GET_FIRMWARE_INFO
#define AUTO_GET_SLAVE_ADDR
//#define SLAVE_WRITE_TEST
//#define SLAVE_READ_TEST

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;

namespace USB2XXXIICTest
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
            }
            //初始化配置I2C
            USB2IIC.IIC_CONFIG IICConfig = new USB2IIC.IIC_CONFIG();
            IICConfig.AddrBits = 7;            //7bit地址模式
            IICConfig.ClockSpeedHz = 400000;     //时钟频率400KHz
        #if SLAVE_WRITE_TEST||SLAVE_READ_TEST
            IICConfig.Master = 0;              //从机模式
            IICConfig.OwnAddr = 0x71;          //从机地址
        #else
            IICConfig.Master = 1;              //主机模式
        #endif
            ret = USB2IIC.IIC_Init(DevHandle,IICIndex,ref IICConfig);
            if(ret != USB2IIC.IIC_SUCCESS){
                Console.WriteLine("Config IIC failed!");
                return;
            }else{
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
        #if SLAVE_WRITE_TEST
            //IIC 从机模式写数据
            Byte[] SlaveWriteBuffer = new Byte[256];
            for(int i=0;i<SlaveWriteBuffer.Length;i++){
                SlaveWriteBuffer[i] = (Byte)i;
            }
            for(int i=0;i<(SlaveWriteBuffer.Length/8);i++){
                ret = USB2IIC.IIC_SlaveWriteBytes(DevHandle,IICIndex,SlaveWriteBuffer,8,10000);
                if(ret < 0){
                    Console.WriteLine("Slave write data error!");
                    break;
                }else if(ret == 0){
                    Console.WriteLine("Slave write data IIC_SUCCESS!");
                }else if(ret > 0){
                    Console.WriteLine("Slave write data IIC_SUCCESS! have {0} byte data remain",ret);
                }
            }
            Console.WriteLine("Slave write end!");
            return;
        #endif
        #if SLAVE_READ_TEST
            //IIC 从机模式读数据
            Byte[] SlaveReadBuffer = new Byte[256];
            for(int i=0;i<16;i++){
                ret = USB2IIC.IIC_SlaveReadBytes(DevHandle, IICIndex, SlaveReadBuffer, 10000);
                if(ret < 0){
                    Console.WriteLine("Slave read data error!");
                    break;
                }else if(ret == 0){
                    Console.WriteLine("Slave read data IIC_SUCCESS! but no data");
                }else if(ret > 0){
                    Console.WriteLine("Slave read data IIC_SUCCESS! have {0} byte data have read", ret);
                    for(i=0;i<ret;i++){
                        Console.WriteLine(SlaveReadBuffer[i].ToString("X2"));
                    }
                    Console.WriteLine("");
                }
            }
            Console.WriteLine("Slave read end!\n");
            return;
        #endif
            //IIC 写数据
            Byte[] WriteBuffer=new Byte[8];
            for(int i=0;i<WriteBuffer.Length;i++){
                WriteBuffer[i] = (Byte)i;
            }
            ret = USB2IIC.IIC_WriteBytes(DevHandle,IICIndex,0x50,WriteBuffer,WriteBuffer.Length,1000);
            if(ret != USB2IIC.IIC_SUCCESS){
                Console.WriteLine("Write IIC failed!");
                Console.WriteLine("Error Code:{0}",ret);
                return;
            }else{
                Console.WriteLine("Write IIC Success!");
            }
            //延时
            System.Threading.Thread.Sleep(10);
            //IIC 读数据
            Byte[] ReadBuffer = new Byte[8];
            ret = USB2IIC.IIC_ReadBytes(DevHandle,IICIndex,0x50,ReadBuffer,ReadBuffer.Length,1000);
            if(ret != USB2IIC.IIC_SUCCESS){
                Console.WriteLine("Read IIC failed!");
                Console.WriteLine("Error Code:{0}",ret);
                return;
            }else{
                Console.WriteLine("Read IIC Success!");
                Console.WriteLine("Read Data:");
                for(int i=0;i<ReadBuffer.Length;i++){
                    Console.Write(ReadBuffer[i].ToString("X2")+" ");
                }
                Console.WriteLine("");
            }
            //延时
            System.Threading.Thread.Sleep(10);
            //IIC 写读数据。也就是先发送数据，然后再次产生START信号，再读数据
            //注意：对于每个I2C设备，必须按照设备要求正确读写，否则可能会导致设备工作不正常，从而导致无法读写数据
            WriteBuffer[0] = 0x08;
            ret = USB2IIC.IIC_WriteReadBytes(DevHandle, IICIndex, 0x50, WriteBuffer, 1, ReadBuffer, ReadBuffer.Length, 1000);
            if (ret != USB2IIC.IIC_SUCCESS)
            {
                Console.WriteLine("WriteRead IIC failed!");
                Console.WriteLine("Error Code:{0}", ret);
                return;
            }else{
                Console.WriteLine("WriteRead IIC Success!");
                Console.WriteLine("Read Data:");
                for(int i=0;i<ReadBuffer.Length;i++){
                    Console.Write(ReadBuffer[i].ToString("X2")+" ");
                }
                Console.WriteLine("");
            }
            //关闭设备
            usb_device.USB_CloseDevice(DevHandle);
        }
    }
}
