package com.usbxyz.usb2xxxdemo.USB2ADC;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.sun.jna.Pointer;
import com.usbxyz.USB2XXX.USB2ADC;
import com.usbxyz.USB2XXX.USB_Device;
import com.usbxyz.usb2xxxdemo.R;

import java.util.HashMap;
import java.util.Timer;
import java.util.TimerTask;

public class ADCTestActivity extends AppCompatActivity {
    private static final String ACTION_USB_PERMISSION ="com.usbxyz.USB_PERMISSION";
    //导入库，必须要
    static{
        System.loadLibrary("jnidispatch");
        System.loadLibrary("USB2XXX");
        System.loadLibrary("usb");
    }
    private UsbManager usbManager;
    private UsbDevice usbDevice=null;
    private PendingIntent pendingIntent;
    TextView textView;
    boolean stopGetDataFlag = false;
    //定义回调函数接口
    USB2ADC.ADC_GET_DATA_HANDLE mGetAdcDataHandle = new USB2ADC.ADC_GET_DATA_HANDLE(){
        @Override
        public void ReceiveCallback(int DevIndex, final Pointer pData, final int DataNum) {
            if(stopGetDataFlag){
                return;
            }
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    try {
                        textView.append(String.format("Read adc data num = %d\n", DataNum));
                        short[] ADCData = pData.getShortArray(0, 1);
                        textView.append(String.format("ADC Data = %fV\n", (ADCData[0] * 3.3) / 4095));
                    }catch (Exception ep){
                        ep.printStackTrace();
                    }
                }
            });
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_adctest);
        USBInit(this);
        textView = findViewById(R.id.textView);
        textView.setMovementMethod(ScrollingMovementMethod.getInstance());
        Button button = findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int ret;
                final int DevHandle;
                int[] DevHandleArry = new int[20];
                boolean state;
                if(usbDevice == null){
                    textView.append("Please connect device\n");
                    return;
                }
                //扫描设备
                int fd = usbManager.openDevice(usbDevice).getFileDescriptor();
                System.out.println("fd = "+fd);
                ret = USB_Device.INSTANCE.USB_ScanDevice(DevHandleArry,fd);
                if(ret > 0){
                    textView.append("DeviceNum = "+ret+"\n");
                    textView.append("DevHandle = "+String.format("0x%08X",DevHandleArry[0])+"\n");
                    DevHandle = DevHandleArry[0];
                }else{
                    textView.append("No device\n");
                    return;
                }

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
                //初始化配置ADC
                char ADC_Channel = 0x01;
                ret = USB2ADC.INSTANCE.ADC_Init(DevHandle, ADC_Channel, 1000000);
                if (ret != USB2ADC.ADC_SUCCESS) {
                    textView.append("Init adc error!\n");
                    return;
                } else {
                    textView.append("Init adc success!\n");
                }
                //读取ADC数据
                short[] Buffer = new short[10];
                ret = USB2ADC.INSTANCE.ADC_Read(DevHandle, Buffer, Buffer.length);
                if (ret != USB2ADC.ADC_SUCCESS) {
                    textView.append("Read adc error!\n");
                    return;
                } else {
                    for (int i = 0; i < Buffer.length; i++) {
                        textView.append(String.format("ADC Data[%d] = %fV\n", i, (Buffer[i] * 3.3) / 4095));
                    }
                }
                //连续读取ADC
                try {
                    ret = USB2ADC.INSTANCE.ADC_StartContinueRead(DevHandle, ADC_Channel, 10000, 1024, mGetAdcDataHandle);
                    if (ret != USB2ADC.ADC_SUCCESS) {
                        textView.append("Start continue read adc error!\n");
                        return;
                    } else {
                        textView.append("Start continue read adc success!\n");
                    }

                }catch (Exception ep){
                    ep.printStackTrace();
                }
                //延时一段时间，等待连续采集一段时间的数据后再停止
                Timer timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                int ret;
                                //1秒后停止连续接收数据
                                stopGetDataFlag = true;
                                ret = USB2ADC.INSTANCE.ADC_StopContinueRead(DevHandle);
                                if(ret != USB2ADC.ADC_SUCCESS){
                                    textView.append("Stop continue read adc error!\n");
                                    return;
                                }else{
                                    textView.append("Stop continue read adc success!\n");
                                }
                                //关闭设备
                                USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
                            }
                        });
                    }
                },1000);
            }
        });
    }
    private void USBInit(Context context){
        usbManager = (UsbManager) context.getSystemService(context.USB_SERVICE);
        pendingIntent = PendingIntent.getBroadcast(context, 0, new Intent(ACTION_USB_PERMISSION), 0);

        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
        filter.addAction(UsbManager.ACTION_USB_DEVICE_ATTACHED);
        filter.setPriority(Integer.MAX_VALUE); //设置级别
        UsbDeviceDetachedBroadcastReceiver usbStateReceiver = new UsbDeviceDetachedBroadcastReceiver();
        context.registerReceiver(usbStateReceiver,filter);

        HashMap<String, UsbDevice> map = usbManager.getDeviceList();
        for(UsbDevice device : map.values()){
            if(USB_Device.DEV_VID == device.getVendorId() && USB_Device.DEV_PID == device.getProductId()){
                usbDevice = device;
                if(!usbManager.hasPermission(device)) {
                    usbManager.requestPermission(usbDevice, pendingIntent);
                }
            }
        }
    }

    //定义USB插拔广播接收器
    class UsbDeviceDetachedBroadcastReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_DETACHED)) {
                usbDevice = null;
                Toast.makeText(context, "Device Detached", Toast.LENGTH_SHORT).show();
            }else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)){
                Toast.makeText(context, "Device Attached", Toast.LENGTH_SHORT).show();
                HashMap<String, UsbDevice> map = usbManager.getDeviceList();
                for(UsbDevice device : map.values()){
                    if(USB_Device.DEV_VID == device.getVendorId() && USB_Device.DEV_PID == device.getProductId()){
                        usbDevice = device;
                        if(!usbManager.hasPermission(device)) {
                            usbManager.requestPermission(usbDevice, pendingIntent);
                        }
                    }
                }
            }
        }
    }
}
