using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace USB2XXX
{
    class LDFDecoder
    {
        public const Int32 LDF_SUCCESS = 0;
        public const Int32 LDF_ERROR_FILE_OPEN = (-1);//打开文件出错
        public const Int32 LDF_ERROR_FILE_FORMAT = (-2);//文件格式错误
        public const Int32 LDF_ERROR_DEV_DISCONNECT = (-3);//设备未连接
        //逻辑类型编码值
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct LDF_CODING_LOGICAL
        {
            [MarshalAs(UnmanagedType.U4)]
            public UInt32 RawValue;
            public IntPtr pDescription;
        }
        //物理类型编码值
        struct LDF_CODING_PHYSICAL
        {
            public float Offset;
            public float Factor;
            public Int32 Minimum;
            public Int32 Maximum;
            public IntPtr pUnit;
        }
        //信号中的值类型
        public enum LDF_SIGNAL_VALUE_TYPE
        {
            LDF_SIGNAL_VALUE_TYPE_SCALAR = 0,   //标量值
            LDF_SIGNAL_VALUE_TYPE_ARRAY = 1     //数组值
        }
        //信号值编码类型
        public enum LDF_SIGNAL_CODING_TYPE
        {
            LDF_SIGNAL_CODING_TYPE_NONE = 0,
            LDF_SIGNAL_CODING_TYPE_PHYSICAL = 1,
            LDF_SIGNAL_CODING_TYPE_LOGICAL = 2
        }
        //信号类型
        public enum LDF_SIGNAL_TYPE
        {
            LDF_SIGNAL_TYPE_NORMAL = 0,
            LDF_SIGNAL_TYPE_DIAGNOSTIC = 1
        }
        //帧类型
        public enum LDF_FRAME_TYPE
        {
            LDF_FRAME_TYPE_NORMAL = 0,
            LDF_FRAME_TYPE_DIAGNOSTIC = 1
        }
        //信号编码
        public struct LDF_SIGNAL_CODING
        {
            public IntPtr pName;
            public Int32 LogicalValueLen;
            public Int32 PhysicalValueLen;
            public IntPtr pLogicalValues;
            public IntPtr pPhysicalValues;
        }
        //信号和信号编码对应值
        public struct LDF_SIGNAL_REPRESENTATION
        {
            public IntPtr pSignalName;
            public IntPtr pCodingName;
        }
        
        public struct LDF_SIGNAL_VALUE
        {
            public UInt64 ScalarValue;
            [ MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public Byte[] ArrayValue;
        }
        //信号编码
        public struct LDF_LIN_SIGNAL
        {
            public IntPtr pName;       //信号名称
            public Int32 Size;           //单位为bit
            public LDF_SIGNAL_TYPE Type;   //普通信号或者诊断信号
            public LDF_SIGNAL_VALUE_TYPE ValueType;//数值或者数组
            public LDF_SIGNAL_VALUE Value;
            public IntPtr pPublisher;
            public Int32 SubscriberLen;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16, ArraySubType = UnmanagedType.U4)]
            public IntPtr[] pSubscribers;
            public Int32 Offset; //该信号在8字节数据中的偏移量
            public LDF_SIGNAL_CODING SignalCodings;
        }
        //帧，每帧数据包含多个信号
        public struct LDF_LIN_FRAME
        {
            public IntPtr pName;         //帧名称
            public Byte ID;               //帧ID
            public IntPtr pPublisher;    //发布者名称
            public Byte Size;             //帧长度，单位为字节
            public LDF_FRAME_TYPE Type;
            public Int32 SignalListLen;
            public IntPtr pSignalList;  //信号列表LDF_LIN_SIGNAL
        }

        public struct LDF_LIN_NODE
        {
            public IntPtr pMasterName;
            public float time_base;
            public float jitter;
            public Int32 SlaveLen;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16, ArraySubType = UnmanagedType.U4)]
            public IntPtr[] pSlaveNames;
        }

        public struct LDF_LIN_SCH_CMD
        {
            public LDF_LIN_FRAME Frame;
            public float DelayOfMs;                //延时时间，单位为毫秒
        }

        public struct LDF_LIN_SCH
        {
            public IntPtr pName;             //调度表名称
            public Int32 ScheduleCmdLen;                 //该表下面包含的帧数
            public IntPtr pScheduleCmdList;//改表下面的帧列表
        }

        public struct LDF_NODE_ATTR
        {
            public IntPtr pName;
            public UInt16 LIN_protocol;
            public Byte configured_NAD;
            public Byte initial_NAD;
            public struct product_id
            {
                UInt32 supplier_id;
                UInt32 function_id;
                UInt32 variant;
            };
            public IntPtr response_error;//signal_name
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
            public IntPtr[] fault_state_signals;
            public Int32 fault_state_signal_len;
            public float P2_min;
            public float ST_min;
            public float N_As_timeout;
            public float N_Cr_timeout;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 64)]
            public IntPtr[] configurable_frames;
            public Int32 configurable_frame_len;
        }

        public struct LDF_CODING
        {
            public Int32 ProtocolVersion;
            public Int32 LanguageVersion;
            public Int32 LINSpeed;
            public IntPtr pNode;
            public Int32 FrameLen;
            public IntPtr pFrames;
            public Int32 NodeAttrLen;
            public IntPtr pNodeAttrs;
            public Int32 LINSchLen;
            public IntPtr pLINSch;
        }
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LDF_Decode(Int32 DevHandle,string pLDFFileName,ref LDF_CODING pLDFCoding);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LDF_DataRawToFrame(ref LDF_LIN_FRAME  pFrame,Byte[] pRawData);
        [DllImport("USB2XXX.dll")]
        public static extern Int32 LDF_DataFrameToRaw(ref LDF_LIN_FRAME  pFrame,Byte[] pRawData);
    }
}
