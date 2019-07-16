package com.usbxyz.usb2xxxdemo.USB2CAN;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.usbxyz.USB2XXX.USB2CAN;
import com.usbxyz.USB2XXX.USB2GPIO;
import com.usbxyz.USB2XXX.USB_Device;
import com.usbxyz.usb2xxxdemo.R;
import com.usbxyz.usb2xxxdemo.USB2GPIO.GPIOTestActivity;

import java.util.HashMap;

public class CANTestActivity extends AppCompatActivity {
    private static final String ACTION_USB_PERMISSION ="com.usbxyz.USB_PERMISSION";
    //导入库，必须要
    static{
        System.loadLibrary("jnidispatch");
        System.loadLibrary("USB2XXX");
        System.loadLibrary("usb");
    }
    private UsbManager usbManager;
    private UsbDevice[] usbDevice=new UsbDevice[20];
    int DevNum=0;
    private PendingIntent pendingIntent;
    TextView textView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cantest);
        USBInit(this);
        textView = findViewById(R.id.textView);
        textView.setMovementMethod(ScrollingMovementMethod.getInstance());
        Button button = findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int ret;
                int DevHandle;
                byte CANIndex = 0;
                boolean state;
                int[] DevHandleArry = new int[20];
                if(DevNum <= 0){
                    textView.append("Please connect device\n");
                    return;
                }
                //扫描设备
                int[] pFd = new int[20];
                for(int i=0;i<DevNum;i++){
                    UsbDeviceConnection mUsbDeviceConnection = usbManager.openDevice(usbDevice[i]);
                    pFd[i] = mUsbDeviceConnection.getFileDescriptor();
                    System.out.println("fd = "+pFd[i]);
                }
                ret = USB_Device.INSTANCE.USB_ScanDevice(DevHandleArry,pFd,DevNum);
                if(ret > 0){
                    textView.append("DeviceNum = "+ret+"\n");
                    for(int i=0;i<ret;i++) {
                        textView.append("DevHandle = " + String.format("0x%08X", DevHandleArry[i]) + "\n");
                    }
                }else{
                    textView.append("No device "+String.format("%d\n",ret));
                    return;
                }
                DevHandle = DevHandleArry[0];
                //打开设备
                state = USB_Device.INSTANCE.USB_OpenDevice(DevHandle);
                if(!state){
                    textView.append("Open device error");
                    return;
                }else{
                    textView.append("Open device success\n");
                }
                //获取设备信息
                try {
                    USB_Device.DEVICE_INFO DevInfo = new USB_Device.DEVICE_INFO();
                    byte[] funcStr = new byte[128];
                    state = USB_Device.INSTANCE.DEV_GetDeviceInfo(DevHandle,DevInfo,funcStr);
                    if(!state){
                        USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
                        textView.append("Get device infomation error");
                        return;
                    }else{
                        textView.append("Firmware Info:\n");
                        textView.append("--Name:" + new String(DevInfo.FirmwareName, "UTF-8")+"\n");
                        textView.append("--Build Date:" + new String(DevInfo.BuildDate, "UTF-8")+"\n");
                        textView.append(String.format("--Firmware Version:v%d.%d.%d\n", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF));
                        textView.append(String.format("--Hardware Version:v%d.%d.%d\n", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF));
                        textView.append("--Functions:" + new String(funcStr, "UTF-8")+"\n");
                    }
                } catch (Exception ep) {
                    ep.printStackTrace();
                }
                //初始化配置CAN
                USB2CAN.CAN_INIT_CONFIG CANConfig = new USB2CAN.CAN_INIT_CONFIG();
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
                ret = USB2CAN.INSTANCE.CAN_Init(DevHandle,CANIndex,CANConfig);
                if(ret != USB2CAN.CAN_SUCCESS){
                    textView.append("Config CAN failed!\n");
                    return;
                }else{
                    textView.append("Config CAN success!\n");
                }
                //配置过滤器，必须配置，否则可能无法收到数据
                USB2CAN.CAN_FILTER_CONFIG CANFilter = new USB2CAN.CAN_FILTER_CONFIG();
                CANFilter.Enable = 1;
                CANFilter.ExtFrame = 0;
                CANFilter.FilterIndex = 0;
                CANFilter.FilterMode = 0;
                CANFilter.MASK_IDE = 0;
                CANFilter.MASK_RTR = 0;
                CANFilter.MASK_Std_Ext = 0;
                ret = USB2CAN.INSTANCE.CAN_Filter_Init(DevHandle,CANIndex,CANFilter);
                if(ret != USB2CAN.CAN_SUCCESS){
                    textView.append("Config CAN filter failed!\n");
                    return;
                }else{
                    textView.append("Config CAN filter success!\n");
                }
                //发送CAN数据
                USB2CAN.CAN_MSG[] CanMsg = (USB2CAN.CAN_MSG[])new USB2CAN.CAN_MSG().toArray(5);
                for(int i=0;i<5;i++){
                    //CanMsg[i] =  new USB2CAN.CAN_MSG.ByValue();
                    CanMsg[i].ExternFlag = 0;
                    CanMsg[i].RemoteFlag = 0;
                    CanMsg[i].ID = i;
                    CanMsg[i].DataLen = 8;
                    //CanMsg[i].Data = new byte[8];
                    for(byte j=0;j<CanMsg[i].DataLen;j++){
                        CanMsg[i].Data[j] = (byte)(i*0x10+j);
                    }
                }

                int SendNum = USB2CAN.INSTANCE.CAN_SendMsg(DevHandle, CANIndex, CanMsg, CanMsg.length);
                if (SendNum >= 0) {
                    textView.append(String.format("Success send frames:%d\n", SendNum));
                } else {
                    textView.append("Send CAN data failed!\n");
                }

                //获取CAN状态
                USB2CAN.CAN_STATUS CANStatus = new USB2CAN.CAN_STATUS();
                ret = USB2CAN.INSTANCE.CAN_GetStatus(DevHandle,CANIndex,CANStatus);
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
                int CanNum = USB2CAN.INSTANCE.CAN_GetMsg(DevHandle,CANIndex,CanMsgBuffer);
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
                //关闭设备
                USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
            }
        });
    }
    private void USBInit(Context context){
        usbManager = (UsbManager) context.getSystemService(context.USB_SERVICE);
        //注册USB插拔检测服务
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.setPriority(Integer.MAX_VALUE); //设置级别
        UsbDeviceDetachedBroadcastReceiver usbStateReceiver = new UsbDeviceDetachedBroadcastReceiver();
        context.registerReceiver(usbStateReceiver,filter);
        //扫描已经连接的USB设备
        DevNum = 0;
        HashMap<String, UsbDevice> map = usbManager.getDeviceList();
        for(UsbDevice device : map.values()){
            if(USB_Device.DEV_VID == device.getVendorId() && USB_Device.DEV_PID == device.getProductId()){
                usbDevice[DevNum] = device;
                if(!usbManager.hasPermission(device)) {
                    pendingIntent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                    usbManager.requestPermission(usbDevice[DevNum], pendingIntent);
                }
                DevNum++;
            }
        }
    }

    //定义USB插拔广播接收器
    class UsbDeviceDetachedBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_DETACHED)) {
                Toast.makeText(context, "Device Detached", Toast.LENGTH_SHORT).show();
            }else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)){
                Toast.makeText(context, "Device Attached", Toast.LENGTH_SHORT).show();
            }
            DevNum = 0;
            HashMap<String, UsbDevice> map = usbManager.getDeviceList();
            for(UsbDevice device : map.values()){
                if(USB_Device.DEV_VID == device.getVendorId() && USB_Device.DEV_PID == device.getProductId()){
                    usbDevice[DevNum] = device;
                    if(!usbManager.hasPermission(device)) {
                        pendingIntent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), 0);
                        usbManager.requestPermission(usbDevice[DevNum], pendingIntent);
                    }
                    DevNum++;
                }
            }
        }
    }
}

