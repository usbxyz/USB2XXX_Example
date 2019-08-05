Attribute VB_Name = "CANTest"

Public Declare Function AllocConsole Lib "kernel32.dll" () As Long
Public Declare Function FreeConsole Lib "kernel32.dll" () As Long
Public Declare Function SetConsoleTitle Lib "kernel32.dll" Alias "SetConsoleTitleA" (ByVal lpConsoleTitle As String) As Long
Public Declare Sub Sleep Lib "kernel32.dll" (ByVal dwMilliseconds As Long)

Private Declare Function GetStdHandle Lib "kernel32.dll" (ByVal nStdHandle As Long) As Long

Private Declare Function WriteConsole Lib "kernel32.dll" Alias "WriteConsoleA" _
                                                (ByVal hConsoleOutput As Long, _
                                                 lpBuffer As Any, _
                                                 ByVal nNumberOfCharsToWrite As Long, _
                                                 lpNumberOfCharsWritten As Long, _
                                                 lpReserved As Any) As Long
                                                 
Private Declare Function ReadConsole Lib "kernel32.dll" Alias "ReadConsoleA" _
                                                (ByVal hConsoleInput As Long, _
                                                lpBuffer As Any, _
                                                ByVal nNumberOfCharsToRead As Long, _
                                                lpNumberOfCharsRead As Long, _
                                                lpReserved As Any) As Long
                                                
Private Declare Function SetConsoleMode Lib "kernel32" (ByVal hConsoleHandle As Long, ByVal dwMode As Long) As Long

Private Const INPUT_HANDLE = -10&
Private Const OUTPUT_HANDLE = -11&
Private Const ERROR_HANDLE = -12&

Private Const LINE_INPUT = &H2
Private Const ECHO_INPUT = &H4
Private Const MOUSE_INPUT = &H10
Private Const PROCESSED = &H1

Public Function Printf(ByVal hConsoleOutput As Long, ByVal str As String) As Long
    Call WriteConsole(hConsoleOutput, ByVal str, Len(str), vbNull, vbNull)
End Function

Sub Main()
    Call AllocConsole
    Call SetConsoleTitle("USB2XXX CANTest")
    
    Dim inputHandle As Long
    Dim str As String
    Dim outputHandle As Long
    Dim ret As Long
    Dim State As Boolean
    Dim DeviceHandle(20) As Long
    outputHandle = GetStdHandle(OUTPUT_HANDLE)
    
    ret = USB_ScanDevice(VarPtr(DeviceHandle(0)))
    If ret <= 0 Then
        str = "No device connected" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    State = USB_OpenDevice(DeviceHandle(0))
    If State Then
        str = "Open device success!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Dim CANConfig As CAN_INIT_CONFIG
    '0-Normal Mode,1-Loop back mode
    CANConfig.CAN_Mode = 1
    CANConfig.CAN_ABOM = 0
    CANConfig.CAN_NART = 1
    CANConfig.CAN_RFLM = 0
    CANConfig.CAN_TXFP = 1
    'Baud Rate = 100M/(BRP*(SJW+BS1+BS2))
    CANConfig.CAN_BRP = 25
    CANConfig.CAN_BS1 = 2
    CANConfig.CAN_BS2 = 1
    CANConfig.CAN_SJW = 1
    ret = CAN_Init(DeviceHandle(0), 0, CANConfig)
    If ret <> CAN_SUCCESS Then
        str = "Init device faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Dim CANFilter As CAN_FILTER_CONFIG
    CANFilter.Enable = 1
    CANFilter.ExtFrame = 0
    CANFilter.FilterIndex = 0
    CANFilter.FilterMode = 0
    CANFilter.MASK_IDE = 0
    CANFilter.MASK_RTR = 0
    CANFilter.MASK_Std_Ext = 0
    ret = CAN_Filter_Init(DeviceHandle(0), 0, CANFilter)
    If ret <> CAN_SUCCESS Then
        str = "Config filter faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Dim CanMsg(5) As CAN_MSG
    Dim i As Long
    For i = 0 To 4
        CanMsg(i).ExternFlag = 0
        CanMsg(i).RemoteFlag = 0
        CanMsg(i).ID = i
        CanMsg(i).DataLen = 8
        Dim j As Long
        For j = 0 To 7
            CanMsg(i).Data(j) = j
        Next j
    Next i
    
    Dim SendedNum As Long
    SendedNum = CAN_SendMsg(DeviceHandle(0), 0, VarPtr(CanMsg(0)), 5)
    If SendedNum >= 0 Then
        str = "Success send frames:" + CStr(SendedNum) + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        str = "Send can msg error!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Call Sleep(1000)
    
    Dim CanMsgBuffer(10240) As CAN_MSG
    Dim GetCanNum As Long
    GetCanNum = CAN_GetMsg(DeviceHandle(0), 0, VarPtr(CanMsgBuffer(0)))
    str = "Success Get frames:" + CStr(GetCanNum) + vbNewLine
    Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    For i = 0 To (GetCanNum - 1)
        str = vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        
        str = "CanMsg[" + CStr(i) + "].ID = " + CStr(CanMsgBuffer(i).ID) + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        
        str = "CanMsg[" + CStr(i) + "].TimeStamp = " + CStr(CanMsgBuffer(i).TimeStamp) + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        
        str = "CanMsg[" + CStr(i) + "].Data = "
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        
        For j = 0 To (CanMsgBuffer(i).DataLen - 1)
            str = CStr(CanMsgBuffer(i).Data(j)) + " "
            Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        Next j
        str = vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Next i
    
    USB_CloseDevice (DeviceHandle(0))
    
    inputHandle = GetStdHandle(INPUT_HANDLE)
    Call ReadConsole(inputHandle, vbNull, 255, vbNull, vbNull)
    
    Call FreeConsole
End Sub
