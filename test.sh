#!/bin/bash

sys='/sys/class/calc/'
proc='/proc/calc/'
filesystem=$sys
n1=7
n2=8
n2_1=10
n2_2=7
n3_1=5
n3_2=10
n4_1=30
n4_2=6
n5_1=-5
n5_2=4

sudo sh -c "echo $n1 > $filesystem/fnum"
cat $filesystem/fnum
sudo sh -c "echo $n2 > $filesystem/snum"
cat $filesystem/snum
sudo sh -c "echo "sum" > $filesystem/operation"
cat $filesystem/operation
echo
res=$(cat $filesystem/result)
echo $res
let an=$n1+$n2
if [[ $an = $res ]]
then echo "Succes!!!"
else echo "Failed..."
fi
echo

sudo sh -c "echo $n2_1 > $filesystem/fnum"
cat $filesystem/fnum
sudo sh -c "echo $n2_2 > $filesystem/snum"
cat $filesystem/snum
sudo sh -c "echo "sub" > $filesystem/operation"
cat $filesystem/operation
echo
res=$(cat $filesystem/result)
echo $res
let an=$n2_1-$n2_2
if [[ $an = $res ]]
then echo "Succes!!!"
else echo "Failed..."
fi
echo

sudo sh -c "echo $n3_1 > $filesystem/fnum"
cat $filesystem/fnum
sudo sh -c "echo $n3_2 > $filesystem/snum"
cat $filesystem/snum
sudo sh -c "echo "mul" > $filesystem/operation"
cat $filesystem/operation
echo
res=$(cat $filesystem/result)
echo $res
let an=$n3_1*$n3_2
if [[ $an = $res ]]
then echo "Succes!!!"
else echo "Failed..."
fi
echo

sudo sh -c "echo $n4_1 > $filesystem/fnum"
cat $filesystem/fnum
sudo sh -c "echo $n4_2 > $filesystem/snum"
cat $filesystem/snum
sudo sh -c "echo "div" > $filesystem/operation"
cat $filesystem/operation
echo
res=$(cat $filesystem/result)
echo $res
let an=$n4_1/$n4_2
if [[ $an = $res ]]
then echo "Succes!!!"
else echo "Failed..."
fi
echo

sudo sh -c "echo $n3_1 > $filesystem/fnum"
cat $filesystem/fnum
sudo sh -c "echo $n3_2 > $filesystem/snum"
cat $filesystem/snum
sudo sh -c "echo "sub" > $filesystem/operation"
cat $filesystem/operation
echo
res=$(cat $filesystem/result)
echo $res
let an=$n3_1-$n3_2
if [[ $an = $res ]]
then echo "Succes!!!"
else echo "Failed..."
fi
echo

sudo sh -c "echo $n5_1 > $filesystem/fnum"
cat $filesystem/fnum
sudo sh -c "echo $n5_2 > $filesystem/snum"
cat $filesystem/snum
sudo sh -c "echo "mul" > $filesystem/operation"
cat $filesystem/operation
echo
res=$(cat $filesystem/result)
echo $res
let an=$n5_1*$n5_2
if [[ $an = $res ]]
then echo "Succes!!!"
else echo "Failed..."
fi
