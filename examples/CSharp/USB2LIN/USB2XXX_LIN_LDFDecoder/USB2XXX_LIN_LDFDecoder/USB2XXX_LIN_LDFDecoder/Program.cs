#define PRINT_LDF_INFO
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Diagnostics;
using System.Text;
using USB2XXX;

namespace USB2XXX_LIN_LDFDecoder
{
    class Program
    {
        static void Main(string[] args)
        {
            Int32[] DevHandles = new Int32[20];
            Int32 DevHandle = 0;
            Byte LINIndex = 0;
            bool state;
            Int32 DevNum, ret=0;
            LDFDecoder.LDF_CODING LDFCoding = new LDFDecoder.LDF_CODING();
            //LDFCoding.pNode[0].pSlaveNames = new StringBuilder[16];
            string[] MSGTypeStr={"UN","MW","MR","SW","SR","BK","SY","ID","DT","CK"};
            //扫描查找设备
            DevNum = USB_DEVICE.USB_ScanDevice(DevHandles);
            if (DevNum <= 0)
            {
                Console.WriteLine("No device connected!");
                return;
            }
            else
            {
                Console.WriteLine("Have {0} device connected!", DevNum);
            }
            DevHandle = DevHandles[0];
            //打开设备
            state = USB_DEVICE.USB_OpenDevice(DevHandle);
            if (!state)
            {
                Console.WriteLine("Open device error!");
                return;
            }
            else
            {
                Console.WriteLine("Open device success!");
            }
            //ldf文件解析
            ret = LDFDecoder.LDF_Decode(DevHandle, "Door_Protection_System_20190710.ldf", ref LDFCoding);
            if (ret != LDFDecoder.LDF_SUCCESS)
            {
                Console.WriteLine("LDF decode failed");
            }
            //获取帧
            int LINFrameLen = LDFCoding.FrameLen;
            LDFDecoder.LDF_LIN_FRAME[] LINFrames = new LDFDecoder.LDF_LIN_FRAME[LINFrameLen];
#if PRINT_LDF_INFO
            Console.WriteLine("LINFrameLen = "+string.Format("{0}", LINFrameLen));
            for (int i = 0; i < LINFrameLen; i++)
            {
                LINFrames[i] = (LDFDecoder.LDF_LIN_FRAME)Marshal.PtrToStructure((IntPtr)((UInt32)LDFCoding.pFrames + i * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_FRAME))), typeof(LDFDecoder.LDF_LIN_FRAME));
                Console.WriteLine("Name = " + Marshal.PtrToStringAnsi(LINFrames[i].pName));
                Console.WriteLine("--ID = 0x"+string.Format("{0:X2}", LINFrames[i].ID));
                Console.WriteLine("--Publisher = "+Marshal.PtrToStringAnsi(LINFrames[i].pPublisher));
                Console.WriteLine(string.Format("--Size = {0} Bytes", LINFrames[i].Size));
                Console.WriteLine(string.Format("--SignalListLen = {0}", LINFrames[i].SignalListLen));
                LDFDecoder.LDF_LIN_SIGNAL[] SignalList = new LDFDecoder.LDF_LIN_SIGNAL[LINFrames[i].SignalListLen];
                for (int j = 0; j < LINFrames[i].SignalListLen; j++)
                {
                    SignalList[j] = (LDFDecoder.LDF_LIN_SIGNAL)Marshal.PtrToStructure((IntPtr)((UInt32)LINFrames[i].pSignalList + j * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_SIGNAL))), typeof(LDFDecoder.LDF_LIN_SIGNAL));
                    Console.WriteLine("--SignalName = "+ Marshal.PtrToStringAnsi(SignalList[j].pName));
                    Console.WriteLine(string.Format("----Size = {0} bits", SignalList[j].Size));
                    Console.WriteLine(string.Format("----Offset = {0} ", SignalList[j].Offset));
                    Console.Write("----InitValue = ");
                    if (SignalList[j].ValueType == LDFDecoder.LDF_SIGNAL_VALUE_TYPE.LDF_SIGNAL_VALUE_TYPE_SCALAR)
                    {
                        Console.WriteLine(string.Format("{0}", SignalList[j].Value.ScalarValue));
                    }
                    else
                    {
                        for (int ii = 0; ii < (SignalList[j].Size / 8); ii++)
                        {
                            Console.Write(string.Format("0x{0:X2} ", SignalList[j].Value.ArrayValue[ii]));
                        }
                        Console.WriteLine("");
                    }
                    Console.WriteLine("----Publisher = "+Marshal.PtrToStringAnsi(SignalList[j].pPublisher));
                    Console.Write("----Subscribers = ");

