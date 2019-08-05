Attribute VB_Name = "usb2ads1256"

Public Const ADS1256_SUCCESS As Long = (0)
Public Const ADS1256_ERR_NOT_SUPPORT As Long = (-1)
Public Const ADS1256_ERR_USB_WRITE_FAIL As Long = (-2)
Public Const ADS1256_ERR_USB_READ_FAIL As Long = (-3)
Public Const ADS1256_ERR_CMD_FAIL As Long = (-4)
Public Const ADS1256_ERR_CH_NO_INIT As Long = (-5)

Public Const ADS1256_DRATE_30000SPS   As Long = &HF0
Public Const ADS1256_DRATE_15000SPS   As Long = &HE0
Public Const ADS1256_DRATE_7500SPS   As Long = &HD0
Public Const ADS1256_DRATE_3750SPS   As Long = &HC0
Public Const ADS1256_DRATE_2000SPS   As Long = &HB0
Public Const ADS1256_DRATE_1000SPS   As Long = &HA1
Public Const ADS1256_DRATE_500SPS    As Long = &H92
Public Const ADS1256_DRATE_100SPS    As Long = &H82
Public Const ADS1256_DRATE_60SPS     As Long = &H72
Public Const ADS1256_DRATE_50SPS     As Long = &H63
Public Const ADS1256_DRATE_30SPS     As Long = &H53
Public Const ADS1256_DRATE_25SPS     As Long = &H43
Public Const ADS1256_DRATE_15SPS     As Long = &H33
Public Const ADS1256_DRATE_10SPS     As Long = &H23
Public Const ADS1256_DRATE_5SPS      As Long = &H13
Public Const ADS1256_DRATE_2_5SPS    As Long = &H3

Public Type ADS1256_CONFIG
    SampleRate As Integer
    PGA As Byte
    BufferEn As Byte
    SampleMode As Long
End Type

Declare Function ADS1256_Init Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal DevHandle As Byte, ByRef pConfig As ADS1256_CONFIG) As Long
Declare Function ADS1256_Start Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal DevHandle As Byte) As Long
Declare Function ADS1256_Read Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal DevHandle As Byte, ByVal pData As Any) As Long
Declare Function ADS1256_Stop Lib "USB2XXX.dll" (ByVal DevHandle As Long, ByVal DevHandle As Byte) As Long
