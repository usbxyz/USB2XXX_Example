"""
文件说明：USB2XXX LIN LDF文件解析相关函数集合
更多帮助：www.toomoss.com
"""
from ctypes import *
import platform
from usb_device import *
from enum import IntEnum

# Define the types we need.
class CtypesEnum(IntEnum):
    """A ctypes-compatible IntEnum superclass."""
    @classmethod
    def from_param(cls, obj):
        return int(obj)

# 定义函数返回错误代码
LDF_SUCCESS               =  0   # 操作成功
LDF_ERROR_FILE_OPEN       =  (-1)# 打开文件出错
LDF_ERROR_FILE_FORMAT     =  (-2)# 文件格式错误
LDF_ERROR_DEV_DISCONNECT  =  (-3)# 设备未连接

# 逻辑类型编码值
class LDF_CODING_LOGICAL(Structure):
    _fields_ = [
        ("RawValue",c_int),        # 实际值
        ("pDescription",c_char_p)  # 逻辑值编码字符串
    ]


# 物理类型编码值，信号物理值=信号实际值*Factor+Offset
class LDF_CODING_PHYSICAL(Structure):
    _fields_ = [
        ("Offset",c_float),  # 信号物理值偏移量
        ("Factor",c_float),  # 信号缩放因子
        ("Minimum",c_int),   # 信号实际最小值，注意不是物理值
        ("Maximum",c_int),   # 信号实际最大值，注意不是物理值
        ("pUnit",c_char_p)   # 信号物理值单位
    ]


# 信号中的值类型
class LDF_SIGNAL_VALUE_TYPE(CtypesEnum):
    LDF_SIGNAL_VALUE_TYPE_SCALAR = 0 #标量值
    LDF_SIGNAL_VALUE_TYPE_ARRAY = 1  #数组值

# 信号值编码类型
class LDF_SIGNAL_CODING_TYPE(CtypesEnum):
    LDF_SIGNAL_CODING_TYPE_NONE = 0
    LDF_SIGNAL_CODING_TYPE_PHYSICAL = 1 # 物理值
    LDF_SIGNAL_CODING_TYPE_LOGICAL = 2  # 逻辑值


# 信号类型
class LDF_SIGNAL_TYPE(CtypesEnum):
    LDF_SIGNAL_TYPE_NORMAL = 0
    LDF_SIGNAL_TYPE_DIAGNOSTIC = 1


# 帧类型
class LDF_FRAME_TYPE(CtypesEnum):
    LDF_FRAME_TYPE_NORMAL = 0
    LDF_FRAME_TYPE_DIAGNOSTIC = 1


# 信号编码
class LDF_SIGNAL_CODING(Structure):
    _fields_ = [
        ("pName",c_char_p),                     # 信号编码名称
        ("LogicalValueLen",c_int),              # 逻辑值长度
        ("PhysicalValueLen",c_int),             # 物理值长度
        ("pLogicalValues",LDF_CODING_LOGICAL),  # 逻辑值列表指针
        ("pPhysicalValues",LDF_CODING_PHYSICAL) # 物理值列表指针
    ]



# 信号和信号编码对应值
class LDF_SIGNAL_REPRESENTATION(Structure):
    _fields_ = [
        ("pSignalName",c_char_p),# 信号名称
        ("pCodingName",c_char_p) # 编码名称
    ]

# 信号值结构体，结构体里面的值二取一，并不是同时存在
class LDF_SIGNAL_VALUE(Structure):
    _fields_ = [
        ("ScalarValue",c_longlong),# 物理值
        ("ArrayValue",c_ubyte*8)   # 数组
    ]


