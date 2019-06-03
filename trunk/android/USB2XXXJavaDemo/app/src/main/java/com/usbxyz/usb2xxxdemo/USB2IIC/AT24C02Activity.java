package com.usbxyz.usb2xxxdemo.USB2IIC;

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

import com.usbxyz.USB2XXX.USB2IIC;
import com.usbxyz.USB2XXX.USB_Device;
import com.usbxyz.usb2xxxdemo.R;

import java.util.HashMap;

public class AT24C02Activity extends AppCompatActivity {
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
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_at24c02);
        USBInit(this);
        textView = findViewById(R.id.textView);
        textView.setMovementMethod(ScrollingMovementMethod.getInstance());
        Button button = findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int ret;
                int DevHandle = 0;
                int[] DevHandleArry = new int[20];
                int IICIndex = 0;
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
                //配置I2C总线相关参数
                USB2IIC.IIC_CONFIG IICConfig =  new USB2IIC.IIC_CONFIG();
                IICConfig.AddrBits = 7;
                IICConfig.ClockSpeedHz = 100000;
                IICConfig.EnablePu = 1;
                IICConfig.Master = 1;
                IICConfig.OwnAddr = 0x01;
                ret = USB2IIC.INSTANCE.IIC_Init(DevHandle,IICIndex,IICConfig);
                if(ret != USB2IIC.IIC_SUCCESS){
                    textView.append("Initialize device error!\n");
                    return;
                }else{
                    textView.append("Initialize device success!\n");
                }
                //扫描从机地址
                short[] SlaveAddrs = new short[128];
                ret = USB2IIC.INSTANCE.IIC_GetSlaveAddr(DevHandle,IICIndex,SlaveAddrs);
                textView.append("Slave Addrs:");
                for(int i=0;i<ret;i++){
                    textView.append(String.format("%02X ",SlaveAddrs[i]));
                }
                textView.append("\n");
                //写数据
                byte[] WriteDataBuffer = new byte[9];
                for(int i=0;i<256;i+=8){
                    WriteDataBuffer[0] = (byte)i;//起始地址
                    for(int j=0;j<8;j++){
                        WriteDataBuffer[1+j] = (byte)(i+j);
                    }
                    ret = USB2IIC.INSTANCE.IIC_WriteBytes(DevHandle,IICIndex,(short)0x50,WriteDataBuffer,9,10);
                    if(ret != USB2IIC.IIC_SUCCESS){
                        textView.append("Write data error!"+String.format("%d\n",ret));
                        return;
                    }
                    try {
                        Thread.sleep(10);
                    }catch (Exception ep){
                        ep.printStackTrace();
                    }
                }
                //读数据
                byte[] ReadDataBuffer = new byte[256];
                WriteDataBuffer[0] = 0x00;//起始地址
                ret = USB2IIC.INSTANCE.IIC_WriteReadBytes(DevHandle,IICIndex,(short)0x50,WriteDataBuffer,1,ReadDataBuffer,256,10);
                if(ret != USB2IIC.IIC_SUCCESS){
                    textView.append("Read data error!"+String.format("%d\n",ret));
                    return;
                }else{
                    textView.append("Read Data:\n");
                }
                for(int i=0;i<256;i++){
                    textView.append(String.format("%02X ",ReadDataBuffer[i]));
                }
                textView.append("\n");
                textView.append("24C02 test success!\n");
                //关闭设备
                USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
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
