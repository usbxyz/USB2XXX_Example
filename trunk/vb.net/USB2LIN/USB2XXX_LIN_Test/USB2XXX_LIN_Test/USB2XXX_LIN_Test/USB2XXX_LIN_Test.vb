
#Const LIN_MASTER = True

Module USB2XXX_LIN_Test

    Sub Main()
        Dim ret As Int32
        Dim State As Boolean
        Dim DeviceHandle(20) As UInt32
        Dim LINIndex As Byte
        LINIndex = 0
        ret = USB_ScanDevice(DeviceHandle)
        If ret <= 0 Then
            Console.WriteLine("No device connected")
        End If

        State = USB_OpenDevice(DeviceHandle(0))
        If State Then
            Console.WriteLine("Open device success!")
        Else
            Console.WriteLine("Open device failed!")
            Return
        End If
        '初始化配置LIN
        Dim LINConfig As usb2lin.LIN_CONFIG
        LINConfig.BaudRate = 19200
        LINConfig.BreakBits = usb2lin.LIN_BREAK_BITS_11
        LINConfig.CheckMode = usb2lin.LIN_CHECK_MODE_EXT
#If LIN_MASTER Then
        LINConfig.MasterMode = usb2lin.LIN_MASTER
#Else
        LINConfig.MasterMode = USB2LIN.LIN_SLAVE
#End If
        ret = usb2lin.LIN_Init(DeviceHandle(0), LINIndex, LINConfig)
        If ret <> usb2lin.LIN_SUCCESS Then
            Console.WriteLine("Config LIN failed!")
            Return
        Else
            Console.WriteLine("Config LIN success!")
        End If

#If LIN_MASTER Then
        '主机写数据
        Dim ID As Byte
        For ID = 0 To 9
            Dim msg(1) As usb2lin.LIN_MSG
            Dim i As Byte
            ReDim msg(0)
            ReDim msg(0).Data(8)
            For i = 0 To 7
                msg(0).Data(i) = ID + i
            Next
            msg(0).DataLen = 8
            msg(0).ID = ID
            ret = usb2lin.LIN_Write(DeviceHandle(0), LINIndex, msg, 1)
            If ret <> usb2lin.LIN_SUCCESS Then
                Console.WriteLine("LIN write data failed!")
                Return
            Else
                Console.WriteLine("LIN write data success!")
            End If
            '延时
            Threading.Thread.Sleep(10)
        Next ID
        '主机读数据
        For ID = 10 To 15
            Dim msg(1) As usb2lin.LIN_MSG
            ReDim msg(0)
            msg(0).ID = ID
            ReDim msg(0).Data(8)
            ret = usb2lin.LIN_Read(DeviceHandle(0), LINIndex, msg, 1)
            If ret < usb2lin.LIN_SUCCESS Then
                Console.WriteLine("LIN write data failed!")
                Return
            Else
                Console.Write("Master LIN Read: ID = 0x{0:X2} Data = ", msg(0).ID)
                If msg(0).DataLen > 0 Then
                    Dim i As Byte
                    For i = 0 To msg(0).DataLen - 1
                        Console.Write("0x{0:X2} ", msg(0).Data(i))
                    Next i
                End If
                Console.WriteLine("")
            End If
            '延时
            Threading.Thread.Sleep(10)
        Next ID
#Else
        '设置从机模式下所有ID都为从接收数据模式，这样就可以获取到主机发送过来的所有数据,初始化配置为从机后，默认所有ID都为接收数据模式，所以若是监听LIN总线数据，这个函数可以不用调用
        Dim LINSlaveData(64) As usb2lin.LIN_MSG
        Dim i As Int32
        For i = 0 To 63
            ReDim LINSlaveData(i)
            LINSlaveData(i).DataLen = 9 '最大8Byte数据+1Byte和校验
            LINSlaveData(i).ID = i      'ID值
            ReDim LINSlaveData(i).Data(8)
        Next i
        ret = usb2lin.LIN_SlaveSetIDMode(DeviceHandle(0), LINIndex, usb2lin.LIN_SLAVE_READ, LINSlaveData, 64)
        If ret <> usb2lin.LIN_SUCCESS Then
            Console.WriteLine("Set LIN operation mode failed!")
            Return
        Else
            Console.WriteLine("Set LIN operation mode success!")
        End If
        '从机接收数据,若要连续不断的监控LIN总线数据，可以循环调用LIN_SlaveGetData函数
        Dim LINSlaveDataBuffer(1024) As usb2lin.LIN_MSG '为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
        For i = 0 To LINSlaveDataBuffer.Length - 1
            ReDim LINSlaveDataBuffer(i)
            ReDim LINSlaveDataBuffer(i).Data(8)
        Next

        While True
            ret = usb2lin.LIN_SlaveGetData(DeviceHandle(0), LINIndex, LINSlaveDataBuffer)
            If ret < usb2lin.LIN_SUCCESS Then
                Console.WriteLine("LIN slave read data error!")
                Return
            ElseIf ret > 0 Then
                Console.WriteLine("LIN slave read data:")
                For i = 0 To ret - 1
                    Console.Write("Data[{0}]: ", i)
                    Console.Write("ID = 0x{0:X2} ", LINSlaveDataBuffer(i).ID)
                    Console.Write("Data = ")
                    Dim j As Int32
                    For j = 0 To LINSlaveDataBuffer(i).DataLen - 1
                        Console.Write("0x{0:X2} ", LINSlaveDataBuffer(i).Data(j))
                    Next j
                Next i
                Console.WriteLine("")
            Else
                'Console.WriteLine("LIN slave read no data!")
                '延时
                Threading.Thread.Sleep(10)
            End If
        End While
#End If

    End Sub

End Module
