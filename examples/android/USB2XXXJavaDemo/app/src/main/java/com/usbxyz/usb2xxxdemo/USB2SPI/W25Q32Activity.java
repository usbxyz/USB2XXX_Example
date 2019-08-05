package com.usbxyz.usb2xxxdemo.USB2SPI;

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

import com.toomoss.USB2XXX.USB2SPI;
import com.toomoss.USB2XXX.USB_Device;
import com.usbxyz.usb2xxxdemo.R;
import com.usbxyz.usb2xxxdemo.USB2IIC.AT24C02Activity;

import java.util.HashMap;

public class W25Q32Activity extends AppCompatActivity {
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
        setContentView(R.layout.activity_w25q32);
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
                int SPIIndex = USB2SPI.SPI1_CS0;
                byte[] WriteDataBuffer = new byte[20*1024];
                byte[] ReadDataBuffer = new byte[20*1024];
                boolean state;
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
                if (!state) {
                    textView.append("Open device error");
                    return;
                } else {
                    textView.append("Open device success\n");
                }
                //获取设备信息
                try {
                    USB_Device.DEVICE_INFO DevInfo = new USB_Device.DEVICE_INFO();
                    byte[] funcStr = new byte[128];
                    state = USB_Device.INSTANCE.DEV_GetDeviceInfo(DevHandle, DevInfo, funcStr);
                    if (!state) {
                        USB_Device.INSTANCE.USB_CloseDevice(DevHandle);
                        textView.append("Get device infomation error");
                        return;
                    } else {
                        textView.append("Firmware Info:\n");
                        textView.append("--Name:" + new String(DevInfo.FirmwareName, "UTF-8") + "\n");
                        textView.append("--Build Date:" + new String(DevInfo.BuildDate, "UTF-8") + "\n");
                        textView.append(String.format("--Firmware Version:v%d.%d.%d\n", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF));
                        textView.append(String.format("--Hardware Version:v%d.%d.%d\n", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF));
                        textView.append("--Functions:" + new String(funcStr, "UTF-8") + "\n");
                    }
                } catch (Exception ep) {
                    ep.printStackTrace();
                }
                //配置SPI总线相关参数
                USB2SPI.SPI_CONFIG SPIConfig = new USB2SPI.SPI_CONFIG();
                SPIConfig.Mode = USB2SPI.SPI_MODE_HARD_HDX;
                SPIConfig.ClockSpeedHz = 25000000;
                SPIConfig.CPHA = 0;
                SPIConfig.CPOL = 0;
                SPIConfig.LSBFirst = USB2SPI.SPI_MSB;
                SPIConfig.Master = USB2SPI.SPI_MASTER;
                SPIConfig.SelPolarity = USB2SPI.SPI_SEL_LOW;
                ret = USB2SPI.INSTANCE.SPI_Init(DevHandle,SPIIndex,SPIConfig);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Initialize device error!\n");
                    return;
                }else{
                    textView.append("Initialize device success!\n");
                }
                //读取芯片ID
                int ChipID;
                WriteDataBuffer[0] = (byte)0x9F;
                ret = USB2SPI.INSTANCE.SPI_WriteReadBytes(DevHandle,SPIIndex,WriteDataBuffer,1,ReadDataBuffer,3,0);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Read flash ID error!\n");
                    return;
                }else{
                    ChipID = (((int)ReadDataBuffer[0]<<16)&0xFF0000)|(((int)ReadDataBuffer[1]<<8)&0xFF00)|(((int)ReadDataBuffer[2]&0xFF));
                    textView.append(String.format("Flash ID = 0x%06X\n",ChipID&0xFFFFFF));
                }
                if(((ChipID&0xFFFFFF)==0xFFFFFF)||((ChipID&0xFFFFFF)==0x000000)){
                    textView.append("Flash ID error,Please check the wiring!\n");
                }
                //发送写使能命令
                WriteDataBuffer[0] = 0x06;
                ret = USB2SPI.INSTANCE.SPI_WriteBytes(DevHandle,SPIIndex,WriteDataBuffer,1);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Flash write enable error!\n");
                    return;
                }else{
                    textView.append("Write enable success!\n");
                }
                // Sector Erase (4KB)
                WriteDataBuffer[0] = 0x20;
                WriteDataBuffer[1] = 0x00;
                WriteDataBuffer[2] = 0x00;
                WriteDataBuffer[3] = 0x00;
                ret = USB2SPI.INSTANCE.SPI_WriteBytes(DevHandle,SPIIndex,WriteDataBuffer,4);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Sector Erase start error!\n");
                    return;
                }else{
                    textView.append("Sector erase start success!\n");
                }
                // Check the operation to complete
                do{
                    WriteDataBuffer[0] = 0x05;// Read Status Register-1
                    ret = USB2SPI.INSTANCE.SPI_WriteReadBytes(DevHandle,SPIIndex,WriteDataBuffer,1,ReadDataBuffer,1,0);
                    try {
                        Thread.sleep(100);
                    }catch (Exception ep){
                        ep.printStackTrace();
                    }
                }while(((ReadDataBuffer[0]&0x01)==0x01)&&(ret == USB2SPI.SPI_SUCCESS));
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Sector Erase error!\n");
                    return;
                }else{
                    textView.append("Sector erase success!\n");
                }
                // Write Enable
                WriteDataBuffer[0] = 0x06;
                ret = USB2SPI.INSTANCE.SPI_WriteBytes(DevHandle,SPIIndex,WriteDataBuffer,1);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Flash write enable error!\n");
                    return;
                }else{
                    textView.append("Write enable success!\n");
                }
                // Page Program
                WriteDataBuffer[0] = 0x02;// Page Program command
                WriteDataBuffer[1] = 0x00;// Address
                WriteDataBuffer[2] = 0x01;
                WriteDataBuffer[3] = 0x00;
                for(int i=4;i<(256+4);i++){
                    WriteDataBuffer[i] = (byte)(i-4);
                }
                ret = USB2SPI.INSTANCE.SPI_WriteBytes(DevHandle,SPIIndex,WriteDataBuffer,256+4);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Page program start error!\n");
                    return;
                }else{
                    textView.append("Page program start success!\n");
                }
                // Check the operation to complete
                do{
                    WriteDataBuffer[0] = 0x05;// Read Status Register-1
                    ret = USB2SPI.INSTANCE.SPI_WriteReadBytes(DevHandle,SPIIndex,WriteDataBuffer,1,ReadDataBuffer,1,0);
                    try {
                        Thread.sleep(100);
                    }catch (Exception ep){
                        ep.printStackTrace();
                    }
                }while(((ReadDataBuffer[0]&0x01)==0x01)&&(ret == USB2SPI.SPI_SUCCESS));
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Page program error!\n");
                    return;
                }else{
                    textView.append("Page program success!\n");
                }
                // Read Data
                WriteDataBuffer[0] = 0x03;//Read Data command
                WriteDataBuffer[1] = 0x00;//address
                WriteDataBuffer[2] = 0x01;
                WriteDataBuffer[3] = 0x00;
                ret = USB2SPI.INSTANCE.SPI_WriteReadBytes(DevHandle,SPIIndex,WriteDataBuffer,4,ReadDataBuffer,256,0);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Read Data error!\n");
                    return;
                }else{
                    textView.append("Read Data success!\n");
                }
                textView.append("Data:\n");
                for(int i=0;i<256;i++){
                    textView.append(String.format("%02X ",ReadDataBuffer[i]));
                }
                textView.append("\n\n");
                textView.append("W25Q32 test success!\n");
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
