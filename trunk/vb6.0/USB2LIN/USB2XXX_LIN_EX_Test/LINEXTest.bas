Attribute VB_Name = "LINEXTest"
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
    Call SetConsoleTitle("USB2LIN_EX Test")
    
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
    
    ret = LIN_EX_Init(DeviceHandle(0), 0, 19200, LIN_EX_MASTER)
    If ret <> LIN_EX_SUCCESS Then
        str = "Config LIN faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        str = "Config LIN success!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    'Master Send Data
    Dim LIN_IN_Msg As LIN_EX_MSG
    Dim LIN_OUT_Msg As LIN_EX_MSG
    LIN_IN_Msg.PID = 1
    LIN_IN_Msg.MsgType = LIN_EX_MSG_TYPE_MW
    LIN_IN_Msg.CheckType = LIN_EX_CHECK_EXT
    LIN_IN_Msg.Timestamp = 10
    LIN_IN_Msg.DataLen = 8
    Dim j As Long
    For j = 0 To 7
        LIN_IN_Msg.Data(j) = j
    Next j
    ret = LIN_EX_MasterSync(DeviceHandle(0), 0, VarPtr(LIN_IN_Msg), VarPtr(LIN_OUT_Msg), 1)
    If ret <> 1 Then
        str = "Send LIN Data Faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        str = "Send LIN Data success!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    'Master Read Data
    LIN_IN_Msg.PID = 2
    LIN_IN_Msg.MsgType = LIN_EX_MSG_TYPE_MR
    LIN_IN_Msg.Timestamp = 10
    ret = LIN_EX_MasterSync(DeviceHandle(0), 0, VarPtr(LIN_IN_Msg), VarPtr(LIN_OUT_Msg), 1)
    If ret <> 1 Then
        str = "Read LIN Data Faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        str = "LIN_OUT_Msg.PID = " + CStr(LIN_OUT_Msg.PID) + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        
        str = "LIN_OUT_Msg.Data = "
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        
        For j = 0 To (LIN_OUT_Msg.DataLen - 1)
            str = CStr(LIN_OUT_Msg.Data(j)) + " "
            Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        Next j
        str = vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    USB_CloseDevice (DeviceHandle(0))
    inputHandle = GetStdHandle(INPUT_HANDLE)
    Call ReadConsole(inputHandle, vbNull, 255, vbNull, vbNull)
    Call FreeConsole
End Sub

