package com.usbxyz.usb2xxxdemo.USB2SPI;

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

import com.usbxyz.USB2XXX.USB_Device;
import com.usbxyz.usb2xxxdemo.R;

import java.util.HashMap;

public class USB2SPIActivity extends AppCompatActivity {
    Button btSPIMasterTest,btSPISlaveTest;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_usb2spi);
        btSPIMasterTest = findViewById(R.id.btSPIMasterTest);
        btSPISlaveTest = findViewById(R.id.btSPISlaveTest);

        btSPIMasterTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(USB2SPIActivity.this, SPIMasterTestActivity.class);
                startActivity(intent);
            }
        });
    }

}