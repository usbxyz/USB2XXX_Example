package com.usbxyz.usb2xxxcantest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.usbxyz.usb2xxx.USB2CAN;
import com.usbxyz.usb2xxx.USB2XXX;
import com.usbxyz.usb2xxx.USBDevice;

public class MainActivity extends AppCompatActivity {
    USB2XXX usb2xxx;
    Button buttonStart;
    TextView textView;
    public class ConnectStateChanged implements USB2XXX.DeviceConnectStateChanged{
        @Override
        public void stateChanged(boolean connected) {
            if(connected){
                Toast.makeText(MainActivity.this, "Device Attached", Toast.LENGTH_SHORT).show();
            }else{
                Toast.makeText(MainActivity.this, "Device Detached", Toast.LENGTH_SHORT).show();
            }
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setContentView(R.layout.activity_main);
        textView =(TextView) findViewById(R.id.textView);
        buttonStart =(Button)findViewById(R.id.buttonStart);
        usb2xxx = new USB2XXX(this,new ConnectStateChanged());//需要监视设备插拔事件
        textView.setMovementMethod(ScrollingMovementMethod.getInstance());//设置为可以垂直滑动
        buttonStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int ret;
                int DevIndex = 0;
                byte CANIndex = 0;
                //扫描设备连接数
                int devNum = usb2xxx.device.USB_ScanDevice();
                if (devNum <= 0) {
                    textView.setText("No device connected!\n");
                    return;
                } else {
                    textView.append("have " + String.format("%d", devNum) + " device connected!\n");
                }
                //打开设备
                if (!usb2xxx.device.USB_OpenDevice(DevIndex)) {
                    textView.append("Open device error!\n");
                    return;
                } else {
                    textView.append("Open device success!\n");
                }
                //获取设备信息
                USBDevice.DEVICE_INFO DevInfo = usb2xxx.device.new DEVICE_INFO();
                byte[] funcStr = new byte[256];
                if (!usb2xxx.device.USB_GetDeviceInfo(DevIndex, DevInfo, funcStr)) {
                    textView.append("Get device information error!\n");
                    return;
                } else {
                    try {
                        textView.append("Firmware Info:\n");
                        textView.append("--Name:" + new String(DevInfo.FirmwareName, "UTF-8") + "\n");
                        textView.append("--Build Date:" + new String(DevInfo.BuildDate, "UTF-8") + "\n");
                        textView.append(String.format("--Firmware Version:v%d.%d.%d\n", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF));
                        textView.append(String.format("--Hardware Version:v%d.%d.%d\n", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF));
                        textView.append("--Functions:" + new String(funcStr, "UTF-8") + "\n");
                    } catch (Exception ep) {
                        ep.printStackTrace();
                    }
                }
                //初始化配置CAN
                USB2CAN.CAN_INIT_CONFIG CANConfig = usb2xxx.usb2can. new CAN_INIT_CONFIG();
                CANConfig.CAN_Mode = 1;//环回模式
                CANConfig.CAN_ABOM = 0;//禁止自动离线
                CANConfig.CAN_NART = 1;//禁止报文重传
                CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
                CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
                //配置波特率,波特率 = 100M/(BRP*(SJW+BS1+BS2))
                CANConfig.CAN_BRP = 25;
                CANConfig.CAN_BS1 = 2;
                CANConfig.CAN_BS2 = 1;
                CANConfig.CAN_SJW = 1;
                ret = usb2xxx.usb2can.CAN_Init(DevIndex,CANIndex,CANConfig);
                if(ret != USB2CAN.CAN_SUCCESS){
                    textView.append("Config CAN failed!\n");
                    return;
                }else{
                    textView.append("Config CAN success!\n");
                }
                //配置过滤器，必须配置，否则可能无法收到数据
                USB2CAN.CAN_FILTER_CONFIG CANFilter = usb2xxx.usb2can. new CAN_FILTER_CONFIG();
                CANFilter.Enable = 1;
                CANFilter.ExtFrame = 0;
                CANFilter.FilterIndex = 0;
                CANFilter.FilterMode = 0;
                CANFilter.MASK_IDE = 0;
                CANFilter.MASK_RTR = 0;
                CANFilter.MASK_Std_Ext = 0;
                ret = usb2xxx.usb2can.CAN_Filter_Init(DevIndex,CANIndex,CANFilter);
                if(ret != USB2CAN.CAN_SUCCESS){
                    textView.append("Config CAN filter failed!\n");
                    return;
                }else{
                    textView.append("Config CAN filter success!\n");
                }
                //发送CAN数据
                USB2CAN.CAN_MSG[] CanMsg = new USB2CAN.CAN_MSG[5];
                for(int i=0;i<5;i++){
                    CanMsg[i] = usb2xxx.usb2can. new CAN_MSG();
                    CanMsg[i].ExternFlag = 0;
                    CanMsg[i].RemoteFlag = 0;
                    CanMsg[i].ID = i;
                    CanMsg[i].DataLen = 8;
                    for(byte j=0;j<CanMsg[i].DataLen;j++){
                        CanMsg[i].Data[j] = (byte)(i*0x10+j);
                    }
                }
                int SendedNum = usb2xxx.usb2can.CAN_SendMsg(DevIndex,CANIndex,CanMsg,5);
                if(SendedNum >= 0){
                    textView.append(String.format("Success send frames:%d\n",SendedNum));
                }else{
                    textView.append("Send CAN data failed!\n");
                }
                //获取CAN状态
                USB2CAN.CAN_STATUS CANStatus = usb2xxx.usb2can. new CAN_STATUS();
                ret = usb2xxx.usb2can.CAN_GetStatus(DevIndex,CANIndex,CANStatus);
                if(ret == USB2CAN.CAN_SUCCESS){
                    textView.append(String.format("TSR = %08X\n",CANStatus.TSR));
                    textView.append(String.format("ESR = %08X\n",CANStatus.ESR));
                }else{
                    textView.append("Get CAN status error!\n");
                }
                //延时
                try {
                    Thread.sleep(100);
                }catch (Exception ep){
                    ep.printStackTrace();
                }
                //读取数据
                USB2CAN.CAN_MSG[] CanMsgBuffer = new USB2CAN.CAN_MSG[10];
                int CanNum = usb2xxx.usb2can.CAN_GetMsg(DevIndex,CANIndex,CanMsgBuffer);
                if(CanNum > 0){
                    textView.append(String.format("Get CAN CanNum = %d\n",CanNum));
                    for(int i=0;i<CanNum;i++){
                        textView.append(String.format("CanMsg[%d].ID = %d\n",i,CanMsgBuffer[i].ID));
                        textView.append(String.format("CanMsg[%d].TimeStamp = %d\n",i,CanMsgBuffer[i].TimeStamp));
                        textView.append(String.format("CanMsg[%d].Data = ",i));
                        for(int j=0;j<CanMsgBuffer[i].DataLen;j++){
                            textView.append(String.format("%02X ",CanMsgBuffer[i].Data[j]));
                        }
                        textView.append("\n");
                    }
                }else if(CanNum == 0){
                    textView.append("No CAN data!\n");
                }else{
                    textView.append("Get CAN data error!\n");
                }
                //关闭设备
                textView.append("CAN test success!\n");
                //测试完毕，关闭设备
                usb2xxx.device.USB_CloseDevice(DevIndex);
            }
        });
    }
}
