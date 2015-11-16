#!/bin/bash

events=(
        PAPI_MEM_SCY 
        PAPI_L1_DCM 
        PAPI_L1_DCH
        PAPI_L2_DCM 
        PAPI_L2_DCH
        PAPI_L3_DCM 
        PAPI_L3_DCH
        PAPI_FP_OPS  
        PAPI_FP_INS
)

for event in ${events[@]} 
do
        echo -n "Trying ${event}... "
        for n in `seq 1 9`
        do
            echo -n "mm$n "
            taskset -c 0 ./mm$n $event 2> /dev/null | grep PAPI | sed "s/${event}//" 
        done > ${event}.data
        if [ $(wc -c <"${event}.data") -gt 36 ]; then
            echo "OK."
            echo -n "  Generating plot... "
            ./plot.py ${event}
            echo "OK."
        else
            rm ${event}.data
            echo "Failed."
        fi
done

