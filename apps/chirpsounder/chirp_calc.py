#!/usr/bin/python
import matplotlib
matplotlib.use("Agg")

import numpy
import matplotlib.pyplot as plt
import glob
import sys
import os

import stuffr
import chirp_config
from optparse import OptionParser

# remove the any residual band specific variation in noise floor
# that isn't removed by the amplitude domain adaptive filter before chirp downconversion.
def medianEqualize(S):
    for i in numpy.arange(S.shape[0]):
        S[i,:] = S[i,:]/numpy.median(S[i,:])
    return(S)

#
# plot chirps
# 
def plot_all(dirn,name="*",window=8192,reanalyze=0):
    
    sl = chirp_config.get_all_sounders()
    for s in sl:
        if s["name"]==name or name=="*":
            print "Sounder %s"%(s["name"])
            cf = glob.glob("%s/%s*.out"%(dirn,s["name"]))
            cf.sort()
            cr = s["rate"]
            sr = chirp_config.if_rate
            if len(cf)>1:
                for f in cf[0:(len(cf)-1)]:
                    print f
                    if not os.path.isfile("%s.png"%(f)) or reanalyze == 1:
                        z = numpy.fromfile(f,dtype=numpy.complex64)
                        S = stuffr.spectrogram(z,window=window)
                        S = medianEqualize(S)
                        freq = numpy.linspace(0, (len(z)/sr)*cr , num=S.shape[0])/1e6
                        vrange = numpy.linspace( 3e8*(-(sr/2))/cr, 3e8*(sr/2.0)/cr , num=S.shape[1])/1e3
                        plt.pcolormesh(freq,vrange,numpy.transpose(stuffr.comprz_dB(S[:,::-1])),cmap="jet",vmin=-1.0)
                        plt.ylim([s["rmin"],s["rmax"]])
                        plt.xlim([0,s["fmax"]])
                        plt.xlabel("Frequency (MHz)")
                        plt.ylabel("Virtual range (km)")
                        plt.title(f)
                        plt.tight_layout()
                        plt.colorbar()
                        plt.savefig("%s.png"%(f))
                        plt.clf()
                        plt.close()
                    

parser = OptionParser()

parser.add_option("-d", "--dirname",dest="dirname", action="store", type="string",
                  default="/data/chirp/2013.10.09",
                  help="Data directory (Default /data/chirp/2013.10.09).")

parser.add_option("-n", "--station_name",dest="station_name", action="store", type="string",
                  default="*",
                  help="Station name to analyze (default *)")

parser.add_option("-r", "--reanalyze",dest="reanalyze", action="store", type="int",
                  default=0,
                  help="Reanalyze results? (default 0)")

parser.add_option("-l", "--fftlen",dest="fftlen", action="store", type="int",
                  default=8192,
                  help="FFT length (default 8192)")

(op, args) = parser.parse_args()

plot_all(op.dirname,name=op.station_name,window=op.fftlen,reanalyze=op.reanalyze)

