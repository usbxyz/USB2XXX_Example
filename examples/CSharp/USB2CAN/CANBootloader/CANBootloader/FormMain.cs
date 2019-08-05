using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using USB2XXX;
namespace CANBootloader
{
    public partial class FormMain : Form
    {
        short ScanStartAddr = 0, ScanEndAddr = 0;
        int DeviceIndex = 0;
        int CANIndex = 0;
        Int32[] DevHandles = new Int32[20];

        public FormMain()
        {
            InitializeComponent();
            init();
        }
        //初始化界面控件值
        private void init()
        {
            this.comboBoxDeviceIndex.SelectedIndex = 0;
            this.comboBoxCANIndex.SelectedIndex = 0;
            this.comboBoxBaudRate.SelectedIndex = 0;
            this.comboBoxIDType.SelectedIndex = 0;
        }
        //根据错误代码获取错误字符串
        private String GetErrorString(int ErrorCode)
        {
            String strTemp = String.Format("[{0}]", ErrorCode);
            switch(ErrorCode){
            case 0:
                    strTemp += "成功";
                break;
            case 1:
                strTemp += "固件擦除出错";
                break;
            case 2:
                strTemp += "当前模式为APP，不能擦除固件";
                break;
            case 3:
                strTemp += "当前地址超出了正常的地址范围";
                break;
            case 4:
                strTemp += "当前模式不能写入固件数据";
                break;
            case 5:
                strTemp += "数据写入程序存储器出错";
                break;
            case 6:
                strTemp += "数据长度超出了程序存储器范围";
                break;
            case 7:
                strTemp += "数据传输CRC校验出错";
                break;
            case 8:
                strTemp += "数据写入芯片CRC校验出错";
                break;
            default:
                strTemp += "未知错误";
                break;
            }
            return strTemp;
        }
        /// <summary>
        /// 初始化配置USB2XXX CAN适配器
        /// </summary>
        /// <returns></returns>
        private bool configDevice()
        {
            int ret,DevNum;
            bool state;
            DeviceIndex = this.comboBoxDeviceIndex.SelectedIndex;
            CANIndex = this.comboBoxCANIndex.SelectedIndex;
            //扫描查找设备
            DevNum = usb_device.USB_ScanDevice(DevHandles);
            if (DevNum <= 0)
            {
                MessageBox.Show(this, "无设备连接！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            //打开设备
            state = usb_device.USB_OpenDevice(DevHandles[DeviceIndex]);
            if (!state)
            {
                MessageBox.Show(this, "打开设备失败！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            //初始化配置设备
            int BaudRate = int.Parse(this.comboBoxBaudRate.Text.Substring(0,this.comboBoxBaudRate.Text.Length-4))*1000;
            Byte IDType = (Byte)this.comboBoxIDType.SelectedIndex;
            //接入内部终端电阻
            if(this.checkBoxEnResistor.Checked){
                IDType |= 0x80;
            }
            ret = CAN_Bootloader.CAN_BOOT_Init(DevHandles[DeviceIndex], CANIndex, (UInt32)this.numericUpDownSendID.Value, (UInt32)this.numericUpDownReceiveID.Value, IDType, BaudRate);
            if (ret != CAN_Bootloader.CAN_SUCCESS)
            {
                MessageBox.Show(this, "初始化设备失败！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            return true;
        }
        /// <summary>
        /// 选择固件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonOpenFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.Filter = "二进制文件(*.bin)|*.bin";
            if (fd.ShowDialog() == DialogResult.OK)
            {
                this.textBoxFilePath.Text = fd.FileName;
            }
        }
        /// <summary>
        /// 升级固件
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void buttonUpgrate_Click(object sender, EventArgs e)
        {
            if (configDevice())
            {
                //如果文件不存在，就提示错误
                if (File.Exists(this.textBoxFilePath.Text))
                {
                    FormProgress upgrateFormProgress = new FormProgress();
                    upgrateFormProgress.DoWork += new FormProgress.DoWorkEventHandler(upgrateFirmware);
                    upgrateFormProgress.ProgressBar.Maximum = (int)new FileInfo(this.textBoxFilePath.Text).Length;
                    upgrateFormProgress.Text = "固件升级";
                    //ScanNode.Argument = checkBoxThrowException.Checked;
                    if (upgrateFormProgress.ShowDialog() == DialogResult.Cancel)
                    {
                        upgrateFormProgress.Close();
                    }
                }
                else
                {
                    MessageBox.Show(this, "无法打开固件文件，是否选择了固件文件？", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                }
            }
        }

        void upgrateFirmware(FormProgress sender, DoWorkEventArgs e)
        {
            int FirmwareFileSize;
            Byte NodeAddr;
            int ret;
            //如果文件不存在，就提示错误
            if (File.Exists(this.textBoxFilePath.Text))
            {
                BinaryReader br;
                FirmwareFileSize = (int)new FileInfo(this.textBoxFilePath.Text).Length;
                try
                {
                    //将固件文件读取到数据缓冲区中
                    br = new BinaryReader(new FileStream(this.textBoxFilePath.Text, FileMode.Open));
                    //判断当前所选择的节点
                    if (this.listViewNodeList.Items.Count <= 0)
                    {
                        //当前节点列表中没有节点
                        MessageBox.Show(this, "节点列表中无节点，请先扫描节点！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        br.Close();
                        return;
                    }
                    else
                    {
                        if (this.listViewNodeList.SelectedIndices[0] < 0)
                        {
                            //没有选择任何节点
                            MessageBox.Show(this, "请先选择节点！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                            br.Close();
                            return;
                        }
                        else
                        {
                            NodeAddr = Byte.Parse(this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[0].Text);
                        }
                    }
                    //检测当前固件信息
                    sender.SetProgress(0, "跳转到Bootloader...");
                    UInt32[] FWVersion = new UInt32[1];
                    Byte[] FWType = new Byte[1];
                    ret = CAN_Bootloader.CAN_BOOT_GetFWInfo(DevHandles[DeviceIndex],CANIndex,NodeAddr,FWType,FWVersion,50);
                    if (ret == CAN_Bootloader.CAN_SUCCESS)
                    {
                        if (FWType[0] != CAN_Bootloader.FW_TYPE_BOOT)
                        {
                            //跳转到Bootloader
                            ret = CAN_Bootloader.CAN_BOOT_EnterBootMode(DevHandles[DeviceIndex],CANIndex,NodeAddr);
                            if (ret != CAN_Bootloader.CAN_SUCCESS)
                            {
                                this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "进入BOOT模式失败！";
                                return;
                            }
                            else
                            {
                                //延时一段时间，保证设备成功进入BOOT模式
                                System.Threading.Thread.Sleep(300);
                                //再次确认固件信息
                                ret = CAN_Bootloader.CAN_BOOT_GetFWInfo(DevHandles[DeviceIndex],CANIndex,NodeAddr,FWType,FWVersion,50);
                                if (ret != CAN_Bootloader.CAN_SUCCESS)
                                {
                                    MessageBox.Show(this, "获取固件信息失败！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                                    br.Close();
                                    return;
                                }
                                else if (FWType[0] != CAN_Bootloader.FW_TYPE_BOOT)
                                {
                                    MessageBox.Show(this, "进入BOOT模式失败！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                                    br.Close();
                                    return;
                                }
                            }
                        }
                    }
                    else
                    {
                        MessageBox.Show(this, "获取固件信息失败！", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                        br.Close();
                        return;
                    }
                    //成功进入BOOT模式，开始升级固件
                    this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "开始擦除固件，该操作可能比较耗时...";
                    ret = CAN_Bootloader.CAN_BOOT_EraseApp(DevHandles[DeviceIndex], CANIndex, NodeAddr, (UInt32)FirmwareFileSize, 10000);
                    if (ret != CAN_Bootloader.CAN_SUCCESS)
                    {
                        this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "执行固件擦除指令失败！\r\n"+GetErrorString(ret);
                        br.Close();
                        return;
                    }else{
                        this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "固件擦除成功！";
                    }
                    //循环发送固件数据
                    UInt32 AddrOffset = 0;
                    byte[] DataBuffer;
                    UInt16[] BufferSize = new UInt16[1];
                    for (AddrOffset = 0; AddrOffset < FirmwareFileSize;)
                    {
                        //设置写数据偏移地址
                        ret = CAN_Bootloader.CAN_BOOT_SetAddrOffset(DevHandles[DeviceIndex], CANIndex, NodeAddr, AddrOffset,BufferSize,50);
                        if (ret != CAN_Bootloader.CAN_SUCCESS)
                        {
                            this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "设置地址偏移失败！\r\n" + GetErrorString(ret);
                            br.Close();
                            return;
                        }
                        if (BufferSize[0] <= 0)
                        {
                            this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "获取数据缓冲区大小为0！";
                        }
                        DataBuffer = new Byte[BufferSize[0]];
                        UInt16[] CRC16 = new UInt16[1];
                        UInt32 read_data_num = (UInt32)br.Read(DataBuffer, 0, (int)BufferSize[0]);
                        //发送数据
                        ret = CAN_Bootloader.CAN_BOOT_SendAppData(DevHandles[DeviceIndex], CANIndex, NodeAddr, DataBuffer, (UInt16)read_data_num, CRC16);
                        if (ret != CAN_Bootloader.CAN_SUCCESS)
                        {
                            this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "发送数据失败！\r\n" + GetErrorString(ret);
                            br.Close();
                            return;
                        }
                        //将数据写入内部程序存储器
                        ret = CAN_Bootloader.CAN_BOOT_WriteAppData(DevHandles[DeviceIndex], CANIndex, NodeAddr,CRC16[0],1000);
                        if (ret != CAN_Bootloader.CAN_SUCCESS)
                        {
                            this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "写数据失败！\r\n" + GetErrorString(ret);
                            br.Close();
                            return;
                        }
                        sender.SetProgress((int)(AddrOffset+read_data_num), "正在发送固件数据...");
                        AddrOffset += read_data_num;
                    }
                    //执行固件
                    ret = CAN_Bootloader.CAN_BOOT_ExecuteApp(DevHandles[DeviceIndex], CANIndex, NodeAddr);
                    if (ret != CAN_Bootloader.CAN_SUCCESS)
                    {
                        this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "发送执行固件指令失败！\r\n" + GetErrorString(ret);
                        br.Close();
                        return;
                    }
                    System.Threading.Thread.Sleep(300);
                    //再次获取固件信息，判断固件是否更新成功
                    ret = CAN_Bootloader.CAN_BOOT_GetFWInfo(DevHandles[DeviceIndex], CANIndex, NodeAddr, FWType, FWVersion, 100);
                    if (ret == CAN_Bootloader.CAN_SUCCESS)
                    {
                        //Ctrl+F5启动程序这里运行正常，F5启动程序运行不正常，能扫描到节点，但是无法在界面上显示出来，原因不明
                        this.listViewNodeList.BeginUpdate();  //数据更新，UI暂时挂起，直到EndUpdate绘制控件，可以有效避免闪烁并大大提高加载速度  
                        if (FWType[0] == CAN_Bootloader.FW_TYPE_BOOT)
                        {
                            this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[1].Text = "BOOT";
                        }
                        else
                        {
                            this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[1].Text = "APP";
                        }
                        this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[2].Text = String.Format("v{0}.{1}.{2}.{3}", (((FWVersion[0] >> 24) & 0xFF)), (FWVersion[0] >> 16) & 0xFF, ((FWVersion[0] >> 8) & 0xFF), FWVersion[0] & 0xFF);
                        this.listViewNodeList.EndUpdate();  //结束数据处理，UI界面一次性绘制。
                        this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "固件更新成功！";
                    }
                    else
                    {
                        this.listViewNodeList.Items[this.listViewNodeList.SelectedIndices[0]].SubItems[3].Text = "固件执行失败！";
                    }
                    br.Close();
                }
                catch (IOException ep)
                {
                    MessageBox.Show(this, ep.Message, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
            }
            else
            {
                MessageBox.Show(this, "无法打开固件文件，是否选择了固件文件？", "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }
        /// <summary>
        /// 扫描当前CAN总线上的节点，然后添加到节点列表里面
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void 扫描节点ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FormStartScanNode StartScanNode = new FormStartScanNode();
            if (StartScanNode.ShowDialog() == DialogResult.OK)
            {
                this.listViewNodeList.Items.Clear();
                if (configDevice())
                {
                    FormProgress ScanNode = new FormProgress();
                    ScanNode.DoWork += new FormProgress.DoWorkEventHandler(scanNode);
                    ScanStartAddr = (short)StartScanNode.StartAddr;
                    ScanEndAddr = (short)StartScanNode.EndAddr;
                    ScanNode.ProgressBar.Maximum = ScanEndAddr - ScanStartAddr+1;
                    ScanNode.Text = "节点扫描";
                    //ScanNode.Argument = checkBoxThrowException.Checked;
                    if (ScanNode.ShowDialog() == DialogResult.Cancel)
                    {
                        ScanNode.Close();
                    }
                }
            }
            StartScanNode.Close();
        }
        /// <summary>
        /// 扫描节点
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void scanNode(FormProgress sender, DoWorkEventArgs e)
        {
            bool GotNode = false;
            for (int i = ScanStartAddr; i <= ScanEndAddr; i++)
            {
                int ret;
                UInt32[] appVersion = new UInt32[1];
                Byte[] FWType = new Byte[1];
                ret = CAN_Bootloader.CAN_BOOT_GetFWInfo(DevHandles[DeviceIndex], CANIndex, (Byte)i, FWType,appVersion, 100);
                if (ret == CAN_Bootloader.CAN_SUCCESS)
                {
                    GotNode = true;
                    //Ctrl+F5启动程序这里运行正常，F5启动程序运行不正常，能扫描到节点，但是无法在界面上显示出来，原因不明
                    this.listViewNodeList.BeginUpdate();  //数据更新，UI暂时挂起，直到EndUpdate绘制控件，可以有效避免闪烁并大大提高加载速度  
                    ListViewItem lvi = new ListViewItem();
                    lvi.Text = i.ToString();//节点地址
                    if (FWType[0] == CAN_Bootloader.FW_TYPE_BOOT)
                    {
                        lvi.SubItems.Add("BOOT");
                    }
                    else
                    {
                        lvi.SubItems.Add("APP");
                    }
                    lvi.SubItems.Add(String.Format("v{0}.{1}.{2}.{3}", (((appVersion[0] >> 24) & 0xFF)) , (appVersion[0] >> 16) & 0xFF, ((appVersion[0] >> 8) & 0xFF) , appVersion[0] & 0xFF));
                    lvi.SubItems.Add("等待更新");
                    this.listViewNodeList.Items.Add(lvi);
                    this.listViewNodeList.EndUpdate();  //结束数据处理，UI界面一次性绘制。
                }
                else if (ret == CAN_Bootloader.CAN_BOOT_ERR_SEND)
                {
                    e.Cancel = true;
                    String msg = "发送CAN数据失败！\r\n";
                    msg += "1，检查CAN总线波特率是否匹配？\r\n";
                    msg += "2，检查总线终端电阻是否接好？\r\n";
                    msg += "3，检查总线接线是否正确？\r\n";
                    msg += "4，请确定固件中CAN总线引脚配置跟硬件匹配！";
                    MessageBox.Show(this, msg, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return;
                }
                sender.SetProgress(i - ScanStartAddr + 1, "正在检测节点：" + i.ToString());
                if (sender.CancellationPending)
                {
                    e.Cancel = true;
                    return;
                }
            }
            if (!GotNode)
            {
                e.Cancel = true;
                String msg = "没有扫描到节点！\r\n";
                msg += "1，请确定节点地址是在设置的节点地址范围？\r\n";
                msg += "2，请确定从节点是否按照协议要求格式返回节点信息？\r\n";
                msg += "3，请确定软件上数据收发ID设置跟节点固件数据收发ID设置一致？\r\n";
                msg += "4，请确定软件上数据收发ID类型跟节点固件数据收发ID类型一致？";
                MessageBox.Show(this, msg, "警告", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
        }

        private void 关于ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox AboutBoxDialog = new AboutBox();
            AboutBoxDialog.ShowDialog();
        }

        private void 打开文件ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog fd = new OpenFileDialog();
            fd.Filter = "二进制文件(*.bin)|*.bin";
            if (fd.ShowDialog() == DialogResult.OK)
            {
                this.textBoxFilePath.Text = fd.FileName;
            }
        }

        private void 退出ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void 联系我们ToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FormContactUs ContactUs = new FormContactUs();
            ContactUs.Show();
        }


    }
}
