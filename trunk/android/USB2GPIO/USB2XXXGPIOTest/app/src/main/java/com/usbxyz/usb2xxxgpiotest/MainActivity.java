package com.usbxyz.usb2xxxgpiotest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.usbxyz.usb2xxx.USB2XXX;
import com.usbxyz.usb2xxx.USBDevice;

public class MainActivity extends AppCompatActivity {
    USB2XXX usb2xxx;
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
        Button button =(Button)findViewById(R.id.button);
        //以下两种方式均可，第二种方式可以实时监测设备连接状态
        //usb2xxx = new USB2XXX(this);//不监视设备插拔时间
        usb2xxx = new USB2XXX(this,new ConnectStateChanged());//需要监视设备插拔事件
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int devIndex = 0;
                int[] PinValue = new int[1];
                //扫描设备连接数
                int devNum = usb2xxx.device.USB_ScanDevice();
                if(devNum <= 0){
                    textView.setText("No device connected!\n");
                    return;
                }else{
                    textView.append("have "+String.format("%d",devNum)+" device connected!\n");
                }
                //打开设备
                if(!usb2xxx.device.USB_OpenDevice(devIndex)){
                    textView.append("Open device error!\n");
                    return;
                }else{
                    textView.append("Open device success!\n");
                }
                //获取设备信息
                USBDevice.DEVICE_INFO DevInfo = usb2xxx.device.new DEVICE_INFO();
                byte[] funcStr = new byte[256];
                if(!usb2xxx.device.USB_GetDeviceInfo(devIndex,DevInfo,funcStr)){
                    textView.append("Get device information error!\n");
                    return;
                }else{
                    try {
                        textView.append("Firmware Info:\n");
                        textView.append("    Name:"+new String(DevInfo.FirmwareName,"UTF-8")+"\n");
                        textView.append("    Build Date:"+new String(DevInfo.BuildDate,"UTF-8")+"\n");
                        textView.append(String.format("    Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF));
                        textView.append(String.format("    Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF));
                        textView.append("    Functions:"+new String(funcStr,"UTF-8")+"\n");
                    }catch (Exception ep){
                        ep.printStackTrace();
                    }
                }
                //输出测试——没上下拉
                usb2xxx.usb2gpio.GPIO_SetOutput(devIndex, 0xFFFF,0);
                for(int i=0;i<10;i++){
                    usb2xxx.usb2gpio.GPIO_Write(devIndex, 0xFFFF, 0xAAAA);
                    usb2xxx.usb2gpio.GPIO_Write(devIndex, 0xFFFF, 0x5555);
                }
                //输出测试——上拉
                usb2xxx.usb2gpio.GPIO_SetOutput(devIndex, 0xFFFF, 1);
                for(int i=0;i<10;i++){
                    usb2xxx.usb2gpio.GPIO_Write(devIndex, 0xFFFF, 0xAAAA);
                    usb2xxx.usb2gpio.GPIO_Write(devIndex, 0xFFFF, 0x5555);
                }
                //输出测试——下拉
                usb2xxx.usb2gpio.GPIO_SetOutput(devIndex, 0xFFFF, 2);
                for(int i=0;i<10;i++){
                    usb2xxx.usb2gpio.GPIO_Write(devIndex, 0xFFFF, 0xAAAA);
                    usb2xxx.usb2gpio.GPIO_Write(devIndex, 0xFFFF, 0x5555);
                }
                //测试输入——浮空
                usb2xxx.usb2gpio.GPIO_SetInput(devIndex, 0xFFFF, 0);
                usb2xxx.usb2gpio.GPIO_Read(devIndex, 0xFFFF,PinValue);
                textView.append(String.format("READ DATA(Float):%04X\n", PinValue[0]));
                //测试输入——上拉输入
                usb2xxx.usb2gpio.GPIO_SetInput(devIndex, 0xFFFF, 1);
                usb2xxx.usb2gpio.GPIO_Read(devIndex, 0xFFFF,PinValue);
                textView.append(String.format("READ DATA(Pu):%04X\n", PinValue[0]));
                //测试输入——下拉输入
                usb2xxx.usb2gpio.GPIO_SetInput(devIndex, 0xFFFF, 2);
                usb2xxx.usb2gpio.GPIO_Read(devIndex, 0xFFFF,PinValue);
                textView.append(String.format("READ DATA(Pd):%04X\n", PinValue[0]));
                //测试开漏输入——浮空
                usb2xxx.usb2gpio.GPIO_SetOpenDrain(devIndex, 0xFFFF, 0);
                usb2xxx.usb2gpio.GPIO_Read(devIndex, 0xFFFF,PinValue);
                textView.append(String.format("READ DATA(OD-Float):%04X\n", PinValue[0]));
                //测试开漏输入——上拉输入
                usb2xxx.usb2gpio.GPIO_SetOpenDrain(devIndex, 0xFFFF, 1);
                usb2xxx.usb2gpio.GPIO_Read(devIndex, 0xFFFF,PinValue);
                textView.append(String.format("READ DATA(OD-Pu):%04X\n", PinValue[0]));
                //测试开漏输入——下拉输入
                usb2xxx.usb2gpio.GPIO_SetOpenDrain(devIndex, 0xFFFF, 2);
                usb2xxx.usb2gpio.GPIO_Read(devIndex, 0xFFFF,PinValue);
                textView.append(String.format("READ DATA(OD-Pd):%04X\n", PinValue[0]));
            }
        });
    }
}
