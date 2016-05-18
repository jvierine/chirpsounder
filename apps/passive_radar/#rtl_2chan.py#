#!/usr/bin/env python
##################################################
# Gnuradio Python Flow Graph
# Title: Top Block
# Generated: Wed Sep 25 16:02:07 2013
##################################################

from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio import window
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
#from gnuradio.gr import firdes
from gnuradio.wxgui import forms
from gnuradio.wxgui import scopesink2
from gnuradio.wxgui import waterfallsink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import osmosdr
import wx
import juha
import os
import time
import math
from gnuradio.eng_option import eng_option
from optparse import OptionParser

def time_stamp():
    a=time.time()
    b=time.strftime("%Y.%m.%d_%H.%M.%S",time.strptime(time.ctime(a)))
    stmp="%s.%09d" % (b,int((a-math.floor(a))*1000000000))
    return(stmp)


parser = OptionParser(option_class=eng_option)

parser.add_option("-c", "--centerfreq",dest="centerfreq", action="store", type="float",
                  default=105.108e6,
                  help="Center frequency (default 105.108e6 Hz)")

parser.add_option("-r", "--samp_rate",dest="samp_rate", action="store", type="float",
                  default=2.4e6,
                  help="Sample rate (default 2.4e6 Hz)")

parser.add_option("-d", "--dec",dest="dec", action="store", type="int",
                  default=10,
                  help="Decimation (default 10)")

parser.add_option("-n", "--name",dest="name", action="store", default="data",
                  type="string",
                  help="Prefix of data directory.")

(op, args) = parser.parse_args()


samp_rate = op.samp_rate
rfgain = 20.0
ifgain = 20.0
dec = op.dec
cf = op.centerfreq

rtl0 = osmosdr.source( args="nchan=" + str(1) + " " + "rtl=0" )
rtl0.set_sample_rate(samp_rate)
rtl0.set_center_freq(cf, 0)
rtl0.set_freq_corr(0, 0)
rtl0.set_dc_offset_mode(2, 0)
rtl0.set_iq_balance_mode(0, 0)
rtl0.set_gain_mode(0, 0)
rtl0.set_gain(rfgain, 0)
rtl0.set_if_gain(ifgain, 0)
rtl0.set_bb_gain(0, 0)
rtl0.set_antenna("", 0)
rtl0.set_bandwidth(1e6, 0)
		  
rtl1 = osmosdr.source( args="nchan=" + str(1) + " " + "rtl=1" )
rtl1.set_sample_rate(samp_rate)
rtl1.set_center_freq(cf, 0)
rtl1.set_freq_corr(0, 0)
rtl1.set_dc_offset_mode(2, 0)
rtl1.set_iq_balance_mode(0, 0)
rtl1.set_gain_mode(0, 0)
rtl1.set_gain(rfgain, 0)
rtl1.set_if_gain(ifgain, 0)
rtl1.set_bb_gain(0, 0)
rtl1.set_antenna("", 0)
rtl1.set_bandwidth(1e6, 0)
		  
lpf0 = filter.fir_filter_ccc(dec, (firdes.low_pass_2(1, samp_rate, 50e3, 10e3, 40, firdes.WIN_BLACKMAN_hARRIS)))
lpf1 = filter.fir_filter_ccc(dec, (firdes.low_pass_2(1, samp_rate, 50e3, 10e3, 40, firdes.WIN_BLACKMAN_hARRIS)))

dc0 = filter.dc_blocker_cc(1000, True)
dc1 = filter.dc_blocker_cc(1000, True)

rtl0.set_gain(0.0, 0)
rtl0.set_if_gain(0.0, 0)
rtl1.set_gain(0.0, 0)
rtl1.set_if_gain(0.0, 0)
		
tstamp=time_stamp()
os.makedirs("%s-%s/000"%(op.name,tstamp))
os.makedirs("%s-%s/001"%(op.name,tstamp))
print "Making dir %s-%s/000"%(op.name,tstamp)
print "Making dir %s-%s/001"%(op.name,tstamp)
fsink0 = juha.filesink("%s-%s/000"%(op.name,tstamp),gr.sizeof_gr_complex, int(samp_rate/dec))
fsink1 = juha.filesink("%s-%s/001"%(op.name,tstamp),gr.sizeof_gr_complex, int(samp_rate/dec))

tb = gr.top_block()
tb.connect((rtl0, 0), (lpf0, 0), (dc0,0), (fsink0,0))
tb.connect((rtl1, 0), (lpf1, 0), (dc1,0), (fsink1,0))
tb.start()
while True:
	time.sleep(1)
