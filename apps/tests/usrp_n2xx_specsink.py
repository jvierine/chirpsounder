#!/usr/bin/env python
#
# Simple usrp N200 seris filesink sample
# (c) 2013 Juha Vierinen
#
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys, time, os, math, re
import juha
import math

# create usrp source block
u = uhd.usrp_source(
   device_addr="addr=192.168.10.2,recv_buff_size=100000000",
   stream_args=uhd.stream_args(
      cpu_format="fc32",
      otw_format="sc16",
      channels=range(1)))

u.set_samp_rate(25e6)
u.set_center_freq(105.1e6)
u.set_gain(20.0)

u.set_clock_source("external", uhd.ALL_MBOARDS)
u.set_time_source("external", uhd.ALL_MBOARDS)

tt = time.time()
u.set_time_unknown_pps(uhd.time_spec(math.ceil(tt)+1.0))
starttime = math.floor(time.time()+10.0)
u.set_start_time(uhd.time_spec(starttime))
fg = gr.top_block()
try:
    os.makedirs("/tmp/testspec")
except:
    pass
os.chdir("/tmp/testspec")
dst = juha.specsink(1024, 1000*25)
fg.connect((u, 0), (dst,0))
fg.start()
while(True):
   time.sleep(1)
