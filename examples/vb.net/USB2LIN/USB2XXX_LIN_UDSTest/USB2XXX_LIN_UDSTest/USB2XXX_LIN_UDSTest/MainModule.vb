Module MainModule
    '若运行程序提示找不到USB2XXX.dll文件，请将USB2XXX.dll文件和libusb-1.0.dll文件拷贝到exe程序输出目录下，比如bin/Debug目录下
    Sub Main()
        Dim ret As Int32
        Dim DevIndex As Int32
        Dim State As Boolean
        Dim DeviceHandle(20) As UInt32
        Dim LINIndex As Byte
        LINIndex = 0
        DevIndex = 0
        '扫描当前连接到电脑的LIN适配器
        ret = USB_ScanDevice(DeviceHandle)
        If ret <= 0 Then
            Console.WriteLine("No device connected")
        End If
        '打开设备
        State = USB_OpenDevice(DeviceHandle(DevIndex))
        If State Then
            Console.WriteLine("Open device success!")
        Else
            Console.WriteLine("Open device failed!")
            Return
        End If
        '初始化配置LIN
        ret = LIN_EX_Init(DeviceHandle(DevIndex), LINIndex, 19200, LIN_EX_MASTER)
        If ret <> LIN_EX_SUCCESS Then
            Console.WriteLine("Config LIN failed!")
            Return
        Else
            Console.WriteLine("Config LIN success!")
        End If
        '发送命令
        Dim UDSAddr As LIN_UDS_ADDR
        UDSAddr.CheckType = 0
        UDSAddr.NAD = &H7F
        UDSAddr.ReqID = &H3C
        UDSAddr.ResID = &H3D
        UDSAddr.STmin = 10
        Dim reqCmds(20) As Byte
        reqCmds(0) = &H10
        For i = 0 To reqCmds.Length - 2
            reqCmds(i + 1) = i
        Next i
        ret = LIN_UDS_Request(DeviceHandle(DevIndex), LINIndex, UDSAddr, reqCmds, reqCmds.Length)
        If ret <> LIN_UDS_OK Then
            Console.WriteLine("LIN UDS Request failed! {0}", ret)
            Return
        Else
            Console.Write("reqCmds =  ", ret)
            For i = 0 To reqCmds.Length - 1
                Console.Write("0x{0:X2} ", reqCmds(i))
            Next i
            Console.WriteLine()
        End If
        '读取从机响应
        Dim resCmds(64) As Byte '为防止缓冲区溢出，建议把接收响应数据的缓冲区设置大一点
        ret = LIN_UDS_Response(DeviceHandle(DevIndex), LINIndex, UDSAddr, resCmds, 100)
        If ret > LIN_UDS_OK Then
            Console.Write("resCmds =  ", ret)
            For i = 0 To ret - 1
                Console.Write("0x{0:X2} ", resCmds(i))
            Next i
            Console.WriteLine()
        Else
            Console.WriteLine("LIN UDS Response failed! {0}", ret)
        End If
        '读取前面步骤交互时收发的LIN消息
        Dim LINMsgBuffer(1024) As LIN_EX_MSG '为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
        ret = LIN_UDS_GetMsgFromUDSBuffer(DeviceHandle(DevIndex), LINIndex, LINMsgBuffer, LINMsgBuffer.Length)
        If ret < LIN_EX_SUCCESS Then
            Console.WriteLine("Get LIN Msg error!{0}", ret)
            Return
        ElseIf ret > 0 Then
            For i = 0 To ret - 1
                Console.Write("LIN MSG[{0}] : ID = 0x{1:X2} Data = ", i, LINMsgBuffer(i).PID And &H3F)
                If LINMsgBuffer(i).DataLen > 0 Then
                    Dim j As Int32
                    For j = 0 To LINMsgBuffer(i).DataLen - 1
                        Console.Write("0x{0:X2} ", LINMsgBuffer(i).Data(j))
                    Next j
                End If
                Console.WriteLine("")
            Next i
        Else
            Console.WriteLine("No LIN Msg")
        End If
    End Sub

End Module
