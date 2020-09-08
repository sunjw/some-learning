//
//  main.cpp
//  cputemp
//
//  Created by Sun Junwen on 2017/4/15.
//  Copyright © 2017年 Sun Junwen. All rights reserved.
//

#include <stdio.h>
#include "smc.h"

static void GetCpuTemp(double *temp)
{
    SMCOpen();
    *temp = SMCGetTemperature();
    SMCClose();
}

int main(int argc, const char *argv[])
{
    double temp = 0.0;
    GetCpuTemp(&temp);
    printf("CPU Tempurature: %.2fºC\n", temp);
    
    return 0;
}
