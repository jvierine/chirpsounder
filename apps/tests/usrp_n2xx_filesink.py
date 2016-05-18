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
      cpu_format="sc16",
      otw_format="sc16",
      channels=range(1)))

u.set_samp_rate(1e6)
u.set_center_freq(126e6)

u.set_clock_source("external", uhd.ALL_MBOARDS)
u.set_time_source("external", uhd.ALL_MBOARDS)

tt = time.time()
u.set_time_unknown_pps(uhd.time_spec(math.ceil(tt)+1.0))
starttime = math.floor(time.time()+10.0)
u.set_start_time(uhd.time_spec(starttime))
fg = gr.top_block()
dst = juha.filesink("/tmp/testdata",2*gr.sizeof_short, int(1e6))
fg.connect((u, 0), (dst,0))
fg.start()
while(True):
   time.sleep(1)
