#!/bin/bash
echo $PWD
SystemType=`uname -m`
echo $SystemType
if [[ "$SystemType" == "x86_64" ]];then
export LD_LIBRARY_PATH="$PWD/lib/x86_64:$LD_LIBRARY_PATH"
else
export LD_LIBRARY_PATH="$PWD/lib/x86_32:$LD_LIBRARY_PATH"
fi
echo $LD_LIBRARY_PATH
./${PWD##*/}
