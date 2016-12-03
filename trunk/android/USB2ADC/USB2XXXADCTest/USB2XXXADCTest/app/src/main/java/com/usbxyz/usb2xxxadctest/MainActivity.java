package com.usbxyz.usb2xxxadctest;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.usbxyz.usb2xxx.USB2ADC;
import com.usbxyz.usb2xxx.USB2XXX;
import com.usbxyz.usb2xxx.USBDevice;

import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    USB2XXX usb2xxx;
    TextView textView;
    //定义回调函数接口
    public class GetAdcDataHandle implements USB2ADC.ADC_GET_DATA_HANDLE{
        public void ReceiveCallback(int DevIndex,final short[] pData,final int DataNum){
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    textView.append(String.format("Read adc data num = %d\n", DataNum));
                    textView.append(String.format("ADC Data = %fV\n", (pData[0] * 3.3) / 4095));
                }
            });
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        textView =(TextView) findViewById(R.id.textView);
        Button button =(Button)findViewById(R.id.button);
        usb2xxx = new USB2XXX(this);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                textView.setText(null);
                int devIndex = 0;
                int ret;
                int[] PinValue = new int[1];
                //扫描设备连接数
                int devNum = usb2xxx.device.USB_ScanDevice();
                if (devNum <= 0) {
                    textView.setText("No device connected!\n");
                    return;
                } else {
                    textView.append("have " + String.format("%d", devNum) + " device connected!\n");
                }
                //打开设备
                if (!usb2xxx.device.USB_OpenDevice(devIndex)) {
                    textView.append("Open device error!\n");
                    return;
                } else {
                    textView.append("Open device success!\n");
                }
                //获取设备信息
                USBDevice.DEVICE_INFO DevInfo = usb2xxx.device.new DEVICE_INFO();
                byte[] funcStr = new byte[256];
                if (!usb2xxx.device.USB_GetDeviceInfo(devIndex, DevInfo, funcStr)) {
                    textView.append("Get device information error!\n");
                    return;
                } else {
                    try {
                        textView.append("Firmware Info:\n");
                        textView.append("    Name:" + new String(DevInfo.FirmwareName, "UTF-8") + "\n");
                        textView.append("    Build Date:" + new String(DevInfo.BuildDate, "UTF-8") + "\n");
                        textView.append(String.format("    Firmware Version:v%d.%d.%d\n", (DevInfo.FirmwareVersion >> 24) & 0xFF, (DevInfo.FirmwareVersion >> 16) & 0xFF, DevInfo.FirmwareVersion & 0xFFFF));
                        textView.append(String.format("    Hardware Version:v%d.%d.%d\n", (DevInfo.HardwareVersion >> 24) & 0xFF, (DevInfo.HardwareVersion >> 16) & 0xFF, DevInfo.HardwareVersion & 0xFFFF));
                        textView.append("    Functions:" + new String(funcStr, "UTF-8") + "\n");
                    } catch (Exception ep) {
                        ep.printStackTrace();
                    }
                }
                //初始化配置ADC
                byte ADC_Channel = 0x01;
                ret = usb2xxx.usb2adc.ADC_Init(devIndex, ADC_Channel, 1000000);
                if (ret != USB2ADC.ADC_SUCCESS) {
                    textView.append("Init adc error!\n");
                    return;
                } else {
                    textView.append("Init adc success!\n");
                }
                //读取ADC数据
                short[] Buffer = new short[10];
                ret = usb2xxx.usb2adc.ADC_Read(devIndex, Buffer, Buffer.length);
                if (ret != USB2ADC.ADC_SUCCESS) {
                    textView.append("Read adc error!\n");
                    return;
                } else {
                    for (int i = 0; i < Buffer.length * USB2ADC.BitCount(ADC_Channel); i++) {
                        textView.append(String.format("ADC Data[%d] = %fV\n", i, (Buffer[i] * 3.3) / 4095));
                    }
                }
                //连续读取ADC
                ret = usb2xxx.usb2adc.ADC_StartContinueRead(devIndex, ADC_Channel, 100000, 1024,new GetAdcDataHandle());
                if (ret != USB2ADC.ADC_SUCCESS) {
                    textView.append("Start continue read adc error!\n");
                    return;
                } else {
                    textView.append("Start continue read adc success!\n");
                }
                Timer timer = new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                int devIndex = 0;
                                int ret;
                                //1秒后停止连续接收数据
                                ret = usb2xxx.usb2adc.ADC_StopContinueRead(devIndex);
                                if(ret != USB2ADC.ADC_SUCCESS){
                                    textView.append("Stop continue read adc error!\n");
                                    return;
                                }else{
                                    textView.append("Stop continue read adc success!\n");
                                }
                            }
                        });
                    }
                },1000);
            }
        });
    }

}
