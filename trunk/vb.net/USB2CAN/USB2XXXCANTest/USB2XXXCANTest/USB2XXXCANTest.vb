Module USB2XXXCANTest

    Sub Main()
        Dim ret As Int32
        Dim State As Boolean
        Dim DeviceHandle(20) As UInt32

        ret = USB_ScanDevice(DeviceHandle)
        If ret <= 0 Then
            Console.WriteLine("No device connected")
        End If

        State = USB_OpenDevice(DeviceHandle(0))
        If State Then
            Console.WriteLine("Open device success!")
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
            Console.WriteLine("Init device faild!")
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
            Console.WriteLine("Config filter faild!")
        End If

        Dim CanMsg(5) As CAN_MSG
        Dim i As Long
        For i = 0 To 4
            CanMsg(i).ExternFlag = 0
            CanMsg(i).RemoteFlag = 0
            CanMsg(i).ID = i
            CanMsg(i).DataLen = 8
            Dim j As Long
            ReDim CanMsg(i).Data(7)
            For j = 0 To 7
                CanMsg(i).Data(j) = j
            Next j
        Next i

        Dim SendedNum As Long
        SendedNum = CAN_SendMsg(DeviceHandle(0), 0, CanMsg, 5)
        If SendedNum >= 0 Then
            Console.WriteLine("Success send frames:" + CStr(SendedNum))
        Else
            Console.WriteLine("Send can msg error!")
        End If

        Threading.Thread.Sleep(100)

        Dim CanMsgBuffer(10240) As CAN_MSG
        Dim GetCanNum As Long
        GetCanNum = CAN_GetMsg(DeviceHandle(0), 0, CanMsgBuffer)
        Console.WriteLine("Success Get frames:" + CStr(GetCanNum))
        For i = 0 To (GetCanNum - 1)
            Console.WriteLine("")
            Console.WriteLine("CanMsg[" + CStr(i) + "].ID = " + CStr(CanMsgBuffer(i).ID))
            Console.WriteLine("CanMsg[" + CStr(i) + "].TimeStamp = " + CStr(CanMsgBuffer(i).TimeStamp))
            Console.Write("CanMsg[" + CStr(i) + "].Data = ")
            For j = 0 To (CanMsgBuffer(i).DataLen - 1)
                Console.Write(CStr(CanMsgBuffer(i).Data(j)) + " ")
            Next j
            Console.WriteLine("")
        Next i

        USB_CloseDevice(DeviceHandle(0))
    End Sub

End Module