# 信号编码
class LDF_LIN_SIGNAL(Structure):
    _fields_ = [
        ("pName",c_char_p),                 # 信号名称
        ("Size",c_int),                     # 信号大小，单位为bit
        ("Type",LDF_SIGNAL_TYPE),           # 普通信号或者诊断信号
        ("ValueType",LDF_SIGNAL_VALUE_TYPE),# 数值或者数组
        ("Value",LDF_SIGNAL_VALUE),         # 信号值
        ("pPublisher",c_char_p),            # 信号发布者名称，可能为主机或者从机
        ("SubscriberLen",c_int),            # 接收该信号节点长度
        ("pSubscribers",c_char_p*16),       # 接收该信号的节点名称数组
        ("Offset",c_int),                   # 该信号在8字节数据中的偏移量
        ("SignalCodings",LDF_SIGNAL_CODING) # 信号编码
    ]


# 帧，每帧数据包含多个信号
class LDF_LIN_FRAME(Structure):
    _fields_ = [
        ("pName",c_char_p),             # 帧名称
        ("ID",c_ubyte),                 # 帧ID
        ("pPublisher",c_char_p),        # 发布者名称
        ("Size",c_ubyte),               # 帧长度，单位为字节
        ("Type",LDF_FRAME_TYPE),        # 帧类型
        ("SignalListLen",c_int),        # 信号长度
        ("pSignalList",LDF_LIN_SIGNAL)  # 信号指针
    ]


# 节点结构体
class LDF_LIN_NODE(Structure):
    _fields_ = [
        ("pMasterName",c_char_p),    # 主机名称
        ("time_base",c_float),              
        ("jitter",c_float),        
        ("SlaveLen",c_int),          # 从机数量
        ("pSlaveNames",c_char_p*16)  # 从机名称数组
    ]


# 调度表命令
class LDF_LIN_SCH_CMD(Structure):
    _fields_ = [
        ("Frame",LDF_LIN_FRAME),    # LIN帧
        ("DelayOfMs",c_float)       # 延时时间，单位为毫秒       
    ]


# 调度表
class LDF_LIN_SCH(Structure):
    _fields_ = [
        ("pName",c_char_p),                 # 调度表名称
        ("ScheduleCmdLen",c_int)            # 该表下面包含的帧数
        ("pScheduleCmdList",LDF_LIN_SCH_CMD)# 该表下面的帧列表
    ]


# 节点属性
typedef struct _LDF_NODE_ATTR{
    char *pName;
    unsigned short LIN_protocol;
    unsigned char configured_NAD;
    unsigned char initial_NAD;
    struct {
        unsigned int supplier_id;
        unsigned int function_id;
        unsigned int variant;
    }product_id;
    char *response_error;//signal_name
    char *fault_state_signals[64];
    int fault_state_signal_len;
    float P2_min;
    float ST_min;
    float N_As_timeout;
    float N_Cr_timeout;
    char *configurable_frames[64];
    int configurable_frame_len;
}LDF_NODE_ATTR;

//解析后的LDF文件内容
typedef struct _LDF_CODING{
    int ProtocolVersion;        //LIN protocol version,实际版本号的一百倍值，比如当前值为210，那么实际版本为2.1
    int LanguageVersion;        //LIN language version,实际版本号的一百倍值，比如当前值为210，那么实际版本为2.1
    int LINSpeed;               //LIN总线波特率，单位为bps
    LDF_LIN_NODE *pNode;        //节点指针
    int FrameLen;               //帧长度
    LDF_LIN_FRAME *pFrames;     //帧指针
    int NodeAttrLen;            //节点属性长度
    LDF_NODE_ATTR *pNodeAttrs;  //节点属性指针
    int LINSchLen;              //调度表长度
    LDF_LIN_SCH *pLINSch;       //调度表指针
}LDF_CODING;

#ifdef __cplusplus
extern "C"
{
#endif
    int WINAPI LDF_Decode(int DevHandle,char *pLDFFileName,LDF_CODING *pLDFCoding);
    int WINAPI LDF_DataRawToFrame(LDF_LIN_FRAME *pFrame,unsigned char *pRawData);
    int WINAPI LDF_DataFrameToRaw(LDF_LIN_FRAME *pFrame,unsigned char *pRawData);
#ifdef __cplusplus
}
#endif

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