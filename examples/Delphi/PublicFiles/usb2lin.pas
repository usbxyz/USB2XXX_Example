unit usb2lin;

interface

const
//定义函数返回错误代码
LIN_SUCCESS             = (0);   //函数执行成功
LIN_ERR_NOT_SUPPORT     = (-1);  //适配器不支持该函数
LIN_ERR_USB_WRITE_FAIL  = (-2);  //USB写数据失败
LIN_ERR_USB_READ_FAIL   = (-3);  //USB读数据失败
LIN_ERR_CMD_FAIL        = (-4);  //命令执行失败
LIN_ERR_CH_NO_INIT      = (-5);  //该通道未初始化
LIN_ERR_READ_DATA       = (-6);  //LIN读数据失败
//LIN和校验模式
LIN_CHECK_MODE_STD     = 0;
LIN_CHECK_MODE_EXT     = 1;
LIN_CHECK_MODE_NONE    = 2;
//从机模式下检测BREAK信号位数定义
LIN_BREAK_BITS_10    = $00;
LIN_BREAK_BITS_11    = $20;
//定义主从模式
LIN_MASTER          = 1;
LIN_SLAVE           = 0;
//定义从机操作模式
LIN_SLAVE_WRITE     = 0;
LIN_SLAVE_READ      = 1;

//定义初始化LIN初始化数据类型
type
PLIN_CONFIG = ^LIN_CONFIG;
LIN_CONFIG = record
  BaudRate:LongWord;      //波特率,最大20K
  CheckMode:Byte;         //校验模式，0-标准校验模式，1-增强校验模式（包含PID）
  MasterMode:Byte;        //主从模式，0-从模式，1-主模式
  BreakBits:Byte;         //Break长度，0x00-10bit,0x20-11bit
end;
//LIN数据收发帧格式定义
type
PLIN_MSG = ^LIN_MSG;
LIN_MSG = record
  ID:Byte;                  //ID，取值范围0~0x3F
  DataLen:Byte;             //发送数据时，代表发送数据的长度，不含校验数据，接收数据时，数据的长度，包含校验数据
  Data:Array[0..8] Of Byte; //数据存储区
end;
//函数声明
function LIN_Init(DevHandle:LongWord; LINIndex:Byte;pConfig:PLIN_CONFIG):Integer; stdcall;
function LIN_Write(DevHandle:LongWord; LINIndex:Byte;pLINMsg:PLIN_MSG;Len:LongWord):Integer; stdcall;
function LIN_Read(DevHandle:LongWord; LINIndex:Byte;pLINMsg:PLIN_MSG;Len:LongWord):Integer; stdcall;
function LIN_SlaveSetIDMode(DevHandle:LongWord; LINIndex,IDMode:Byte;pLINMsg:PLIN_MSG;Len:LongWord):Integer; stdcall;
function LIN_SlaveGetData(DevHandle:LongWord; LINIndex:Byte;pLINMsg:PLIN_MSG):Integer; stdcall;

implementation
function LIN_Init;external 'USB2XXX.dll' name 'LIN_Init';
function LIN_Write;external 'USB2XXX.dll' name 'LIN_Write';
function LIN_Read;external 'USB2XXX.dll' name 'LIN_Read';
function LIN_SlaveSetIDMode;external 'USB2XXX.dll' name 'LIN_SlaveSetIDMode';
function LIN_SlaveGetData;external 'USB2XXX.dll' name 'LIN_SlaveGetData';
end.
