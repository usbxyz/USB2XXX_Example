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
            /*
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
            */
            //启动自动烧写数据线程
            int TestTimes = 1;
            while (TestTimes > 0)
            {
                TestTimes--;
                Thread[] t = new Thread[DevCnt];
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i] = new Thread(new ParameterizedThreadStart(ChipWriteDataThread));
                    t[i].IsBackground = true;
                    t[i].Start(i.ToString());
                }
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i].Join();
                }
                Console.WriteLine("TestTimes = {0}", TestTimes);
                //Console.ReadKey();
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
        public static void ChipWriteDataThread(object Index)
        {
            USB2SPI.SPI_FLASH_CONFIG SPIFlashConfig = new USB2SPI.SPI_FLASH_CONFIG();
            int ret;
            int SPIIndex = 0;//使用SPI1
            int DevIndex = Convert.ToInt32(Index.ToString());
            //根据W25Q40配置USB2SPI适配器
            Console.WriteLine("{0}配置USB2SPI适配器...", DevIndex);
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
            SPIFlashConfig.ID = new Byte[16];
            SPIFlashConfig.NumPages = 4096;
            SPIFlashConfig.PageSize = 256;
            SPIFlashConfig.ReadDataAddressBytes = 3;
            SPIFlashConfig.ReadFastAddressBytes = 3;
            SPIFlashConfig.SectorSize = 4096;
            SPIFlashConfig.WritePageAddressBytes = 3;
            ret = USB2SPI.SPI_FlashInit(DevIndex, SPIIndex, 50000000 >> 1, ref SPIFlashConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}初始化设备错误!", DevIndex);
                return;
            }
            //读取芯片ID
            Console.WriteLine("{0}读取芯片ID...", SPIIndex);
            ret = USB2SPI.SPI_FlashReadID(DevIndex, SPIIndex, SPIFlashConfig.ID);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}获取芯片ID出错！", SPIIndex);
                return;
            }
            else
            {
                Console.Write("{0}芯片ID为：", SPIIndex);
                for (int i = 0; i < SPIFlashConfig.ID_Length; i++)
                {
                    Console.Write(SPIFlashConfig.ID[i].ToString("X2"));
                }
                Console.WriteLine("");
            }
            //整片擦除，该操作比较耗时
            Console.WriteLine("{0}开始擦除芯片!", DevIndex);
            ret = USB2SPI.SPI_FlashEraseChip(DevIndex, SPIIndex);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("擦除芯片失败!");
                return;
            }
            //将要烧写的文件读到内存
            const string name = "data.bin";
            //打开文件
            FileStream fs = null;
            try
            {
                fs = fs = new FileStream(name, FileMode.Open, FileAccess.Read);
            }
            catch (Exception ex)
            {
                Console.WriteLine("{0}打开文件失败!", DevIndex);
                return;
            }
            // 读取数据
            BinaryReader r = new BinaryReader(fs);
            long ReadDataSize = fs.Length < (SPIFlashConfig.PageSize * SPIFlashConfig.NumPages) ? fs.Length : SPIFlashConfig.PageSize * SPIFlashConfig.NumPages;
            byte[] DataBuffer = r.ReadBytes((int)ReadDataSize);
            //循环将数据写入芯片
            Console.WriteLine("{0}开始写数据!", DevIndex);
            int count = 0;
            int PackSize = 10240;
            int StartAddr = 0;
            for (count = 0; count < (ReadDataSize / PackSize); count++)
            {
                byte[] DataBufferTemp = new byte[PackSize];
                Array.ConstrainedCopy(DataBuffer, PackSize * count, DataBufferTemp, 0, PackSize);
                ret = USB2SPI.SPI_FlashWrite(DevIndex, SPIIndex, StartAddr, DataBufferTemp, PackSize);
                if (ret != USB2SPI.SPI_SUCCESS)
                {
                    Console.WriteLine("{0}芯片写数据出错!", DevIndex);
                    return;
                }
                StartAddr += PackSize;
            }
            if ((ReadDataSize % PackSize) > 0)
            {
                byte[] DataBufferTemp = new byte[ReadDataSize % PackSize];
                Array.ConstrainedCopy(DataBuffer, PackSize * count, DataBufferTemp, 0, (int)(ReadDataSize % PackSize));
                ret = USB2SPI.SPI_FlashWrite(DevIndex, SPIIndex, StartAddr, DataBufferTemp, (int)(ReadDataSize % PackSize));
                if (ret != USB2SPI.SPI_SUCCESS)
                {
                    Console.WriteLine("{0}芯片写数据出错!", DevIndex);
                    return;
                }
            }
            //校验数据
            Console.WriteLine("{0}开始校验数据!", DevIndex);
            int ErrorDataCount = 0;
            StartAddr = 0;
            for (count = 0; count < (ReadDataSize / PackSize); count++)
            {
                byte[] DataBufferTemp = new byte[PackSize];
                ret = USB2SPI.SPI_FlashRead(DevIndex, SPIIndex, StartAddr, DataBufferTemp, PackSize);
                if (ret != USB2SPI.SPI_SUCCESS)
                {
                    Console.WriteLine("{0}芯片读数据出错!", DevIndex);
                    return;
                }
                else
                {
                    StartAddr += PackSize;
                    for (int i = 0; i < PackSize; i++)
                    {
                        if (DataBufferTemp[i] != DataBuffer[PackSize * count + i])
                        {
                            ErrorDataCount++;
                        }
                    }
                }
            }
            if ((ReadDataSize % PackSize) > 0)
            {
                byte[] DataBufferTemp = new byte[ReadDataSize % PackSize];
                ret = USB2SPI.SPI_FlashRead(DevIndex, SPIIndex, StartAddr, DataBufferTemp, (int)(ReadDataSize % PackSize));
                if (ret != USB2SPI.SPI_SUCCESS)
                {
                    Console.WriteLine("{0}芯片读数据出错!", DevIndex);
                    return;
                }
                else
                {
                    for (int i = 0; i < (ReadDataSize % PackSize); i++)
                    {
                        if (DataBufferTemp[i] != DataBuffer[PackSize * count + i])
                        {
                            ErrorDataCount++;
                        }
                    }
                }
            }
            //统计出错的数据字节数
            if (ErrorDataCount > 0)
            {
                Console.WriteLine("{0}数据校验失败!有{1}字节出错！", DevIndex, ErrorDataCount);
            }
            else
            {
                Console.WriteLine("{0}烧写完毕!数据校验无误！", DevIndex);
            }
        }
    }
}
