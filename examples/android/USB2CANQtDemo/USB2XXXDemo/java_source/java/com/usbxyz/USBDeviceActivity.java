package com.usbxyz;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import java.util.HashMap;
import android.widget.Toast;

public class USBDeviceActivity extends org.qtproject.qt5.android.bindings.QtActivity {
    private static final String ACTION_USB_PERMISSION ="com.usbxyz.USB_PERMISSION";
    private static UsbManager usbManager;
    private static UsbDevice usbDevice=null;
    private PendingIntent pendingIntent;
    private int DEV_PID = 0x7918;
    private int DEV_VID = 0x0483;
    private static int dev_fd = 0;
    Context mContext;
    public int getDevFd()
    {
        if((usbDevice == null)||(usbManager == null)){
            return 0;
        }
        //打开设备并返回文件描述符
        dev_fd = usbManager.openDevice(usbDevice).getFileDescriptor();
        return dev_fd;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        USBInit(this);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);
    }

    private void USBInit(Context context){
        mContext = context;
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
         if(DEV_VID == device.getVendorId() && DEV_PID == device.getProductId()){
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
                 dev_fd = 0;
                 Toast.makeText(context, "Device Detached", Toast.LENGTH_SHORT).show();
             }else if (intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)){
                 Toast.makeText(context, "Device Attached", Toast.LENGTH_SHORT).show();
                 HashMap<String, UsbDevice> map = usbManager.getDeviceList();
                 for(UsbDevice device : map.values()){
                     if(DEV_VID == device.getVendorId() && DEV_PID == device.getProductId()){
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
