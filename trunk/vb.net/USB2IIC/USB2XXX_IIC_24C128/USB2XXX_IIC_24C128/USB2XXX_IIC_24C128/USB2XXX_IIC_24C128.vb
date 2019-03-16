Module USB2XXX_IIC_24C128
	'若运行程序提示找不到USB2XXX.dll文件，请将USB2XXX.dll文件和libusb-1.0.dll文件拷贝到exe程序输出目录下，比如bin/Debug目录下
    Sub Main()
        Dim ret As Int32
        Dim DevIndex As Int32
        Dim IICIndex As Int32
        Dim State As Boolean
        Dim DeviceHandle(20) As UInt32
        Dim timer As New Stopwatch()
        '扫描设备，必须调用
        ret = USB_ScanDevice(DeviceHandle)
        If ret <= 0 Then
            Console.WriteLine("No device connected")
            Return
        End If
        '打开设备，必须调用
        DevIndex = 0
        IICIndex = 0    '使用IIC0通道
        State = USB_OpenDevice(DeviceHandle(DevIndex))
        If State Then
            Console.WriteLine("Open device success!")
        End If
        '初始化I2C
        Dim I2C_Config As IIC_CONFIG
        I2C_Config.AddrBits = 7
        I2C_Config.ClockSpeedHz = 400000
        I2C_Config.EnablePu = 1
        I2C_Config.Master = 1
        I2C_Config.OwnAddr = &H70
        ret = IIC_Init(DeviceHandle(DevIndex), IICIndex, I2C_Config)
        If ret <> IIC_SUCCESS Then
            System.Console.WriteLine("初始化IIC出错！")
            Return
        End If
        '准备写入EEPROM芯片中的数据，可以在这里增加读取文件程序，将文件中的数据读入数组中
        Dim WriteDataBuffer(16 * 1024 - 1) As Byte
        Dim PageSize As Int32 = 64  '芯片页大小，每次写数据最大数据量不能超过该大小
        Dim i As Integer
        Dim Addr As UInt16 = 0
        For i = 0 To (WriteDataBuffer.Length - 1)
            WriteDataBuffer(i) = CType(Math.Round(Rnd() * 255), Byte)    '这里用随机数填充数据缓冲区
        Next i
        '循环将数据写入芯片
        Dim WriteDataBufferTemp(PageSize + 2 - 1) As Byte
        System.Console.WriteLine("开始循环写数据...")
        timer.Start()
        For i = 0 To (WriteDataBuffer.Length / PageSize) - 1
            Addr = i * PageSize '写入数据起始地址
            WriteDataBufferTemp(0) = (Addr >> 8) And &HFF   '内部存储区地址高8位
            WriteDataBufferTemp(1) = Addr And &HFF          '内部存储区地址低8位
            '循环将数据缓冲区中的数据拷贝到该数组中
            For j = 0 To (PageSize - 1)
                WriteDataBufferTemp(2 + j) = WriteDataBuffer(Addr + j)
            Next j
            '将数据写入芯片
            ret = IIC_WriteBytes(DeviceHandle(DevIndex), IICIndex, &H50, WriteDataBufferTemp, WriteDataBufferTemp.Length, 100)
            If ret <> IIC_SUCCESS Then
                System.Console.WriteLine("写EEPROM数据出错！" + ret.ToString)
                Return
            Else
                System.Console.WriteLine("写入数据成功，起始地址：" + Addr.ToString)
            End If
            '延时一段时间，否则可能下次写入数据会失败
            Threading.Thread.Sleep(10)
        Next i
        timer.Stop()
        System.Console.WriteLine("写数据耗时(秒)：" + (timer.ElapsedMilliseconds / 1000.0).ToString() + " 秒")
        '读出数据并校验
        System.Console.WriteLine("开始循环读数据...")
        timer.Restart()
        Dim ReadDataBuffer(16 * 1024 - 1) As Byte
        Addr = 0    '从0地址开始读数据
        WriteDataBufferTemp(0) = (Addr >> 8) And &HFF   '内部存储区地址高8位
        WriteDataBufferTemp(1) = Addr And &HFF          '内部存储区地址低8位
        '该适配器每次I2C读取数据可以达20K字节，由于芯片容量小于20K，所以这里可以一次性读出所有数据，不过这样做这个函数耗时可能会比较多，也可以考虑用循环的方式读出数据
        ret = IIC_WriteReadBytes(DeviceHandle(DevIndex), IICIndex, &H50, WriteDataBufferTemp, 2, ReadDataBuffer, ReadDataBuffer.Length, 100)
        If ret <> IIC_SUCCESS Then
            System.Console.WriteLine("读EEPROM数据出错！")
            Return
        Else
            System.Console.WriteLine("读出数据成功，开始校验...")
        End If
        '循环校验数据
        Dim ErrorDataNum As Integer = 0
        For i = 0 To (WriteDataBuffer.Length - 1)
            If WriteDataBuffer(i) <> ReadDataBuffer(i) Then
                ErrorDataNum += 1
                System.Console.WriteLine("地址：" + i.ToString + "，写入：" + Hex(WriteDataBuffer(i)) + "，读出" + Hex(ReadDataBuffer(i)))
            End If
        Next i
        System.Console.WriteLine("数据校验完毕，错误的数据字节数为：" + ErrorDataNum.ToString)
        timer.Stop()
        System.Console.WriteLine("读数据加校验耗时(秒)：" + (timer.ElapsedMilliseconds / 1000.0).ToString() + " 秒")
    End Sub

End Module
