Attribute VB_Name = "usb2spi"

Public Const SPI1 = &H0
Public Const SPI1_CS0 = &H0
Public Const SPI1_CS1 = &H10
Public Const SPI1_CS2 = &H20
Public Const SPI1_CS3 = &H30
Public Const SPI1_CS4 = &H40
 
Public Const SPI2 = &H1
Public Const SPI2_CS0 = &H1
Public Const SPI2_CS1 = &H11
Public Const SPI2_CS2 = &H21
Public Const SPI2_CS3 = &H31
Public Const SPI2_CS4 = &H41

Public Const SPI_MODE_HARD_FDX = 0
Public Const SPI_MODE_HARD_HDX = 1
Public Const SPI_MODE_SOFT_HDX = 2
Public Const SPI_MODE_SOFT_ONE_WIRE = 3

Public Const SPI_MASTER = 1
Public Const SPI_SLAVE = 0

Public Const SPI_MSB = 0
Public Const SPI_LSB = 1

Public Const SPI_SEL_LOW = 0
Public Const SPI_SEL_HIGH = 1

Public Const SPI_EVENT_P0 = &H1
Public Const SPI_EVENT_P1 = &H2
Public Const SPI_EVENT_P2 = &H4
Public Const SPI_EVENT_P3 = &H8
Public Const SPI_EVENT_P4 = &H10
Public Const SPI_EVENT_P5 = &H20
Public Const SPI_EVENT_P6 = &H40
Public Const SPI_EVENT_P7 = &H80
Public Const SPI_EVENT_P8 = &H100
Public Const SPI_EVENT_P9 = &H200
Public Const SPI_EVENT_P10 = &H400
Public Const SPI_EVENT_P11 = &H800
Public Const SPI_EVENT_P12 = &H1000
Public Const SPI_EVENT_P13 = &H2000
Public Const SPI_EVENT_P14 = &H4000
Public Const SPI_EVENT_P15 = &H8000

Public Const EVENT_TYPE_LOW = 0
Public Const EVENT_TYPE_HIGH = &H11
Public Const EVENT_TYPE_RISING = &H1
Public Const EVENT_TYPE_FALLING = &H10
 
Public Const SPI_SUCCESS = 0
Public Const SPI_ERR_NOT_SUPPORT = -1
Public Const SPI_ERR_USB_WRITE_FAIL = -2
Public Const SPI_ERR_USB_READ_FAIL = -3
Public Const SPI_ERR_CMD_FAIL = -4
Public Const SPI_ERR_PARAMETER = -5

Public Type SPI_CONFIG
  Mode As Byte
  Master As Byte
  CPOL As Byte
  CPHA   As Byte
  LSBFirst As Byte
  SelPolarity As Byte
  ClockSpeedHz As Long
End Type

Public Type SPI_FLASH_CONFIG
    CMD_WriteEnable As Byte
    CMD_WriteDisable  As Byte
    CMD_WritePage As Byte
    WritePageAddressBytes As Byte
    CMD_EraseSector As Byte
    EraseSectorAddressBytes As Byte
    CMD_EraseBulk As Byte
    CMD_EraseChip As Byte
    CMD_ReadID As Byte
    CMD_ReadData As Byte
    ReadDataAddressBytes As Byte
    CMD_ReadFast As Byte
    ReadFastAddressBytes As Byte
    CMD_ReadStatus As Byte
    CMD_WriteStatus As Byte
    ID(15) As Byte
    ID_Length As Byte
    PageSize As Long
    NumPages As Long
    SectorSize As Long
End Type

Declare Function SPI_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByRef pConfig As SPI_CONFIG) As Long
Declare Function SPI_WriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long) As Long
Declare Function SPI_WriteBytesAsync Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long) As Long
Declare Function SPI_ReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadData As Any, ByVal ReadLen As Long) As Long
Declare Function SPI_WriteReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal IntervalTimeUs As Long) As Long

Declare Function SPI_WriteBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfMs As Long) As Long
Declare Function SPI_ReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfMs As Long) As Long
Declare Function SPI_WriteReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal pReadData As Any, ByVal ReadLen As Long, ByVal IntervalTimeUs As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfMs As Long) As Long

Declare Function SPI_WriteBits Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteBitStr As Any) As Long
Declare Function SPI_ReadBits Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadBitStr As Any, ByVal ReadBitsNum As Long) As Long
Declare Function SPI_WriteReadBits Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteBitStr As Any, ByVal pReadBitStr As Any, ByVal ReadBitsNum As Long) As Long

Declare Function SPI_SlaveWriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteLen As Long, ByVal TimeOutMs As Long) As Long
Declare Function SPI_SlaveReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadData As Any, ByVal TimeOutMs As Long) As Long
'Declare Function SPI_SlaveContinueRead Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, pSlaveReadDataHandle As SPI_GET_DATA_HANDLE) As Long
Declare Function SPI_SlaveGetBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadData As Any, BufferSize As Long) As Long
Declare Function SPI_SlaveContinueReadStop Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long) As Long

Declare Function SPI_FlashInit Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ClockSpeed As Long, pConfig As SPI_FLASH_CONFIG) As Long
Declare Function SPI_FlashReadID Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, pID As Any) As Long
Declare Function SPI_FlashEraseSector Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, StartSector As Long, NumSector As Long) As Long
Declare Function SPI_FlashEraseChip Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long) As Long
Declare Function SPI_FlashWrite Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, StartAddr As Long, ByVal pWriteData As Any, ByVal WriteLen As Long) As Long
Declare Function SPI_FlashRead Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, StartAddr As Long, ByVal pReadData As Any, ByVal ReadLen As Long) As Long
Declare Function SPI_FlashReadFast Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, StartAddr As Long, ByVal pReadData As Any, ByVal ReadLen As Long) As Long
Declare Function SPI_BlockWriteBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal BlockSize As Long, ByVal BlockNum As Long, ByVal IntervalTimeUs As Long) As Long

Declare Function SPI_BlockReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadData As Any, ByVal BlockSize As Long, ByVal BlockNum As Long, ByVal IntervalTimeUs As Long) As Long
Declare Function SPI_BlockWriteReadBytes Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteBlockSize As Long, ByVal pReadData As Any, ByVal ReadBlockSize As Long, ByVal BlockNum As Long, ByVal IntervalTimeUs As Long) As Long

Declare Function SPI_BlockWriteBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal BlockSize As Long, ByVal BlockNum As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfMs As Long) As Long
Declare Function SPI_BlockReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pReadData As Any, ByVal BlockSize As Long, ByVal BlockNum As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfMs As Long) As Long
Declare Function SPI_BlockWriteReadBytesOfEvent Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal SPIIndex As Long, ByVal pWriteData As Any, ByVal WriteBlockSize As Long, ByVal pReadData As Any, ByVal ReadBlockSize As Long, ByVal BlockNum As Long, ByVal EventPin As Long, ByVal EventType As Byte, ByVal TimeOutOfMs As Long) As Long


