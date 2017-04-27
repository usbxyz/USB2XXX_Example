using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;

namespace USB2XXXSPIW25Q64
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            USB2SPI.SPI_FLASH_CONFIG SPIFlashConfig = new USB2SPI.SPI_FLASH_CONFIG();
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Int32 SPIIndex = 1;
            bool state;
            Int32 DevNum, ret;
            Byte[] WriteBuffer = new Byte[64];
            Byte[] ReadBuffer = new Byte[20480];
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
            DevHandle = DevHandles[0];//选择设备0
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
                Console.WriteLine("    Serial Number:" + DevInfo.SerialNumber[0].ToString("X8") + DevInfo.SerialNumber[1].ToString("X8") + DevInfo.SerialNumber[2].ToString("X8"));
            }
            //根据W25Q64配置相关参数
            SPIFlashConfig.CMD_EraseSector = 0x20;
            SPIFlashConfig.CMD_ReadData = 0x03;
            SPIFlashConfig.CMD_ReadFast = 0x0B;
            SPIFlashConfig.CMD_ReadID = 0x9F;
            SPIFlashConfig.CMD_ReadStatus = 0x05;
            SPIFlashConfig.CMD_WriteEnable = 0x06;
            SPIFlashConfig.CMD_WritePage = 0x02;
            SPIFlashConfig.CMD_EraseChip = 0xC7;
            SPIFlashConfig.EraseSectorAddressBytes = 3;
            SPIFlashConfig.ID_Length = 3;
            SPIFlashConfig.NumPages = 32768;
            SPIFlashConfig.PageSize = 256;
            SPIFlashConfig.ReadDataAddressBytes = 3;
            SPIFlashConfig.ReadFastAddressBytes = 3;
            SPIFlashConfig.SectorSize = 4096;
            SPIFlashConfig.WritePageAddressBytes = 3;
            ret = USB2SPI.SPI_FlashInit(DevHandle, SPIIndex, 50000000 >> 2, ref SPIFlashConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("Initialize Device Error!");
                return;
            }
            //读取芯片ID
            ret = USB2SPI.SPI_FlashReadID(DevHandle, SPIIndex, SPIFlashConfig.ID);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("Get Device ID Error!");
                return;
            }
            else
            {
                Console.Write("ID = ");
                for (int i = 0; i < SPIFlashConfig.ID_Length; i++)
                {
                    Console.Write(SPIFlashConfig.ID[i].ToString("X2"));
                }
                Console.WriteLine("\n");
                if ((SPIFlashConfig.ID[0] == 0xFF) && (SPIFlashConfig.ID[1] == 0xFF))
                {
                    return;
                }
            }
            //擦除扇区
            ret = USB2SPI.SPI_FlashEraseSector(DevHandle, SPIIndex, 0, 1);
            if(ret != USB2SPI.SPI_SUCCESS){
                Console.WriteLine("Erase Sector Error!");
                return;
            }
            Byte[] TestBuffer = new Byte[20*1024];
            for (int i = 0; i < TestBuffer.Length; i++)
            {
                TestBuffer[i] = (Byte)i;
            }
            ret = USB2SPI.SPI_FlashWrite(DevHandle, SPIIndex, 0, TestBuffer, 256);
            if(ret != USB2SPI.SPI_SUCCESS){
                Console.WriteLine("Flash Write Error!");
                return;
            }
            ret = USB2SPI.SPI_FlashReadFast(DevHandle, SPIIndex, 0, TestBuffer, 256);
            if(ret != USB2SPI.SPI_SUCCESS){
                Console.WriteLine("Flash Read Error!");
                return;
            }
            for(int i=0;i<256;i++){
                if((i%16)==0){
                    Console.WriteLine("");
                }
                Console.Write(TestBuffer[i].ToString("X2")+" ");
            }
            Console.WriteLine("\n");
        }
    }
}
