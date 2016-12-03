package com.usbxyz.usb2xxxiicam2321;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.usbxyz.usb2xxx.USB2IIC;
import com.usbxyz.usb2xxx.USB2XXX;

import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {
    USB2XXX usb2xxx;
    Button buttonStart;
    TextView textViewHumidity;
    TextView textViewTemperature;
    Timer timer;
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
        buttonStart = (Button)findViewById(R.id.buttonsStart);
        textViewHumidity = (TextView)findViewById(R.id.textViewHumidity);
        textViewTemperature = (TextView)findViewById(R.id.textViewTemperature);
        usb2xxx = new USB2XXX(this,new ConnectStateChanged());//需要监视设备插拔事件

        buttonStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(buttonStart.getText().equals("开始")) {
                    int devIndex = 0;
                    int ret;
                    //扫描设备连接数
                    int devNum = usb2xxx.device.USB_ScanDevice();
                    if (devNum <= 0) {
                        Toast.makeText(MainActivity.this, "No device connected!", Toast.LENGTH_SHORT).show();
                        return;
                    }
                    //打开设备
                    if (!usb2xxx.device.USB_OpenDevice(devIndex)) {
                        Toast.makeText(MainActivity.this, "Open device error!", Toast.LENGTH_SHORT).show();
                        return;
                    }
                    //初始化I2C适配器
                    USB2IIC.IIC_CONFIG IICConfig = usb2xxx.usb2iic.new IIC_CONFIG((byte)1,(byte)7,(byte)1,100000,(short)0);
                    ret = usb2xxx.usb2iic.IIC_Init(0,0,IICConfig);
                    if(ret != USB2IIC.IIC_SUCCESS){
                        Toast.makeText(MainActivity.this, "Init device error!", Toast.LENGTH_SHORT).show();
                        return;
                    }
                    //开启定时读取数据
                    timer = new Timer(true);
                    //这里必须用这种方式启动定时器任务，若是单独传入定时器任务参数，则下次再启动的时候程序会闪退
                    timer.schedule(new TimerTask() {
                        @Override
                        public void run() {
                            int ret;
                            usb2xxx.usb2iic.IIC_WriteBytes(0,0,(short)0x5C,null,0,10);
                            byte[] WriteBuffer = new byte[3];
                            WriteBuffer[0] = 0x03;
                            WriteBuffer[1] = 0x00;
                            WriteBuffer[2] = 0x04;
                            ret = usb2xxx.usb2iic.IIC_WriteBytes(0,0,(short)0x5C,WriteBuffer,3,10);
                            if(ret != USB2IIC.IIC_SUCCESS){
                                timer.cancel();
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(MainActivity.this, "Write data error!", Toast.LENGTH_SHORT).show();
                                        buttonStart.setText("开始");
                                    }
                                });
                                return;
                            }
                            try {
                                Thread.sleep(10);
                            }catch (Exception ep){
                                ep.printStackTrace();
                            }
                            byte[] ReadBuffer = new byte[10];
                            ret = usb2xxx.usb2iic.IIC_ReadBytes(0,0,(short)0x5C,ReadBuffer,ReadBuffer.length,10);
                            if(ret != USB2IIC.IIC_SUCCESS){
                                timer.cancel();
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        Toast.makeText(MainActivity.this, "Write data error!", Toast.LENGTH_SHORT).show();
                                        buttonStart.setText("开始");
                                    }
                                });
                                return;
                            }else{
                                final float t = (short)(((short)ReadBuffer[4] << 8)&0xFF00 | (short)ReadBuffer[5]&0xFF) / 10.0f;
                                final float h =(short) (((short)ReadBuffer[2] << 8)&0xFF00 | (short)ReadBuffer[3]&0xFF) / 10.0f;
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        textViewTemperature.setText(String.format("%.1f ℃",t));
                                        textViewHumidity.setText(String.format("%.1f %%",h));
                                    }
                                });
                            }
                        }
                    },1000,1000);
                    buttonStart.setText("停止");
                }else{
                    timer.cancel();
                    usb2xxx.device.USB_CloseDevice(0);
                    buttonStart.setText("开始");
                }
            }
        });
    }
}
