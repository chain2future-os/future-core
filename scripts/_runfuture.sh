#!/bin/bash
FUTURE_PATH=$1
rm -f /log/${HOSTNAME}.log
nohup $FUTURE_PATH/scripts/_runfuture.py $FUTURE_PATH/build &>> /log/${HOSTNAME}.log  &
