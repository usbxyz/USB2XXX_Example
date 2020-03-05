Attribute VB_Name = "usb2adc"

Public Const ADC_SUCCESS             As Long = (0)
Public Const ADC_ERR_NOT_SUPPORT     As Long = (-1)
Public Const ADC_ERR_USB_WRITE_FAIL  As Long = (-2)
Public Const ADC_ERR_USB_READ_FAIL   As Long = (-3)
Public Const ADC_ERR_CMD_FAIL        As Long = (-4)
Public Const ADC_ERR_CH_NO_INIT      As Long = (-5)

Declare Function ADC_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal Channel As Byte,ByVal SampleRateHz As Long) As Long
Declare Function ADC_Read Lib "USB2XXX.dll" (ByVal DevHandle As Long,ByVal pData As Any,ByVal DataNum As Long) As Long

