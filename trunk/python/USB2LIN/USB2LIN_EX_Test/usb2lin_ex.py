"""
文件说明：USB2XXX LIN操作相关函数集合
更多帮助：www.usbxyz.com
"""
from ctypes import *
import platform
from usb_device import *

# 定义函数返回错误代码
LIN_EX_SUCCESS            = (0)   # 函数执行成功
LIN_EX_ERR_NOT_SUPPORT    = (-1)  # 适配器不支持该函数
LIN_EX_ERR_USB_WRITE_FAIL = (-2)  # USB写数据失败
LIN_EX_ERR_USB_READ_FAIL  = (-3)  # USB读数据失败
LIN_EX_ERR_CMD_FAIL       = (-4)  # 命令执行失败
LIN_EX_ERR_CH_NO_INIT     = (-5)  # 该通道未初始化
LIN_EX_ERR_READ_DATA      = (-6)  # LIN读数据失败
# LIN和校验模式
LIN_EX_CHECK_STD     = 0    # 标准校验，不含PID
LIN_EX_CHECK_EXT     = 1    # 增强校验，包含PID
LIN_EX_CHECK_USER    = 2    # 自定义校验类型，需要用户自己计算并传入Check，不进行自动校验
LIN_EX_CHECK_NONE    = 3    # 数据没有校验
LIN_EX_CHECK_ERROR   = 4    # 接收数据校验错误

# 定义主从模式
LIN_EX_MASTER         = 1
LIN_EX_SLAVE          = 0

# 定义消息类型
LIN_EX_MSG_TYPE_UN     = 0  # 未知类型
LIN_EX_MSG_TYPE_MW	   = 1	# 主机向从机发送数据
LIN_EX_MSG_TYPE_MR	   = 2	# 主机向从机读取数据
LIN_EX_MSG_TYPE_SW     = 3	# 从机发送数据
LIN_EX_MSG_TYPE_SR     = 4	# 从机接收数据
LIN_EX_MSG_TYPE_BK     = 5	# 只发送BREAK信号，若是反馈回来的数据，表明只检测到BREAK信号
LIN_EX_MSG_TYPE_SY     = 6	# 反馈回来的数据，表明检测到了BREAK，SYNC信号
LIN_EX_MSG_TYPE_ID     = 7	# 反馈回来的数据，表明检测到了BREAK，SYNC，PID信号
LIN_EX_MSG_TYPE_DT     = 8	# 反馈回来的数据，表明检测到了BREAK，SYNC，PID,DATA信号
LIN_EX_MSG_TYPE_CK     = 9	# 反馈回来的数据，表明检测到了BREAK，SYNC，PID,DATA,CHECK信号

# 定义初始化LIN初始化数据类型
class LIN_CONFIG(Structure):
    _fields_ = [
        ("BaudRate",c_uint),      # 波特率,最大20K
        ("CheckMode",c_ubyte),    # 校验模式，0-标准校验模式，1-增强校验模式（包含PID）
        ("MasterMode",c_ubyte),   # 主从模式，0-从模式，1-主模式
        ("BreakBits",c_ubyte),    # Break长度，0x00-10bit,0x20-11bit
    ]
# 设置从模式下ID操作模式
class LIN_EX_MSG(Structure):
    _fields_ = [
        ("Timestamp",c_uint),   #接收数据表示时间戳，单位为100us；发送数据表示数据发送后的延时时间，单位为毫秒
        ("MsgType",c_ubyte),    #帧类型
        ("CheckType",c_ubyte),  #数据校验类型
        ("DataLen",c_ubyte),    #数据字节数
        ("Sync",c_ubyte),       #同步数据，固定值0x55，若接收数据时该值不为0x55，那么数据有可能有错
        ("PID",c_ubyte),        #接收数据表示带校验位的ID值，发送数据的时候只需要传递ID值即可，底层会自动计算校验位并添加上去
        ("Data",c_ubyte*8),     #接收或者发送的数据
        ("Check",c_ubyte),      #根据CheckType校验类型进行计算的校验数据，发送数据时若不是自定义校验类型则底层会自动计算
    ]
def LIN_EX_Init(DevHandle,LINIndex,BaudRate,MasterMode):
    return USB2XXXLib.LIN_EX_Init(DevHandle,LINIndex,BaudRate,MasterMode)

def LIN_EX_MasterSync(DevHandle,LINIndex,pInMsg,pOutMsg,MsgLen):
    return USB2XXXLib.LIN_EX_MasterSync(DevHandle,LINIndex,pInMsg,pOutMsg,MsgLen)

def LIN_EX_SlaveGetIDMode(DevHandle,LINIndex,pLINMsg,MsgLen):
    return USB2XXXLib.LIN_EX_SlaveGetIDMode(DevHandle,LINIndex,pLINMsg,MsgLen)

def LIN_EX_SlaveSetIDMode(DevHandle,LINIndex,pLINMsg,MsgLen):
    return USB2XXXLib.LIN_EX_SlaveSetIDMode(DevHandle,LINIndex,pLINMsg,MsgLen)

def LIN_EX_SlaveGetData(DevHandle,LINIndex,pLINMsg):
    return USB2XXXLib.LIN_EX_SlaveGetData(DevHandle,LINIndex,pLINMsg)

def LIN_EX_CtrlPowerOut(DevHandle,State):
    return USB2XXXLib.LIN_EX_CtrlPowerOut(DevHandle,State)

def LIN_EX_GetVbatValue(DevHandle,pBatValue):
    return USB2XXXLib.LIN_EX_GetVbatValue(DevHandle,pBatValue)

def LIN_EX_MasterStartSch(DevHandle,LINIndex,pLINMsg,MsgLen):
    return USB2XXXLib.LIN_EX_MasterStartSch(DevHandle,LINIndex,pLINMsg,MsgLen)

def LIN_EX_MasterStopSch(DevHandle,LINIndex):
    return USB2XXXLib.LIN_EX_MasterStopSch(DevHandle,LINIndex)

def LIN_EX_MasterGetSch(DevHandle,LINIndex,pLINMsg):
    return USB2XXXLib.LIN_EX_MasterGetSch(DevHandle,LINIndex,pLINMsg)

def LIN_EX_MasterOfflineSch(DevHandle,LINIndex,BaudRate,pLINMsg,MsgLen):
    return USB2XXXLib.LIN_EX_MasterOfflineSch(DevHandle,LINIndex,BaudRate,pLINMsg,MsgLen)