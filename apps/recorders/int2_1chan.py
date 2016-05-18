#!/usr/bin/env python
#
# Simple single channel usrp2 sampler
# (c) 2011-2013 Juha Vierinen
#
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from optparse import OptionParser
import sys, time, os, math, re

import juha
import sampler_util

sversion = "$Revision: 15$"

print "USRP2 Sampler %s support for GDF2\n" % sversion
print "(c) 2011-2013 Juha Vierinen\n\n"

## parse options
parser = OptionParser(option_class=eng_option)

parser.add_option("-m", "--mainboard", dest="mboard", type="string", default="192.168.10.2",
                  help="Mainboard addresses. E.g., -m 192.168.10.2")

parser.add_option("-c", "--centerfreq",dest="centerfreq", action="store", type="float",
                  help="Center frequency (default 126.0e6)")

parser.add_option("-g", "--gain",dest="gain", action="store",
                  help="Gain (default 0)")

parser.add_option("-f", "--filesize",dest="filesize", action="store",type="long",
                  help="File size (samples)")

parser.add_option("-r", "--samplerate",dest="samplerate", action="store", type="long",
                  help="Sample rate (default 10e6 Hz)")

parser.add_option("-s", "--starttime",dest="starttime", action="store",
                  type="int",
                  help="Start time of the experiment (unix time)")

parser.add_option("-d", "--subdevice",dest="subdev", action="store",
                  type="string", default="A:A",
                  help="USRP subdevice string (default A:A).")

parser.add_option("-p", "--period",dest="period", action="store",
                  type="int",
                  help="Experiment cycle length. Used to time an experiment that has already started (default 1 s).")

parser.add_option("-n", "--name",dest="name", action="store",
                  type="string",
                  help="Prefix of data directory.")

(op, args) = parser.parse_args()

if op.samplerate is None:
   op.samplerate = 10000000

if op.name is None:
   op.name = "data"

dir_name = "%s-%s"%(op.name,sampler_util.time_stamp())
os.makedirs(dir_name)

if op.period is None:
   op.period = 1

if op.mboard is None:
   op.mboard = "192.168.10.2"

if op.centerfreq is None:
   op.centerfreq = 126e6+0.2e6

if op.filesize is None:
   op.filesize = op.samplerate

if op.gain is None:
   op.gain = 0

r = gr.enable_realtime_scheduling()
if r == gr.RT_OK:
   print("Realtime scheduling enabled")
else:
   print "Note: failed to enable realtime scheduling"

# create usrp source block
u = uhd.usrp_source(
   device_addr="addr=%s,recv_buff_size=1000000000"%(op.mboard),
   stream_args=uhd.stream_args(
      cpu_format="sc16",
      otw_format="sc16",
      channels=range(1)))

u.set_clock_source("gpsdo")
u.set_time_source("gpsdo")

u.set_subdev_spec(op.subdev)
u.set_samp_rate(op.samplerate)
u.set_center_freq(op.centerfreq)

# create flowgraph
fg = gr.top_block()
dst = juha.filesink("%s/000"%(dir_name), 2*gr.sizeof_short, int(op.filesize))

# find next suitable launch time
if op.starttime is None:
    op.starttime = math.ceil(time.time())+5
    b=time.strftime("%Y.%m.%d_%H.%M.%S",time.strptime(time.ctime(op.starttime)))

op.starttime = sampler_util.find_next(op.starttime,op.period)
u.set_start_time(uhd.time_spec(op.starttime))
sampler_util.write_metadata(dir_name,u,1,1,op.starttime,dtype="<i2",itemsize=4,sr=op.samplerate,dec=1)

#dst.set_launchtime(int(op.starttime))

fg.connect((u, 0),(dst,0))

print "Launch time: ",op.starttime
print "Sample rate: ",op.samplerate
print "Main board: ",op.mboard
print "Frequencies: ",op.centerfreq
print "Gains: ",op.gain
print "Starting time: ",op.starttime
print "Directory: ",dir_name

fg.start()

while(True):
   time.sleep(1)


