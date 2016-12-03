package com.usbxyz.usb2xxxiic24c02;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.usbxyz.usb2xxx.USB2IIC;
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
        textView =(TextView) findViewById(R.id.textView);
        buttonStart =(Button)findViewById(R.id.buttonStart);
        usb2xxx = new USB2XXX(this,new ConnectStateChanged());//需要监视设备插拔事件
        textView.setMovementMethod(ScrollingMovementMethod.getInstance());//设置为可以垂直滑动
        buttonStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int DevIndex = 0;
                int ret;
                int IICIndex = 0;
                byte[] WriteDataBuffer = new byte[1024];
                byte[] ReadDataBuffer = new byte[1024];
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
                //配置SPI总线相关参数
                USB2IIC.IIC_CONFIG IICConfig = usb2xxx.usb2iic. new IIC_CONFIG();
                ret = usb2xxx.usb2iic.IIC_Init(DevIndex,IICIndex,IICConfig);
                if(ret != USB2IIC.IIC_SUCCESS){
                    textView.append("Initialize device error!\n");
                    return;
                }else{
                    textView.append("Initialize device success!\n");
                }
                //写数据
                for(int i=0;i<256;i+=8){
                    WriteDataBuffer[0] = (byte)i;//起始地址
                    for(int j=0;j<8;j++){
                        WriteDataBuffer[1+j] = (byte)(i+j);
                    }
                    ret = usb2xxx.usb2iic.IIC_WriteBytes(DevIndex,IICIndex,(short)0x50,WriteDataBuffer,9,10);
                    if(ret != USB2IIC.IIC_SUCCESS){
                        textView.append("Write data error!\n");
                        return;
                    }
                    try {
                        Thread.sleep(10);
                    }catch (Exception ep){
                        ep.printStackTrace();
                    }
                }
                //读数据
                WriteDataBuffer[0] = 0x00;//起始地址
                ret = usb2xxx.usb2iic.IIC_WriteReadBytes(DevIndex,IICIndex,(short)0x50,WriteDataBuffer,1,ReadDataBuffer,256,10);
                if(ret != USB2IIC.IIC_SUCCESS){
                    textView.append("Read data error!\n");
                    return;
                }else{
                    textView.append("Read Data:\n");
                }
                for(int i=0;i<256;i++){
                    textView.append(String.format("%02X ",ReadDataBuffer[i]));
                }
                textView.append("\n");
                textView.append("24C02 test success!\n");
                //测试完毕，关闭设备
                usb2xxx.device.USB_CloseDevice(DevIndex);
            }
        });
    }
}
