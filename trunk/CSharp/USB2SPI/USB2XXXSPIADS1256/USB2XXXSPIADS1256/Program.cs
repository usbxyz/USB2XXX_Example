using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USB2XXX;

namespace USB2XXXSPIADS1256
{
    class Program
    {
        static void Main(string[] args)
        {
            usb_device.DEVICE_INFO DevInfo = new usb_device.DEVICE_INFO();
            USB2SPI.SPI_CONFIG SPIConfig = new USB2SPI.SPI_CONFIG();
            Int32 DevIndex = 0;
            Int32 ADS1256Index = USB2SPI.SPI1_CS0;
            UInt32 ResetPinMask,DrdyPinMask;
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
                Console.WriteLine("    Serial Number:" + DevInfo.SerialNumber[0].ToString("X8") + DevInfo.SerialNumber[1].ToString("X8") + DevInfo.SerialNumber[2].ToString("X8"));
            }
            //配置RESET引脚和DRADY引脚
            if (ADS1256Index == 0)
            {
                ResetPinMask = 1 << 9;
                DrdyPinMask = 1 << 8;
            }
            else
            {
                ResetPinMask = 1 << 1;
                DrdyPinMask = 1 << 0;
            }
            ret = USB2GPIO.GPIO_SetOutput(DevIndex, ResetPinMask, 1);
            if (ret != USB2GPIO.GPIO_SUCCESS)
            {
                Console.WriteLine("Initialize gpio error!");
                return;
            }
            //配置SPI总线相关参数
            SPIConfig.Mode = USB2SPI.SPI_MODE_HARD_HDX;
            SPIConfig.ClockSpeedHz = 1562500;
            SPIConfig.CPHA = 1;
            SPIConfig.CPOL = 0;
            SPIConfig.LSBFirst = USB2SPI.SPI_MSB;
            SPIConfig.Master = USB2SPI.SPI_MASTER;
            SPIConfig.SelPolarity = USB2SPI.SPI_SEL_LOW;
            ret = USB2SPI.SPI_Init(DevIndex, ADS1256Index, ref SPIConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("Initialize device error!");
                return;
            }
            //RESET引脚输出低脉冲复位ADS1256
            USB2GPIO.GPIO_Write(DevIndex, ResetPinMask, 0); //输出低电平
            System.Threading.Thread.Sleep(10);
            USB2GPIO.GPIO_Write(DevIndex, ResetPinMask, ResetPinMask);  //输出高电平
            //准备配置寄存器数据
            WriteBuffer[0] = 0x50;//ADS1256_CMD_WREG
            WriteBuffer[1] = 0x04;
            WriteBuffer[2] = 0x04;
            WriteBuffer[3] = 0x08;//配置AIN0为单端模式
            WriteBuffer[4] = 0;   //PGA
            WriteBuffer[5] = 0xA1;//1000SPS
            WriteBuffer[6] = 0xFF;
            ret = USB2SPI.SPI_WriteBytesOfEvent(DevIndex, ADS1256Index, WriteBuffer, 7, (Int32)DrdyPinMask, 0x00, 1000);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI Event Write Data Error!");
                return;
            }
            //准备读配置寄存器数据，验证配置数据写入是否成功
            WriteBuffer[0] = 0x10;//ADS1256_CMD_RREG;
            WriteBuffer[1] = 4;
            ret = USB2SPI.SPI_WriteReadBytesOfEvent(DevIndex, ADS1256Index, WriteBuffer, 2, ReadBuffer,5,10,(Int32)DrdyPinMask, 0x10, 1000);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI Event Write&Read Data Error!");
                return;
            }
            //判断写入配置寄存器的值和读出寄存器的值是否一样
            if ((WriteBuffer[3] != ReadBuffer[1]) || (WriteBuffer[4] != ReadBuffer[2]) || (WriteBuffer[5] != ReadBuffer[3]))
            {
                Console.WriteLine("Config ADS1256 Error!");
                return;
            }
            //发送连续采集数据命令，该命令只适合连续采集一个通道的情况
            WriteBuffer[0] = 0xFC;//ADS1256_CMD_SYNC
            WriteBuffer[1] = 0x00;//ADS1256_CMD_WAKEUP
            WriteBuffer[2] = 0x03;//ADS1256_CMD_RDATAC
            ret = USB2SPI.SPI_WriteReadBytesOfEvent(DevIndex, ADS1256Index, WriteBuffer, 3, ReadBuffer, 3, 10, (Int32)DrdyPinMask, 0x10, 1000);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI Event Write&Read Data Error!");
                return;
            }
            Console.WriteLine("Continuous Get ADC Data!");
            //检测DRADY引脚下降沿之后读回数据
            int ReadDataNum = 10;
            ret = USB2SPI.SPI_BlockReadBytesOfEvent(DevIndex, ADS1256Index, ReadBuffer, 3, ReadDataNum, (Int32)DrdyPinMask, 0x10, 100);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI Event BlockRead Data Error!");
                return;
            }
            //将读回的数据转换为实际的电压值
            for (int i = 0; i < ReadDataNum; i++)
            {
                int ADCDataTemp = (ReadBuffer[i * 3 + 0] << 16) | (ReadBuffer[i * 3 + 1] << 8) | ReadBuffer[i * 3 + 2];
                if ((ADCDataTemp & 0x800000) != 0x00)
                {
                    ADCDataTemp = (Int32)(0xFF000000 | ((UInt32)ADCDataTemp));
                }
                double Volutage = (ADCDataTemp * 0.59604644775390625)/1000000;
                Console.WriteLine("ADC[{0}] = {1} V", i, Volutage);
            }
            //发送停止连续采样数据命令
            WriteBuffer[0] = 0x0F;//ADS1256_CMD_SDATAC
            ret = USB2SPI.SPI_WriteBytesOfEvent(DevIndex, ADS1256Index, WriteBuffer, 1, (Int32)DrdyPinMask, 0x10, 1000);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("SPI Event Write Data Error!");
                return;
            }
            Console.WriteLine("Circulate Get ADC Data!");
            //循环采集每个通道数据
            ReadDataNum = 3;
            byte Channel = 0;
            WriteBuffer[0] = 0x50|1;//ADS1256_CMD_WREG
            WriteBuffer[1] = 0x00;
            WriteBuffer[2] = (byte)((Channel << 4) | 0x08);
            WriteBuffer[3] = 0xFC;//ADS1256_CMD_SYNC
            WriteBuffer[4] = 0x00;//ADS1256_CMD_WAKEUP
            WriteBuffer[5] = 0x01;//ADS1256_CMD_RDATA
            for (int i = 0; i < ReadDataNum; i++)
            {
                for (Channel = 0; Channel < 8; Channel++)
                {
                    WriteBuffer[2] = (byte)((Channel << 4) | 0x08);
                    ret = USB2SPI.SPI_WriteReadBytesOfEvent(DevIndex, ADS1256Index, WriteBuffer, 6, ReadBuffer, 3, 10, (Int32)DrdyPinMask, 0x10, 1000);
                    if (ret != USB2SPI.SPI_SUCCESS)
                    {
                        Console.WriteLine("SPI Event Write&Read Data Error!");
                        return;
                    }
                    else
                    {
                        //丢弃第一次的数据
                        if ((i == 0) && (Channel == 0))
                        {
                            continue;
                        }
                        int ADCDataTemp = (ReadBuffer[0] << 16) | (ReadBuffer[1] << 8) | ReadBuffer[2];
                        if ((ADCDataTemp & 0x800000) != 0x00)
                        {
                            ADCDataTemp = (Int32)(0xFF000000 | ((UInt32)ADCDataTemp));
                        }
                        double Volutage = (ADCDataTemp * 0.59604644775390625) / 1000000;
                        Console.WriteLine("ADC_CH[{0}][{1}] = {2} V", i, Channel == 0 ? 7 : Channel - 1, Volutage);
                    }
                }
            }
        }
    }
}
