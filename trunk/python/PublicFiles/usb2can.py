"""
文件说明：USB2XXX CAN操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# 1.CAN信息帧的数据类型定义
class CAN_MSG(Structure):
    _fields_ = [
        ("ID",c_uint),              # 报文ID。
        ("TimeStamp",c_uint),       # 接收到信息帧时的时间标识，从CAN 控制器初始化开始计时。
        ("RemoteFlag",c_ubyte),      # 是否是远程帧
        ("ExternFlag",c_ubyte),      # 是否是扩展帧
        ("DataLen",c_ubyte),         # 数据长度(<=8)，即Data 的长度。
        ("Data",c_ubyte*8),          # 报文的数据。
    ]

# 2.初始化CAN的数据类型定义
class CAN_INIT_CONFIG(Structure):
    _fields_ = [
        # CAN波特率 = 100MHz/(CAN_BRP)/(CAN_SJW+CAN_BS1+CAN_BS2)
        ("CAN_BRP_CFG3",c_uint),    # 取值范围1~1024
        ("CAN_SJW",c_ubyte),    # 取值范围1~4
        ("CAN_BS1_CFG1",c_ubyte),    # 取值范围1~16
        ("CAN_BS2_CFG2",c_ubyte),    # 取值范围1~8
        ("CAN_Mode",c_ubyte),   # CAN工作模式，0-正常模式，1-环回模式，2-静默模式，3-静默环回模式
        ("CAN_ABOM",c_ubyte),   # 自动离线管理，0-禁止，1-使能
        ("CAN_NART",c_ubyte),   # 报文重发管理，0-使能报文重传，1-禁止报文重传
        ("CAN_RFLM",c_ubyte),   # FIFO锁定管理，0-新报文覆盖旧报文，1-丢弃新报文
        ("CAN_TXFP",c_ubyte),   # 发送优先级管理，0-标识符决定，1-发送请求顺序决定
    ]
# 3.CAN 滤波器设置数据类型定义
class CAN_FILTER_CONFIG(Structure):
    _fields_ = [
        ("Enable",c_ubyte),          # 使能该过滤器，1-使能，0-禁止
        ("FilterIndex",c_ubyte),     # 过滤器索引号，取值范围为0到13
        ("FilterMode",c_ubyte),      # 过滤器模式，0-屏蔽位模式，1-标识符列表模式
        ("ExtFrame",c_ubyte),        # 过滤的帧类型标志，为1 代表要过滤的为扩展帧，为0 代表要过滤的为标准帧。
        ("ID_Std_Ext",c_uint),      # 验收码ID
        ("ID_IDE",c_uint),          # 验收码IDE
        ("ID_RTR",c_uint),          # 验收码RTR
        ("MASK_Std_Ext",c_uint),    # 屏蔽码ID，该项只有在过滤器模式为屏蔽位模式时有用
        ("MASK_IDE",c_uint),        # 屏蔽码IDE，该项只有在过滤器模式为屏蔽位模式时有用
        ("MASK_RTR",c_uint),        # 屏蔽码RTR，该项只有在过滤器模式为屏蔽位模式时有用
    ]
# 4.CAN总线状态数据类型定义
class CAN_STATUS(Structure):
    _fields_ = [
        ("TSR",c_uint),
        ("ESR",c_uint),
        ("RECounter",c_ubyte),    # CAN 控制器接收错误寄存器。
        ("TECounter",c_ubyte),    # CAN 控制器发送错误寄存器。
        ("LECode",c_ubyte),       # 最后的错误代码
    ]
# 5.定义CAN Bootloader命令列表
class CBL_CMD_LIST(Structure):
    _fields_ = [
        # Bootloader相关命令
        ("Erase",c_ubyte),            # 擦出APP储存扇区数据
        ("WriteInfo",c_ubyte),        # 设置多字节写数据相关参数（写起始地址，数据量）
        ("Write",c_ubyte),            # 以多字节形式写数据
        ("Check",c_ubyte),            # 检测节点是否在线，同时返回固件信息
        ("SetBaudRate",c_ubyte),      # 设置节点波特率
        ("Excute",c_ubyte),           # 执行固件
        # 节点返回状态
        ("CmdSuccess",c_ubyte),       # 命令执行成功
        ("CmdFaild",c_ubyte),         # 命令执行失败
    ]
# 6.函数返回错误代码定义
CAN_SUCCESS             = (0)   # 函数执行成功
CAN_ERR_NOT_SUPPORT     = (-1)  # 适配器不支持该函数
CAN_ERR_USB_WRITE_FAIL  = (-2)  # USB写数据失败
CAN_ERR_USB_READ_FAIL   = (-3)  # USB读数据失败
CAN_ERR_CMD_FAIL        = (-4)  # 命令执行失败
CAN_BL_ERR_CONFIG       = (-20) # 配置设备错误
CAN_BL_ERR_SEND         = (-21) # 发送数据出错
CAN_BL_ERR_TIME_OUT     = (-22) # 超时错误
CAN_BL_ERR_CMD          = (-23) # 执行命令失败

# 7.CAN Bootloader固件类型
CAN_BL_BOOT     = 0x55555555
CAN_BL_APP      = 0xAAAAAAAA

# 8.MCP芯片波特率参数配置定义
MCP_16MHz_1000kBPS_CFG1 = (0x00)
MCP_16MHz_1000kBPS_CFG2 = (0xD0)
MCP_16MHz_1000kBPS_CFG3 = (0x82)

MCP_16MHz_500kBPS_CFG1 = (0x00)
MCP_16MHz_500kBPS_CFG2 = (0xF0)
MCP_16MHz_500kBPS_CFG3 = (0x86)

MCP_16MHz_250kBPS_CFG1 = (0x41)
MCP_16MHz_250kBPS_CFG2 = (0xF1)
MCP_16MHz_250kBPS_CFG3 = (0x85)

MCP_16MHz_200kBPS_CFG1 = (0x01)
MCP_16MHz_200kBPS_CFG2 = (0xFA)
MCP_16MHz_200kBPS_CFG3 = (0x87)

MCP_16MHz_125kBPS_CFG1 = (0x03)
MCP_16MHz_125kBPS_CFG2 = (0xF0)
MCP_16MHz_125kBPS_CFG3 = (0x86)

MCP_16MHz_100kBPS_CFG1 = (0x03)
MCP_16MHz_100kBPS_CFG2 = (0xFA)
MCP_16MHz_100kBPS_CFG3 = (0x87)

MCP_16MHz_95kBPS_CFG1 = (0x03)
MCP_16MHz_95kBPS_CFG2 = (0xAD)
MCP_16MHz_95kBPS_CFG3 = (0x07)

MCP_16MHz_83k3BPS_CFG1 = (0x03)
MCP_16MHz_83k3BPS_CFG2 = (0xBE)
MCP_16MHz_83k3BPS_CFG3 = (0x07)

MCP_16MHz_80kBPS_CFG1 = (0x03)
MCP_16MHz_80kBPS_CFG2 = (0xFF)
MCP_16MHz_80kBPS_CFG3 = (0x87)

MCP_16MHz_50kBPS_CFG1 = (0x07)
MCP_16MHz_50kBPS_CFG2 = (0xFA)
MCP_16MHz_50kBPS_CFG3 = (0x87)

MCP_16MHz_40kBPS_CFG1 = (0x07)
MCP_16MHz_40kBPS_CFG2 = (0xFF)
MCP_16MHz_40kBPS_CFG3 = (0x87)

MCP_16MHz_33kBPS_CFG1 = (0x09)
MCP_16MHz_33kBPS_CFG2 = (0xBE)
MCP_16MHz_33kBPS_CFG3 = (0x07)

MCP_16MHz_31k25BPS_CFG1 = (0x0F)
MCP_16MHz_31k25BPS_CFG2 = (0xF1)
MCP_16MHz_31k25BPS_CFG3 = (0x85)

MCP_16MHz_25kBPS_CFG1 = (0X0F)
MCP_16MHz_25kBPS_CFG2 = (0XBA)
MCP_16MHz_25kBPS_CFG3 = (0X07)

MCP_16MHz_20kBPS_CFG1 = (0x0F)
MCP_16MHz_20kBPS_CFG2 = (0xFF)
MCP_16MHz_20kBPS_CFG3 = (0x87)

MCP_16MHz_10kBPS_CFG1 = (0x1F)
MCP_16MHz_10kBPS_CFG2 = (0xFF)
MCP_16MHz_10kBPS_CFG3 = (0x87)

MCP_16MHz_5kBPS_CFG1 = (0x3F)
MCP_16MHz_5kBPS_CFG2 = (0xFF)
MCP_16MHz_5kBPS_CFG3 = (0x87)

MCP_16MHz_666kBPS_CFG1 = (0x00)
MCP_16MHz_666kBPS_CFG2 = (0xA0)
MCP_16MHz_666kBPS_CFG3 = (0x04)

def CAN_Init(DevHandle, CANIndex, pCanConfig):
    return USB2XXXLib.CAN_Init(DevHandle, CANIndex, pCanConfig)

def CAN_Filter_Init(DevHandle, CANIndex, pFilterConfig):
    return USB2XXXLib.CAN_Filter_Init(DevHandle, CANIndex, pFilterConfig)

def CAN_SendMsg(DevHandle, CANIndex, pCanSendMsg,SendMsgNum):
    return USB2XXXLib.CAN_SendMsg(DevHandle, CANIndex, pCanSendMsg,SendMsgNum)

def CAN_GetMsg(DevHandle, CANIndex, pCanGetMsg):
    return USB2XXXLib.CAN_GetMsg(DevHandle, CANIndex, pCanGetMsg)

def CAN_GetStatus(DevHandle, CANIndex, pCANStatus):
    return USB2XXXLib.CAN_GetStatus(DevHandle, CANIndex, pCANStatus)

def CAN_BL_Init(DevHandle,CANIndex,pInitConfig,pCmdList):
    return USB2XXXLib.CAN_BL_Init(DevHandle,CANIndex,pInitConfig,pCmdList)

def CAN_BL_NodeCheck(DevHandle,CANIndex,NodeAddr,pVersion,pType,TimeOut):
    return USB2XXXLib.CAN_BL_NodeCheck(DevHandle,CANIndex,NodeAddr,pVersion,pType,TimeOut)

def CAN_BL_Erase(DevHandle,CANIndex,NodeAddr,FlashSize,TimeOut):
    return USB2XXXLib.CAN_BL_Erase(DevHandle,CANIndex,NodeAddr,FlashSize,TimeOut)

def CAN_BL_Write(DevHandle,CANIndex,NodeAddr,AddrOffset,pData,DataNum,TimeOut):
    return USB2XXXLib.CAN_BL_Write(DevHandle,CANIndex,NodeAddr,AddrOffset,pData,DataNum,TimeOut)

def CAN_BL_Excute(DevHandle,CANIndex,NodeAddr,Type):
    return USB2XXXLib.CAN_BL_Excute(DevHandle,CANIndex,NodeAddr,Type)

def CAN_BL_SetNewBaudRate(DevHandle,CANIndex,NodeAddr,pInitConfig,NewBaudRate,TimeOut):
    return USB2XXXLib.CAN_BL_SetNewBaudRate(DevHandle,CANIndex,NodeAddr,pInitConfig,NewBaudRate,TimeOut)