                    for (int ii = 0; ii < SignalList[j].SubscriberLen; ii++)
                    {
                        Console.Write(string.Format("{0} ",Marshal.PtrToStringAnsi(SignalList[j].pSubscribers[ii])));
                    }
                    Console.WriteLine("");
                    if((Int32)SignalList[j].SignalCodings.pName != 0){
                        if(Marshal.PtrToStringAnsi(SignalList[j].SignalCodings.pName).Length > 0){
                            Console.WriteLine("----SignalCodings = "+ Marshal.PtrToStringAnsi(SignalList[j].SignalCodings.pName));
                        }
                    }
                    Console.WriteLine("");
                }
            }
#endif
            //获取调度表
            int SchTableLen = LDFCoding.LINSchLen;
            LDFDecoder.LDF_LIN_SCH[] SchTables = new LDFDecoder.LDF_LIN_SCH[SchTableLen];
#if PRINT_LDF_INFO
            Console.WriteLine(string.Format("SchTableLen = {0}", SchTableLen));
            for (int i = 0; i < SchTableLen; i++)
            {
                SchTables[i] = (LDFDecoder.LDF_LIN_SCH)Marshal.PtrToStructure((IntPtr)((UInt32)LDFCoding.pLINSch + i * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_SCH))), typeof(LDFDecoder.LDF_LIN_SCH));
                Console.Write("{0}{"+ Marshal.PtrToStringAnsi(SchTables[i].pName));
                LDFDecoder.LDF_LIN_SCH_CMD[] SCHCmds = new LDFDecoder.LDF_LIN_SCH_CMD[SchTables[i].ScheduleCmdLen];
                for (int j = 0; j < SchTables[i].ScheduleCmdLen; j++)
                {
                    SCHCmds[j] = (LDFDecoder.LDF_LIN_SCH_CMD)Marshal.PtrToStructure((IntPtr)((UInt32)SchTables[i].pScheduleCmdList + j * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_SCH_CMD))), typeof(LDFDecoder.LDF_LIN_SCH_CMD));
                    Console.Write(Marshal.PtrToStringAnsi(SCHCmds[j].Frame.pName)+string.Format(" delay {0} ms;", SCHCmds[j].DelayOfMs));
                }
                Console.WriteLine("}");
            }
#endif
            //获取节点信息
            LDFDecoder.LDF_LIN_NODE Node = (LDFDecoder.LDF_LIN_NODE)Marshal.PtrToStructure(LDFCoding.pNode, typeof(LDFDecoder.LDF_LIN_NODE));
#if PRINT_LDF_INFO
            Console.WriteLine("Master=" + Marshal.PtrToStringAnsi(Node.pMasterName) + string.Format(" time_base ={0} ms,jitter={1} ms", Node.time_base, Node.jitter));
            Console.Write("Slaves=");
            for (int i = 0; i < Node.SlaveLen; i++)
            {
                Console.Write(Marshal.PtrToStringAnsi(Node.pSlaveNames[i])+" ");
            }
            Console.WriteLine("");
