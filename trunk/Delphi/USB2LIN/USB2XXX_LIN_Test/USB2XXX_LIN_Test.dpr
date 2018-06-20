program USB2XXX_LIN_Test;

{$APPTYPE CONSOLE}

{$R *.res}

uses
  System.SysUtils,
  usb2lin in 'usb2lin.pas',
  usb_device in 'usb_device.pas';
Var
    DevHandles:Array[0..9] Of LongWord;
    LINMasterIndex:Byte;
    LINSlaveIndex:Byte;
    state:Boolean;
    ret,i:LongWord;
    LINConfig:LIN_CONFIG;
    ID:Byte;
    msg:LIN_MSG;
begin
  try
    { TODO -oUser -cConsole Main : Insert code here }
    //Scan device(must call)
    ret := USB_ScanDevice(@DevHandles[0]);
    if ret<=0 then
      Writeln('No device connect!');
    //Open device(must call)
    state := USB_OpenDevice(DevHandles[0]);
    if state <> True then
      Writeln('Open device error!');
    //初始化配置LIN
    LINConfig.BaudRate := 19200;
    LINConfig.BreakBits := LIN_BREAK_BITS_11;
    LINConfig.CheckMode := LIN_CHECK_MODE_EXT;
    LINConfig.MasterMode := LIN_MASTER;
    LINMasterIndex := 0;
    ret := LIN_Init(DevHandles[0],LINMasterIndex,@LINConfig);
    if ret <> LIN_SUCCESS Then
        Writeln('Config LIN failed!')
    else
        Writeln('Config LIN Success!');
    //主机写数据
    for ID:=0 to 10 do
    begin
      for i:=0 to 8 do
        msg.Data[i] := ID+i;
      msg.DataLen := 8;
      msg.ID := ID;
      ret := LIN_Write(DevHandles[0],LINMasterIndex,@msg,1);
      if ret <> LIN_SUCCESS then
        Writeln('LIN write data failed!')
      else
        Writeln('LIN write data success!');
    end;
    //等待输入
    Readln
  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
end.
