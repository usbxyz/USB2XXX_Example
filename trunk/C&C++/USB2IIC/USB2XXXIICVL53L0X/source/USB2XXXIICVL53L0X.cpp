  /*
  ******************************************************************************
  * @file     : USB2XXXIICTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX IIC test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2014, usbxyz.com
  * http://www.usbxyz.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "VL53L0X.h"

#define LONG_RANGE      1
#define HIGH_SPEED      0
#define HIGH_ACCURACY   1

int main(int argc, const char* argv[])
{
    bool state;
    state = VL53L0X_init();
    if(!state){
        printf("VL53L0X initialize error!\n");
    }
    setTimeout(500);
#if LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  setVcselPulsePeriod(VcselPeriodPreRange, 18);
  setVcselPulsePeriod(VcselPeriodFinalRange, 14);
#endif

#if HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  setMeasurementTimingBudget(20000);
#elif HIGH_ACCURACY
  // increase timing budget to 200 ms
  setMeasurementTimingBudget(200000);
#endif
  while(true)
  {
      printf("%d mm\n",readRangeSingleMillimeters());
      if (timeoutOccurred()) {
          printf(" TIMEOUT\n"); 
      }
      Sleep(500);
  }
    return 0;
}