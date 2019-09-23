
#Const LIN_MASTER = True

Module USB2XXX_LIN_Test
    '若运行程序提示找不到USB2XXX.dll文件，请将USB2XXX.dll文件和libusb-1.0.dll文件拷贝到exe程序输出目录下，比如bin/Debug目录下
    Sub Main()
        Dim ret As Int32
        Dim State As Boolean
        Dim DeviceHandle(20) As UInt32
        Dim LINIndex As Byte
        LINIndex = 0
        '扫描当前连接到电脑的LIN适配器
        ret = USB_ScanDevice(DeviceHandle)
        If ret <= 0 Then
            Console.WriteLine("No device connected")
        End If
        '打开设备
        State = USB_OpenDevice(DeviceHandle(0))
        If State Then
            Console.WriteLine("Open device success!")
        Else
            Console.WriteLine("Open device failed!")
            Return
        End If
        '初始化配置LIN
#If LIN_MASTER Then
        ret = LIN_EX_Init(DeviceHandle(0), LINIndex, 19200, LIN_EX_MASTER)
#Else
        ret = LIN_EX_Init(DeviceHandle(0), LINIndex, 19200, LIN_EX_SLAVE)
#End If
        If ret <> LIN_EX_SUCCESS Then
            Console.WriteLine("Config LIN failed!")
            Return
        Else
            Console.WriteLine("Config LIN success!")
        End If

#If LIN_MASTER Then
        '主机循环写数据
        Dim ID As Byte
        For ID = 0 To 9
            Dim msg_len As Byte = 1
            Dim msg_in(msg_len - 1) As LIN_EX_MSG
            Dim msg_out(msg_len - 1) As LIN_EX_MSG
            Dim i As Byte
            ReDim msg_in(0)
            ReDim msg_in(0).Data(8)
            ReDim msg_out(0)
            ReDim msg_out(0).Data(8)
            For i = 0 To 7
                msg_in(0).Data(i) = ID + i  '具体数据根据实际情况修改
            Next
            msg_in(0).DataLen = 8   '指定需要发送的数据字节数
            msg_in(0).PID = ID      '传入ID即可，底层会自动计算校验位
            msg_in(0).CheckType = LIN_EX_CHECK_EXT  '校验类型
            msg_in(0).MsgType = LIN_EX_MSG_TYPE_MW  '消息类型，主机写数据
            msg_in(0).Timestamp = 0                 '数据发送完毕后不需要延时
            ret = LIN_EX_MasterSync(DeviceHandle(0), LINIndex, msg_in, msg_out, msg_len)
            If ret <> msg_len Then
                Console.WriteLine("LIN write data failed!")
                Return
            Else
                '通过对输出的消息进行判断可以知道发送出去的数据是否成功发送
                Console.Write("Master LIN Write: PID[ID] = 0x{0:X2}[0x{1:X2}] Data = ", msg_out(0).PID, msg_out(0).PID And &H3F)
                If msg_out(0).DataLen > 0 Then
                    For i = 0 To msg_out(0).DataLen - 1
                        Console.Write("0x{0:X2} ", msg_out(0).Data(i))
                    Next i
                End If
                Console.WriteLine("")
            End If
            '延时
            Threading.Thread.Sleep(10)
        Next ID
        '主机循环读数据
        For ID = 10 To 15
            Dim msg_len As Byte = 1
            Dim msg_in(msg_len - 1) As LIN_EX_MSG
            Dim msg_out(msg_len - 1) As LIN_EX_MSG
            Dim i As Byte
            ReDim msg_in(0)
            ReDim msg_in(0).Data(8)
            ReDim msg_out(0)
            ReDim msg_out(0).Data(8)
            msg_in(0).PID = ID  '传入ID即可，底层会自动计算校验位
            msg_in(0).MsgType = LIN_EX_MSG_TYPE_MW  '消息类型，主机读数据
            ReDim msg_in(0).Data(8)
            ret = LIN_EX_MasterSync(DeviceHandle(0), LINIndex, msg_in, msg_out, msg_len)
            If ret <> msg_len Then
                Console.WriteLine("LIN read data failed!")
                Return
            Else
                '主机读到的数据存放在数据消息中
                Console.Write("Master LIN Read : PID[ID] = 0x{0:X2}[0x{1:X2}] Data = ", msg_out(0).PID, msg_out(0).PID And &H3F)
                If msg_out(0).DataLen > 0 Then  '具体读到了多少字节数据，可以通过DataLen参数获取
                    For i = 0 To msg_out(0).DataLen - 1
                        Console.Write("0x{0:X2} ", msg_out(0).Data(i))
                    Next i
                End If
                Console.WriteLine("")
            End If
            '延时
            Threading.Thread.Sleep(10)
        Next ID
#Else
        '设置从机模式下所有ID都为从接收数据模式，这样就可以获取到主机发送过来的所有数据,初始化配置为从机后，默认所有ID都为接收数据模式，所以若是监听LIN总线数据，这个函数可以不用调用
        Dim LINSlaveData(64) As LIN_EX_MSG
        Dim i As Int32
        For i = 0 To 63
            ReDim LINSlaveData(i)
            LINSlaveData(i).PID = i      'ID值
            LINSlaveData(i).MsgType = LIN_EX_MSG_TYPE_SR 'LIN_EX_MSG_TYPE_SR：从机接收数据模式，配置为从机后所有ID默认为该模式
            ReDim LINSlaveData(i).Data(8)
        Next i
        ret = LIN_EX_SlaveSetIDMode(DeviceHandle(0), LINIndex, LINSlaveData, 64)
        If ret <> LIN_EX_SUCCESS Then
            Console.WriteLine("Set LIN operation mode failed!")
            Return
        Else
            Console.WriteLine("Set LIN operation mode success!")
        End If
        '从机接收数据,若要连续不断的监控LIN总线数据，可以循环调用LIN_SlaveGetData函数
        Dim LINSlaveDataBuffer(1024) As LIN_EX_MSG '为了防止缓冲区溢出，可以将接收数据缓冲区设置大一点
        For i = 0 To LINSlaveDataBuffer.Length - 1
            ReDim LINSlaveDataBuffer(i)
            ReDim LINSlaveDataBuffer(i).Data(8)
        Next
        Console.WriteLine("LIN slave start get data...")
        While True
            ret = LIN_EX_SlaveGetData(DeviceHandle(0), LINIndex, LINSlaveDataBuffer)
            If ret < LIN_EX_SUCCESS Then
                Console.WriteLine("LIN slave read data error!")
                Return
            ElseIf ret > 0 Then
                For i = 0 To ret - 1
                    Console.Write("Slave LIN Read : PID[ID] = 0x{0:X2}[0x{1:X2}] Data = ", LINSlaveDataBuffer(i).PID, LINSlaveDataBuffer(i).PID And &H3F)
                    If LINSlaveDataBuffer(i).DataLen > 0 Then
                        Dim j As Int32
                        For j = 0 To LINSlaveDataBuffer(i).DataLen - 1
                            Console.Write("0x{0:X2} ", LINSlaveDataBuffer(i).Data(j))
                        Next j
                    End If
                    Console.WriteLine("")
                Next i
            Else
                'Console.WriteLine("LIN slave read no data!")
                '延时
                Threading.Thread.Sleep(100)
            End If
        End While
#End If

    End Sub

End Module
