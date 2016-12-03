#!/bin/bash
echo $PWD
export LD_LIBRARY_PATH="$PWD/lib/RaspberryPi:$LD_LIBRARY_PATH"
echo $LD_LIBRARY_PATH
./${PWD##*/}
