#!/bin/bash

sudo insmod calc.ko
sudo sh -c "echo 2 > /proc/calc/fnum"
sudo sh -c "echo 2 > /proc/calc/snum"
sudo sh -c "echo "sum" > /proc/calc/operation"
cat /proc/calc/result
dmesg | tail



