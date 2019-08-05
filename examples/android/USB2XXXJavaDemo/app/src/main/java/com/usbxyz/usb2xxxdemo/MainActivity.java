package com.usbxyz.usb2xxxdemo;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.usbxyz.usb2xxxdemo.USB2ADC.USB2ADCActivity;
import com.usbxyz.usb2xxxdemo.USB2CAN.USB2CANActivity;
import com.usbxyz.usb2xxxdemo.USB2GPIO.USB2GPIOActivity;
import com.usbxyz.usb2xxxdemo.USB2IIC.USB2IICActivity;
import com.usbxyz.usb2xxxdemo.USB2LIN.USB2LINActivity;
import com.usbxyz.usb2xxxdemo.USB2PWM.USB2PWMActivity;
import com.usbxyz.usb2xxxdemo.USB2SPI.USB2SPIActivity;
import com.usbxyz.usb2xxxdemo.USB2UART.USB2UARTActivity;

public class MainActivity extends AppCompatActivity {
    Button btUSB2ADC,btUSB2CAN,btUSB2GPIO,btUSB2IIC,btUSB2PWM,btUSB2SPI,btUSB2UART,btUSB2LIN;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btUSB2ADC = findViewById(R.id.btUSB2ADC);
        btUSB2CAN = findViewById(R.id.btUSB2CAN);
        btUSB2GPIO = findViewById(R.id.btUSB2GPIO);
        btUSB2IIC = findViewById(R.id.btUSB2IIC);
        btUSB2PWM = findViewById(R.id.btUSB2PWM);
        btUSB2SPI = findViewById(R.id.btUSB2SPI);
        btUSB2UART = findViewById(R.id.btUSB2UART);
        btUSB2LIN = findViewById(R.id.btUSB2LIN);

        //设置按钮点击事件
        btUSB2ADC.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2ADCActivity.class);
                startActivity(intent);
            }
        });
        btUSB2CAN.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2CANActivity.class);
                startActivity(intent);
            }
        });
        btUSB2GPIO.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2GPIOActivity.class);
                startActivity(intent);
            }
        });
        btUSB2IIC.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2IICActivity.class);
                startActivity(intent);
            }
        });
        btUSB2PWM.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2PWMActivity.class);
                startActivity(intent);
            }
        });
        btUSB2SPI.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2SPIActivity.class);
                startActivity(intent);
            }
        });
        btUSB2UART.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2UARTActivity.class);
                startActivity(intent);
            }
        });
        btUSB2LIN.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent=new Intent();
                intent.setClass(MainActivity.this, USB2LINActivity.class);
                startActivity(intent);
            }
        });
    }

}
