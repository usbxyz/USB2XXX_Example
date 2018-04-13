package com.usbxyz.usb2xxxdemo.USB2GPIO;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.usbxyz.usb2xxxdemo.MainActivity;
import com.usbxyz.usb2xxxdemo.R;

public class USB2GPIOActivity extends AppCompatActivity {
    Button btGPIOTest,btGPIOSpeedTest;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_usb2gpio);
        btGPIOTest = findViewById(R.id.btGPIOTest);
        btGPIOSpeedTest = findViewById(R.id.btGPIOSpeedTest);

        btGPIOTest.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(USB2GPIOActivity.this, GPIOTestActivity.class);
                startActivity(intent);
            }
        });
    }
}
