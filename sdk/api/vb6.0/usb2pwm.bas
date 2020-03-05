Attribute VB_Name = "usb2pwm"
Public Const PWM_SUCCESS             As Long = (0)
Public Const PWM_ERR_NOT_SUPPORT     As Long = (-1)
Public Const PWM_ERR_USB_WRITE_FAIL  As Long = (-2)
Public Const PWM_ERR_USB_READ_FAIL   As Long = (-3)
Public Const PWM_ERR_CMD_FAIL        As Long = (-4)

Public Type PWM_CONFIG
    Prescaler(7) As Int '预分频器
    Precision(7) As Int '占空比调节精度,实际频率 = 168MHz/(Prescaler*Precision)
    Pulse(7) As Int     '占空比，实际占空比=(Pulse/Precision)*100%
    Polarity(7) As Byte '波形极性，取值0或者1
    ChannelMask As Byte '通道号，若要使能某个通道，则对应位为1，最低位对应通道0
End Type

Declare Function Lib "USB2XXX.dll" PWM_Init(ByVal DevHandle As Long, ByRef pConfig As PWM_CONFIG) As Long
Declare Function Lib "USB2XXX.dll" PWM_Start(ByVal DevHandle As Long,ByVal ChannelMask As Byte,ByVal RunTimeUs As Long) As Long
Declare Function Lib "USB2XXX.dll" PWM_SetPulse(ByVal DevHandle As Long,ByVal ChannelMask As Byte,ByVal pPulse As Any) As Long
Declare Function Lib "USB2XXX.dll" PWM_SetPhase(ByVal DevHandle As Long,ByVal ChannelMask As Byte,ByVal pPhase As Any) As Long
Declare Function Lib "USB2XXX.dll" PWM_SetFrequency(ByVal DevHandle As Long,ByVal ChannelMask As Byte,ByVal pPrescaler As Any,ByVal pPrecision As Any) As Long
Declare Function Lib "USB2XXX.dll" PWM_Stop(ByVal DevHandle As Long,ByVal ChannelMask As Byte);
Declare Function Lib "USB2XXX.dll" PWM_SendPulses(ByVal DevHandle As Long,ByVal PinMask As Long,ByVal PulseWidthUs As Long,ByVal PulsePeriodUs As Long,ByVal PulseNum As Long) As Long

