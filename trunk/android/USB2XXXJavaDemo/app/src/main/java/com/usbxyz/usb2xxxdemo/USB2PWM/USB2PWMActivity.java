package com.usbxyz.usb2xxxdemo.USB2PWM;

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

import com.toomoss.USB2XXX.USB_Device;
import com.usbxyz.usb2xxxdemo.R;
import com.usbxyz.usb2xxxdemo.USB2LIN.LINMasterActivity;
import com.usbxyz.usb2xxxdemo.USB2LIN.USB2LINActivity;

import java.util.HashMap;

public class USB2PWMActivity extends AppCompatActivity {
    Button btPWMTest;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_usb2pwm);
        btPWMTest = findViewById(R.id.btPWMTest);

        btPWMTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(USB2PWMActivity.this, PWMTestActivity.class);
                startActivity(intent);
            }
        });
    }

}