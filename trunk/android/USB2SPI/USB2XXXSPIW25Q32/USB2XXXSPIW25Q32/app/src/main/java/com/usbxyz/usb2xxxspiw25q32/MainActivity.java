package com.usbxyz.usb2xxxspiw25q32;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.ScrollingTabContainerView;
import android.text.method.ScrollingMovementMethod;
import android.util.TypedValue;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.usbxyz.usb2xxx.USB2SPI;
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
                int SPIIndex = USB2SPI.SPI2_CS0;
                byte[] WriteDataBuffer = new byte[20*1024];
                byte[] ReadDataBuffer = new byte[20*1024];
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
                USB2SPI.SPI_CONFIG SPIConfig = usb2xxx.usb2spi. new SPI_CONFIG();
                SPIConfig.Mode = 1;
                SPIConfig.ClockSpeedHz = 25000000;
                SPIConfig.CPHA = 0;
                SPIConfig.CPOL = 0;
                SPIConfig.LSBFirst = 0;
                SPIConfig.Master = 1;
                SPIConfig.SelPolarity = 0;
                ret = usb2xxx.usb2spi.SPI_Init(DevIndex,SPIIndex,SPIConfig);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Initialize device error!\n");
                    return;
                }else{
                    textView.append("Initialize device success!\n");
                }
                //读取芯片ID
                int ChipID;
                WriteDataBuffer[0] = (byte)0x9F;
                ret = usb2xxx.usb2spi.SPI_WriteReadBytes(DevIndex,SPIIndex,WriteDataBuffer,1,ReadDataBuffer,3,0);
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
                ret = usb2xxx.usb2spi.SPI_WriteBytes(DevIndex,SPIIndex,WriteDataBuffer,1);
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
                ret = usb2xxx.usb2spi.SPI_WriteBytes(DevIndex,SPIIndex,WriteDataBuffer,4);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Sector Erase start error!\n");
                    return;
                }else{
                    textView.append("Sector erase start success!\n");
                }
                // Check the operation to complete
                do{
                    WriteDataBuffer[0] = 0x05;// Read Status Register-1
                    ret = usb2xxx.usb2spi.SPI_WriteReadBytes(DevIndex,SPIIndex,WriteDataBuffer,1,ReadDataBuffer,1,0);
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
                ret = usb2xxx.usb2spi.SPI_WriteBytes(DevIndex,SPIIndex,WriteDataBuffer,1);
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
                ret = usb2xxx.usb2spi.SPI_WriteBytes(DevIndex,SPIIndex,WriteDataBuffer,256+4);
                if(ret != USB2SPI.SPI_SUCCESS){
                    textView.append("Page program start error!\n");
                    return;
                }else{
                    textView.append("Page program start success!\n");
                }
                // Check the operation to complete
                do{
                    WriteDataBuffer[0] = 0x05;// Read Status Register-1
                    ret = usb2xxx.usb2spi.SPI_WriteReadBytes(DevIndex,SPIIndex,WriteDataBuffer,1,ReadDataBuffer,1,0);
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
                ret = usb2xxx.usb2spi.SPI_WriteReadBytes(DevIndex,SPIIndex,WriteDataBuffer,4,ReadDataBuffer,256,0);
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
            }
        });
    }
}
