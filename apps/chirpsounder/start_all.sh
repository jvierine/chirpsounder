#!/bin/bash

nohup ./chirp.py -o /data1/ch0 -m 192.168.60.2 > log/192.168.60.2.log 2>&1 &
nohup ./chirp.py -o /data1/ch1 -m 192.168.60.3 > log/192.168.60.3.log 2>&1 &
nohup ./chirp.py -o /data1/ch2 -m 192.168.60.4 > log/192.168.60.4.log 2>&1 &
nohup ./chirp.py -o /data1/ch3 -m 192.168.60.5 > log/192.168.60.5.log 2>&1 &
nohup ./chirp.py -o /data1/ch4 -m 192.168.60.6 > log/192.168.60.6.log 2>&1 &

nohup ./chirp.py -o /data1/ch5 -m 192.168.50.2 > log/192.168.50.2.log 2>&1 &
nohup ./chirp.py -o /data1/ch6 -m 192.168.50.3 > log/192.168.50.3.log 2>&1 &
nohup ./chirp.py -o /data1/ch7 -m 192.168.50.4 > log/192.168.50.4.log 2>&1 &
nohup ./chirp.py -o /data1/ch8 -m 192.168.50.5 > log/192.168.50.5.log 2>&1 &
nohup ./chirp.py -o /data1/ch9 -m 192.168.50.6 > log/192.168.50.6.log 2>&1 &
