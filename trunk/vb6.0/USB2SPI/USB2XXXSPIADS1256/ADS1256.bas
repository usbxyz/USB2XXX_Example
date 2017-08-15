Attribute VB_Name = "SPIADS1256"

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
    Call SetConsoleTitle("USB2XXX ADS1256")
    
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
    
    Dim ADS1256Config As ADS1256_CONFIG
    ADS1256Config.BufferEn = 0
    ADS1256Config.PGA = 0
    ADS1256Config.SampleRate = ADS1256_DRATE_1000SPS
    ADS1256Config.SampleMode = &H1
    ret = ADS1256_Init(DeviceHandle(0), 0, ADS1256Config)
    If ret <> ADS1256_SUCCESS Then
        str = "Initialize ADS1256 error!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    ret = ADS1256_Start(DeviceHandle(0), 0)
    If ret <> ADS1256_SUCCESS Then
        str = "Start ADS1256 error!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Sleep (10)
    
    Dim DataBuffer(20 * 1024) As Long
    Dim DataNum As Long
    DataNum = 0
    ret = ADS1256_Read(DeviceHandle(0), 0, VarPtr(DataBuffer(0)))
    If ret <> ADS1256_SUCCESS Then
        str = "Read ADS1256 error!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        DataNum = DataNum + ret
    End If
    
    Sleep (10)
    ret = ADS1256_Read(DeviceHandle(0), 0, VarPtr(DataBuffer(DataNum)))
    If ret <> ADS1256_SUCCESS Then
        str = "Read ADS1256 error!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        DataNum = DataNum + ret
    End If
    
    ret = ADS1256_Stop(DeviceHandle(0), 0)
    If ret <> ADS1256_SUCCESS Then
        str = "Stop ADS1256 error!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    For i = 0 To (DataNum - 1)
        str = CStr(ReadBuffer(i)) + " "
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Next i
    
    USB_CloseDevice (DeviceHandle(0))
    
    inputHandle = GetStdHandle(INPUT_HANDLE)
    Call ReadConsole(inputHandle, vbNull, 255, vbNull, vbNull)
    Call FreeConsole
End Sub



