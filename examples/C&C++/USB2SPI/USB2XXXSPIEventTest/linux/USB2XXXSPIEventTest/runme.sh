#!/bin/bash
echo $PWD
SystemType=`uname -m`
echo $SystemType
if [[ "$SystemType" == "x86_64" ]];then
export LD_LIBRARY_PATH="$PWD/lib/x64:$LD_LIBRARY_PATH"
elif [[ "$SystemType" == "x86_32" ]];then
export LD_LIBRARY_PATH="$PWD/lib/x86:$LD_LIBRARY_PATH"
elif [[ "$SystemType" =~ "armv6" ]];then
export LD_LIBRARY_PATH="$PWD/lib/ARMv6:$LD_LIBRARY_PATH"
elif [[ "$SystemType" =~ "armv7" ]];then
export LD_LIBRARY_PATH="$PWD/lib/ARMv7:$LD_LIBRARY_PATH"
elif [[ "$SystemType" =~ "armv8" ]];then
export LD_LIBRARY_PATH="$PWD/lib/ARMv8:$LD_LIBRARY_PATH"
fi
echo $LD_LIBRARY_PATH
./${PWD##*/}