#endif
            //初始化LIN适配器
            ret = USB2LIN_EX.LIN_EX_Init(DevHandle, LINIndex, 9600, 1);
            if(ret != USB2LIN_EX.LIN_EX_SUCCESS){
                Console.WriteLine("Config LIN failed!");
                return;
            }else{
                Console.WriteLine("Config LIN Success!");
            }
            Console.WriteLine(string.Format("SchTableLen = {0}",SchTableLen));
            //执行调度表
            for(int i=0;i<SchTableLen;i++){
                for(int j=0;j<SchTables[i].ScheduleCmdLen;j++){
                    LDFDecoder.LDF_LIN_SCH_CMD SCHCmds = (LDFDecoder.LDF_LIN_SCH_CMD)Marshal.PtrToStructure((IntPtr)((UInt32)SchTables[i].pScheduleCmdList + j * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_SCH_CMD))), typeof(LDFDecoder.LDF_LIN_SCH_CMD));
                    LDFDecoder.LDF_LIN_FRAME LINFrame = SCHCmds.Frame;
                    USB2LIN_EX.LIN_EX_MSG[] LINMsg = new USB2LIN_EX.LIN_EX_MSG[1];
                    USB2LIN_EX.LIN_EX_MSG[] LINOutMsg = new USB2LIN_EX.LIN_EX_MSG[10];
                    if((Int32)LINFrame.pName != 0){//确保当前帧是有效帧
                        if((Marshal.PtrToStringAnsi(LINFrame.pName).Equals("MasterReq")) || ((LINFrame.Type == LDFDecoder.LDF_FRAME_TYPE.LDF_FRAME_TYPE_NORMAL) && (Marshal.PtrToStringAnsi(LINFrame.pPublisher).Equals(Marshal.PtrToStringAnsi(Node.pMasterName)))))
                        {
                            //该帧为主机发送数据帧
                            if (LINFrame.Type == LDFDecoder.LDF_FRAME_TYPE.LDF_FRAME_TYPE_NORMAL)
                            {
                                LINMsg[0].CheckType = USB2LIN_EX.LIN_EX_CHECK_EXT;//可以根据ldf文件中的版本号来设置，大于等于2.0的一般都是增强校验，小于2.0的一般都是标准校验
                            }else{
                                LINMsg[0].CheckType = USB2LIN_EX.LIN_EX_CHECK_STD;
                            }
                            LINMsg[0].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MW;
                            LINMsg[0].PID = LINFrame.ID;
                            LINMsg[0].Timestamp = (UInt32)SCHCmds.DelayOfMs;
                            //将信号进行赋值,此处可以根据实际情况进行赋值
                            LDFDecoder.LDF_LIN_SIGNAL[] SignalList = new LDFDecoder.LDF_LIN_SIGNAL[LINFrame.SignalListLen];
                            for(int k=0;k<LINFrame.SignalListLen;k++){
                                SignalList[k] = (LDFDecoder.LDF_LIN_SIGNAL)Marshal.PtrToStructure((IntPtr)((UInt32)LINFrame.pSignalList + j * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_SIGNAL))), typeof(LDFDecoder.LDF_LIN_SIGNAL));
                                SignalList[k].Value.ScalarValue = 0;
                            }
                            //将帧信号中的值转换到字节数组中
                            LINMsg[0].Data = new Byte[8];
                            LDFDecoder.LDF_DataFrameToRaw(ref LINFrame, LINMsg[0].Data);
                            LINMsg[0].DataLen = LINFrame.Size;
                            IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) * LINOutMsg.Length);
                            ret = USB2LIN_EX.LIN_EX_MasterSync(DevHandle, (Byte)LINIndex, LINMsg, pt, 1);
                            if (ret < USB2LIN_EX.LIN_EX_SUCCESS)
                            {
                                Console.WriteLine("MasterSync LIN failed!");
                                return;
                            }else{
                                Console.WriteLine(string.Format("MsgLen = {0}", ret));
                                for(int k=0;k<ret;k++){
                                    LINOutMsg[k] = (USB2LIN_EX.LIN_EX_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + k * Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG))), typeof(USB2LIN_EX.LIN_EX_MSG));
                                    Console.Write(Marshal.PtrToStringAnsi(LINFrame.pName) + "["+MSGTypeStr[LINMsg[0].MsgType]+"]"+string.Format(" SYNC[{0:X2}] PID[{1:X2}] ", LINOutMsg[k].Sync, LINOutMsg[k].PID));
                                    for(int z=0;z<LINOutMsg[k].DataLen;z++){
                                        Console.Write(string.Format("{0:X2} ", LINOutMsg[k].Data[z]));
                                    }
                                    Console.WriteLine(string.Format("[{0:X2}] [{1:D2}:{2:D2}:{3:D2}.{4:D3}]", LINOutMsg[k].Check, (LINOutMsg[k].Timestamp / 36000000) % 60, (LINOutMsg[k].Timestamp / 600000) % 60, (LINOutMsg[k].Timestamp / 10000) % 60, (LINOutMsg[k].Timestamp / 10) % 1000));
                                }
                            }
                        }else{
                            //该帧为主机读数据帧
                            LINMsg[0].MsgType = USB2LIN_EX.LIN_EX_MSG_TYPE_MR;
                            LINMsg[0].PID = LINFrame.ID;
                            LINMsg[0].Timestamp = (UInt32)SCHCmds.DelayOfMs;
                            LINMsg[0].Data = new Byte[8];
                            IntPtr pt = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG)) * LINOutMsg.Length);
                            ret = USB2LIN_EX.LIN_EX_MasterSync(DevHandle, (Byte)LINIndex, LINMsg, pt, 1);
                            if (ret < USB2LIN_EX.LIN_EX_SUCCESS)
                            {
                                Console.WriteLine("MasterSync LIN failed!");
                                return;
                            }else{
                                Console.WriteLine(string.Format("MsgLen = {0}", ret));
                                for(int k=0;k<ret;k++){
                                    LINOutMsg[k] = (USB2LIN_EX.LIN_EX_MSG)Marshal.PtrToStructure((IntPtr)((UInt32)pt + k * Marshal.SizeOf(typeof(USB2LIN_EX.LIN_EX_MSG))), typeof(USB2LIN_EX.LIN_EX_MSG));
                                    Console.Write(Marshal.PtrToStringAnsi(LINFrame.pName) + "[" + MSGTypeStr[LINMsg[0].MsgType] + "]" + string.Format(" SYNC[{0:X2}] PID[{1:X2}] ", LINOutMsg[k].Sync, LINOutMsg[k].PID));
                                    for (int z = 0; z < LINOutMsg[k].DataLen; z++)
                                    {
                                        Console.Write(string.Format("{0:X2} ", LINOutMsg[k].Data[z]));
                                    }
                                    Console.WriteLine(string.Format("[{0:X2}] [{1:D2}:{2:D2}:{3:D2}.{4:D3}]", LINOutMsg[k].Check, (LINOutMsg[k].Timestamp / 36000000) % 60, (LINOutMsg[k].Timestamp / 600000) % 60, (LINOutMsg[k].Timestamp / 10000) % 60, (LINOutMsg[k].Timestamp / 10) % 1000));
                                    if (LINOutMsg[i].DataLen > 0)
                                    {
                                        //将字节数组的数据解析到帧信号中
                                        LDFDecoder.LDF_DataRawToFrame(ref LINFrame, LINOutMsg[i].Data);
                                        //循环显示信号值
                                        for(int x=0;x<LINFrame.SignalListLen;x++){
                                            LDFDecoder.LDF_LIN_SIGNAL Signal = (LDFDecoder.LDF_LIN_SIGNAL)Marshal.PtrToStructure((IntPtr)((UInt32)LINFrame.pSignalList + x * Marshal.SizeOf(typeof(LDFDecoder.LDF_LIN_SIGNAL))), typeof(LDFDecoder.LDF_LIN_SIGNAL));
                                            if (Signal.ValueType == LDFDecoder.LDF_SIGNAL_VALUE_TYPE.LDF_SIGNAL_VALUE_TYPE_SCALAR)
                                            {
                                                Console.WriteLine(Marshal.PtrToStringAnsi(LINFrame.pName) + string.Format(" = {0}", Signal.Value.ScalarValue));
                                            }else{
                                                Console.Write(Marshal.PtrToStringAnsi(LINFrame.pName)+" = ");
                                                for (int y = 0; y < Signal.Size / 8; y++)
                                                {
                                                    Console.Write(string.Format("{0:X2} ", Signal.Value.ArrayValue[k]));
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
