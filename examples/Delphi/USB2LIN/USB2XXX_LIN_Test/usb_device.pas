unit usb_device;

interface
//定义电压输出值
const
POWER_LEVEL_NONE	 = 0;	//不输出
POWER_LEVEL_1V8		 = 1;	//输出1.8V
POWER_LEVEL_2V5		 = 2;	//输出2.5V
POWER_LEVEL_3V3		 = 3;	//输出3.3V
POWER_LEVEL_5V0		 = 4;	//输出5.0V

//定义初始化LIN初始化数据类型
type
PDEVICE_INFO = ^DEVICE_INFO;
DEVICE_INFO = record
  FirmwareName:Array[0..31] Of Byte;    //固件名称字符串
  BuildDate:Array[0..31] Of Byte;       //固件编译时间字符串
  HardwareVersion:LongWord;             //硬件版本号
  FirmwareVersion:LongWord;             //固件版本号
  SerialNumber:Array[0..2] Of LongWord; //适配器序列号
  Functions:LongWord;                   //适配器当前具备的功能
end;

function USB_ScanDevice(pDevHandle:PLongWord):Integer; stdcall;
function USB_OpenDevice(DevHandle:LongWord):Boolean; stdcall;
function USB_CloseDevice(DevHandle:LongWord):Boolean; stdcall;
function DEV_GetDeviceInfo(DevHandle:LongWord;pDevInfo:PDEVICE_INFO; pFunctionStr:PByte):Boolean; stdcall;
function DEV_EraseUserData(DevHandle:LongWord):Boolean; stdcall;
function DEV_WriteUserData(DevHandle,OffsetAddr:LongWord;pWriteData:PByte;DataLen:LongWord):Boolean; stdcall;
function DEV_ReadUserData(DevHandle,OffsetAddr:LongWord;upReadData:PByte;DataLen:LongWord):Boolean; stdcall;
function DEV_SetPowerLevel(DevHandle:LongWord;PowerLevel:Byte):Boolean; stdcall;

implementation
function USB_ScanDevice;external 'USB2XXX.dll' name 'USB_ScanDevice';
function USB_OpenDevice;external 'USB2XXX.dll' name 'USB_OpenDevice';
function USB_CloseDevice;external 'USB2XXX.dll' name 'USB_CloseDevice';
function DEV_GetDeviceInfo;external 'USB2XXX.dll' name 'DEV_GetDeviceInfo';
function DEV_EraseUserData;external 'USB2XXX.dll' name 'DEV_EraseUserData';
function DEV_WriteUserData;external 'USB2XXX.dll' name 'DEV_WriteUserData';
function DEV_ReadUserData;external 'USB2XXX.dll' name 'DEV_ReadUserData';
function DEV_SetPowerLevel;external 'USB2XXX.dll' name 'DEV_SetPowerLevel';
end.
