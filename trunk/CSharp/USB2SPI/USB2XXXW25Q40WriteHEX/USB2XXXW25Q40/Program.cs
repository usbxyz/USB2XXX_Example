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
            int[] DevHandle = new int[20];
            Console.WriteLine("扫描设备...");
            int DevCnt = usb_device.USB_ScanDevice(DevHandle); ;//获取设备连接数，同时获取每个设备的序号
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
                    Console.Write(DevHandle[i].ToString("X8") + "  ");
                }
                Console.WriteLine("");
            }
            //依次打开设备
            for (int i = 0; i < DevCnt; i++)
            {
                bool state;
                //打开设备
                state = usb_device.USB_OpenDevice(DevHandle[i]);
                if (!state)
                {
                    Console.WriteLine("Open device({0}) error!", DevHandle[i]);
                }
            }
            //启动自动烧写数据线程
            int TestTimes = 100;
            while (TestTimes > 0)
            {
                TestTimes--;
                Thread[] t = new Thread[DevCnt];
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i] = new Thread(new ParameterizedThreadStart(ChipWriteDataThread));
                    t[i].IsBackground = true;
                    t[i].Start(DevHandle[i].ToString());
                }
                for (int i = 0; i < DevCnt; i++)
                {
                    t[i].Join();
                }
                Console.WriteLine("TestTimes = {0}", TestTimes);
            }
        }

        public static int WriteDataFromHexFile(int DevHandle,int SPIIndex,string FileName)
        {
            StreamReader HexReader = new StreamReader(FileName);
            String szLine = "";
            String szHex = "";
            int StartAddr = 0;
            int Addr;
            while (true)
            {
                szLine = HexReader.ReadLine(); //读取一行数据

                if (szLine == null) //读完所有行
                {
                    break;
                }
                if (szLine.Substring(0, 1) == ":") //判断第1字符是否是:
                {
                    int DataLen = Convert.ToByte(szLine.Substring(1, 2), 16);
                    Byte DataType = Convert.ToByte(szLine.Substring(7, 2), 16);
                    if (DataType == 0x01)//'01' End of File Record: 用来标识文件结束，放在文件的最后，标识HEX文件的结尾
                    {
                        break;
                    }
                    if (DataType == 0x04)//'04' Extended Linear Address Record: 用来标识扩展线性地址的记录
                    {
                        StartAddr = Convert.ToInt16(szLine.Substring(9, 4), 16);
                    }
                    if (DataType == 0x00)//'00' Data Rrecord：用来记录数据，HEX文件的大部分记录都是数据记录
                    {
                        Addr = Convert.ToUInt16(szLine.Substring(3, 4), 16) + StartAddr;
                        szHex = szLine.Substring(9, DataLen * 2); //读取有效字符：后0和1
                        Byte[] DataBuffer = new Byte[DataLen];
                        Int32 i = 0, j = 0;
                        Byte RealCheckSum = 0;//数据校验和
                        for (i = 0; i < DataLen; i ++) //两字符合并成一个16进制字节
                        {
                            DataBuffer[j] = Convert.ToByte(szHex.Substring(i*2, 2), 16);
                            RealCheckSum += DataBuffer[j];
                            j++;
                        }
                        for (i = 0; i < 4; i++)
                        {
                            RealCheckSum += Convert.ToByte(szLine.Substring(1 + i * 2, 2), 16);
                        }
                        Byte CheckSum = Convert.ToByte(szLine.Substring(9 + DataLen * 2, 2), 16);//文件中记录的校验和
                        if (CheckSum != (Byte)(0x100 - RealCheckSum))
                        {
                            Console.WriteLine("{0}文件数据校验有误!", DevHandle);
                            break;
                        }
                        int ret = USB2SPI.SPI_FlashWrite(DevHandle, SPIIndex, Addr, DataBuffer, DataLen);
                        if (ret != USB2SPI.SPI_SUCCESS)
                        {
                            Console.WriteLine("{0}芯片写数据出错!", DevHandle);
                            return ret;
                        }
                    }   
                }
            }
            HexReader.Close(); //关闭目标文件
            return 0;
        }

        public static int VerifyDataFromHexFile(int DevHandle, int SPIIndex, string FileName)
        {
            StreamReader HexReader = new StreamReader(FileName);
            String szLine = "";
            String szHex = "";
            int StartAddr = 0;
            int Addr;
            int ErrorDataCount = 0;
            while (true)
            {
                szLine = HexReader.ReadLine(); //读取一行数据
                if (szLine == null) //读完所有行
                {
                    break;
                }
                if (szLine.Substring(0, 1) == ":") //判断第1字符是否是:
                {
                    int DataLen = Convert.ToByte(szLine.Substring(1, 2), 16);
                    Byte DataType = Convert.ToByte(szLine.Substring(7, 2), 16);
                    if (DataType == 0x01)//'01' End of File Record: 用来标识文件结束，放在文件的最后，标识HEX文件的结尾
                    {
                        break;
                    }
                    if (DataType == 0x04)//'04' Extended Linear Address Record: 用来标识扩展线性地址的记录
                    {
                        StartAddr = Convert.ToInt16(szLine.Substring(9, 4),16);
                    }
                    if (DataType == 0x00)//'00' Data Rrecord：用来记录数据，HEX文件的大部分记录都是数据记录
                    {
                        Addr = Convert.ToUInt16(szLine.Substring(3, 4), 16) + StartAddr;
                        szHex = szLine.Substring(9, DataLen * 2); //读取有效字符：后0和1
                        Byte[] DataBuffer = new Byte[DataLen];
                        Int32 i = 0, j = 0;
                        Byte RealCheckSum = 0;//数据校验和
                        for (i = 0; i < DataLen; i++) //两字符合并成一个16进制字节
                        {
                            DataBuffer[j] = Convert.ToByte(szHex.Substring(i * 2, 2), 16);
                            RealCheckSum += DataBuffer[j];
                            j++;
                        }
                        for (i = 0; i < 4; i++)
                        {
                            RealCheckSum += Convert.ToByte(szLine.Substring(1 + i * 2, 2), 16);
                        }
                        Byte CheckSum = Convert.ToByte(szLine.Substring(9 + DataLen * 2, 2), 16);//文件中记录的校验和
                        if (CheckSum != (Byte)(0x100 - RealCheckSum))
                        {
                            Console.WriteLine("{0}文件数据校验有误!", DevHandle);
                            break;
                        }
                        Byte[] DataBufferTemp = new Byte[DataLen];
                        int ret = USB2SPI.SPI_FlashRead(DevHandle, SPIIndex, Addr, DataBufferTemp, DataLen);
                        if (ret != USB2SPI.SPI_SUCCESS)
                        {
                            Console.WriteLine("{0}芯片读数据出错!", DevHandle);
                            return ret;
                        }
                        for (i = 0; i < DataLen; i++)
                        {
                            if (DataBuffer[i] != DataBufferTemp[i])
                            {
                                ErrorDataCount++;
                            }
                        }
                    }
                }
            }
            HexReader.Close(); //关闭目标文件
            return ErrorDataCount;
        }

        public static void ChipWriteDataThread(object Index)
        {
            USB2SPI.SPI_FLASH_CONFIG SPIFlashConfig = new USB2SPI.SPI_FLASH_CONFIG();
            int ret;
            int SPIIndex = 0;//使用SPI1
            int DevHandle = Convert.ToInt32(Index.ToString());
            //根据W25Q40配置USB2SPI适配器
            Console.WriteLine("{0}配置USB2SPI适配器...", DevHandle);
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
            ret = USB2SPI.SPI_FlashInit(DevHandle, SPIIndex, 50000000 >> 1, ref SPIFlashConfig);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}初始化设备错误!", DevHandle);
                return;
            }
            //读取芯片ID
            /*Console.WriteLine("{0}读取芯片ID...", SPIIndex);
            ret = USB2SPI.SPI_FlashReadID(DevHandle, SPIIndex, SPIFlashConfig.ID);
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
            }*/
            //整片擦除，该操作比较耗时
            Console.WriteLine("{0}开始擦除芯片!", DevHandle);
            ret = USB2SPI.SPI_FlashEraseChip(DevHandle, SPIIndex);
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("擦除芯片失败!");
                return;
            }
            //读取Hex文件并将数据烧入Flash
            Console.WriteLine("开始将Hex文件中的数据写入芯片!");
            ret = WriteDataFromHexFile(DevHandle, SPIIndex, "CAACDS00-001-R02M0.HEX");
            if (ret != USB2SPI.SPI_SUCCESS)
            {
                Console.WriteLine("{0}芯片写数据出错!", DevHandle);
                return;
            }
            //读取Hex文件并校验Flash中的数据
            Console.WriteLine("开始校验Flash中的数据!");
            int ErrorDataCount = VerifyDataFromHexFile(DevHandle, SPIIndex, "CAACDS00-001-R02M0.HEX");
            //统计出错的数据字节数
            if (ErrorDataCount > 0)
            {
                Console.WriteLine("{0}数据校验失败!有{1}字节出错！", DevHandle, ErrorDataCount);
            }
            else if (ErrorDataCount == 0)
            {
                Console.WriteLine("{0}烧写完毕!数据校验无误！", DevHandle);
            }
            else
            {
                Console.WriteLine("{0}读数据出错，数据校验失败！", DevHandle);
            }
        }
    }
}
