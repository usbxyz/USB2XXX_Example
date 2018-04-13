package com.usbxyz.usb2xxxdemo.USB2CAN;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.usbxyz.usb2xxxdemo.R;

public class USB2CANActivity extends AppCompatActivity {
    Button btCANTest;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_usb2can);
        btCANTest = findViewById(R.id.btCANTest);

        btCANTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(USB2CANActivity.this, CANTestActivity.class);
                startActivity(intent);
            }
        });
    }
}
