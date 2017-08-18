Attribute VB_Name = "USB2XXXSPITest"

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
    Call SetConsoleTitle("USB2XXX SPI Test")
    
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
    
    Dim SPIConfig As SPI_CONFIG
    SPIConfig.Mode = 0
    SPIConfig.ClockSpeedHz = 50000000
    SPIConfig.CPHA = 0
    SPIConfig.CPOL = 0
    SPIConfig.LSBFirst = 0
    SPIConfig.Master = 1
    SPIConfig.SelPolarity = 0
    ret = SPI_Init(DeviceHandle(0), 0, SPIConfig)
    If ret <> SPI_SUCCESS Then
        str = "Config SPI faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Dim WriteBuffer(16) As Byte
    Dim i As Long
    For i = 0 To 15
        WriteBuffer(i) = CByte(i)
    Next i
    ret = SPI_WriteBytes(DeviceHandle(0), 0, VarPtr(WriteBuffer(0)), 16)
    If ret <> SPI_SUCCESS Then
        str = "SPI Write Data faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    End If
    
    Dim ReadBuffer(16) As Byte
    ret = SPI_ReadBytes(DeviceHandle(0), 0, VarPtr(ReadBuffer(0)), 16)
    If ret <> SPI_SUCCESS Then
        str = "SPI Read Data faild!" + vbNewLine
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
    Else
        str = "Read Data:"
        Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        For i = 0 To 15
            str = CStr(ReadBuffer(i)) + " "
            Call WriteConsole(outputHandle, ByVal str, Len(str), vbNull, vbNull)
        Next i
    End If
    
    USB_CloseDevice (DeviceHandle(0))
    
    inputHandle = GetStdHandle(INPUT_HANDLE)
    Call ReadConsole(inputHandle, vbNull, 255, vbNull, vbNull)
    Call FreeConsole
End Sub



