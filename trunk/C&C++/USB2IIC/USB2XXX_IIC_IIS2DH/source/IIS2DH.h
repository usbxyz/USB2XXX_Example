/*
 * IIS2DH.h
 * A library for 3-Axis Digital Accelerometer(¡À16g)
 *  
 * Copyright (c) 2014 seeed technology inc.
 * Website    : www.usbxyz.com
 * Author     : wdluo
 * Create Time: April 2018
 * Change Log :
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
 
#include "stdint.h"
#include "usb2iic.h"

#ifndef IIS2DH_H
#define IIS2DH_H

#define ValBit(VAR,Place)         (VAR & (1<<Place))
#define BIT(x) ( (x) )

#define IIS2DH_MEMS_I2C_ADDRESS     0x19//0x32

//Register and define
#define IIS2DH_WHO_AM_I             0x0F   // device identification register

// CONTROL REGISTER 1 
#define IIS2DH_CTRL_REG1            0x20
#define IIS2DH_ODR                  BIT(5) //0000 - power down / 0001 - 1Hz / 0010 - 10Hz / 0011 - 25Hz / 0100 - 50Hz / 0101 - 100Hz / 0110 - 200Hz / 0111 - 400Hz / 1000 - Low-power mode (1.620 kHz)
#define IIS2DH_LPEN                 BIT(3) //0: normal mode, 1: low-power mode
#define IIS2DH_ZEN                  BIT(2) //Z-axis enable: 0 - disable / 1 - enable
#define IIS2DH_YEN                  BIT(1) //Y-axis enable: 0 - disable / 1 - enable
#define IIS2DH_XEN                  BIT(0) //Y-axis enable: 0 - disable / 1 - enable


#endif
