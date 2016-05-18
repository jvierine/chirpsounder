#!/usr/bin/env python

import numpy as n
import matplotlib.pyplot as plt
import glob

import numpy
import matplotlib.pyplot as plt
import glob
import sys
import os
import scipy
import scipy.signal

# Antennas:
# 
#  A     B
#     E 
#  D     C
#
antenna_radius=35.4

shit_ranges=[[0,40103],
             [173633,175275],
             [241514,242104],
             [329731,330268],
             [333916,334467],
             [336308,336864],
             [454054,454663],
             [496819,497514],
             [501133,501649],
             [504500,505219],
             [662867,663550],
             [669710,670398],
             [672768,673481], 
             [979611,988222],
             [1000820,1002010],
             [1005630,1006500]]

import stuffr
from optparse import OptionParser

def polyfit(z,idx=n.arange(40100,42000),np=7,plot=False):
    n_meas=len(idx)
    A=n.zeros([n_meas,np],dtype=n.complex64)
    t = n.array(idx-n.mean(idx),n.float32)/n.mean(idx)
    for i in range(np):
        A[:,i]=t**float(i)
    xhat=n.linalg.lstsq(A,z[idx])[0]
    model=n.dot(A,xhat)
    if plot:
        pidx=n.arange(n.min(idx),n.max(idx))
        plt.plot(pidx,z[pidx].real,color="pink")
        plt.plot(pidx,z[pidx].imag,color="lightblue")
        plt.plot(idx,model.real,color="red")
        plt.plot(idx,model.imag,color="blue")
        plt.show()

    return(model)

def poly_gc(z,step=2048,plot=False):
    shit_idx = n.zeros(0)
    for si in shit_ranges:
        shit_idx=n.concatenate((shit_idx,n.arange(si[0],si[1])))
    n_z=len(z)
    steps = n_z/step
    shit_idx=n.array(shit_idx,dtype=n.int64)
    z[shit_idx]=0.0
    for i in range(steps):
        idx = n.arange(step)+i*step
        good_idx=n.setdiff1d(idx,shit_idx)
        print(len(good_idx))
        if len(good_idx)>0:
            z[good_idx]=z[good_idx]-polyfit(z,idx=good_idx,np=9)
            if plot:
                plt.plot(z[good_idx].real,color="red")
                plt.plot(z[good_idx].imag,color="blue")
                plt.show()
    return(z)
        
def hanning(L=1000):
    n = numpy.linspace(0.0,L-1,num=L)
    return(0.5*(1.0-numpy.cos(2.0*scipy.constants.pi*n/L)))

def cheb100(L=1000):
    return(scipy.signal.chebwin(L,100.0))

def spectrogram(x,window=1024,wf=cheb100,overlap=0):
    wfv = wf(L=window)
    step = int(window-overlap)

    Nwindow = int(numpy.floor((len(x)-window)/step))
    res = n.zeros([Nwindow,window],dtype=n.complex64)
    for i in range(Nwindow):
        res[i,] = numpy.fft.fftshift(numpy.fft.fft(wfv*x[i*step + numpy.arange(window)]))
    return(res)

#        A     B     C     D     E
#pol0 = ["a0","b0","c0","d0","e0"]
#pol1 = ["a1","b1","c1","d1","e1"]

pol0 = ["ch0","ch1","ch2","ch3","ch4"]
pol1 = ["ch5","ch6","ch7","ch8","ch9"]

sample_rate = 20e6
dec = 500
if_rate = sample_rate/dec
whiten = False
whiten_len = 8192
whiten_n = 30000

dname="/data1"
date="2016.02.22"
fname="sod-1456109874.out"
sounders = [[{'rep':60,'chirpt':594,'name':'sod','rate':0.5e6+8.5,'dur':35, 'cf':12.5e6,'rmin':0,'rmax':1500.0,'fmin':0.0,'fmax':16.0}]]

# remove the any residual band specific variation in noise floor
# that isn't removed by the amplitude domain adaptive filter before chirp downconversion.
def medianEqualize(S):
    for i in numpy.arange(S.shape[0]):
        S[i,:] = S[i,:]/numpy.median(S[i,:])
    return(S)



#
# plot chirps
# 
def analyze_chirp(fname="",window=8192/2,sr=sample_rate/float(dec),s=sounders[0][0],
                  plot=False):
    cr=s["rate"]
    z = n.fromfile(fname,dtype=numpy.complex64)
#    z = poly_gc(z)
#    polyfit(z,np=10)

    S = spectrogram(z,window=window,overlap=window/2.0)
#    S = medianEqualize(S)
    freq = n.linspace(0, (len(z)/sr)*cr , num=S.shape[0])/1e6
    vrange = n.linspace( 3e8*(-(sr/2))/cr, 3e8*(sr/2.0)/cr , num=S.shape[1])/1e3
    if plot:
        plt.pcolormesh(freq,vrange,numpy.transpose(10.0*n.log10(n.abs(S[:,::-1])**2.0)),cmap="jet",vmin=-20,vmax=50.0)
        plt.ylim([0,1500])
        plt.xlim([0,16.0])
        plt.xlabel("Frequency (MHz)")
        plt.ylabel("Virtual range (km)")
        plt.tight_layout()
        plt.colorbar()
        plt.show()                    
    return((freq,vrange,S))



for i in range(len(pol1)):
    ch0 = pol0[i]
    ch1 = pol1[i]
    freq,vrange,S0=analyze_chirp(fname="%s/%s/%s/%s"%(dname,ch0,date,fname))
    freq,vrange,S1=analyze_chirp(fname="%s/%s/%s/%s"%(dname,ch1,date,fname))

    FR = n.transpose((S0*n.conj(S1))[:,::-1])
  # print(FR.shape)
#    zi=n.argmin(n.abs(vrange))
#    chirp = []
#    for fj in range(FR.shape[1]):
#        chirp.append(n.exp(-1j*n.angle(FR[zi,fj])))#*n.abs(FR[zi,fj]))
#        FR[:,fj]=n.exp(-1j*n.angle(FR[zi,fj]))*FR[:,fj]
#    chirp=n.array(chirp,dtype=n.complex64)
#    plt.plot(chirp.real,color="red")
#    plt.plot(chirp.imag,color="blue")
#    plt.show()
    plt.subplot(121)
    plt.pcolormesh(freq,vrange,n.transpose(10.0*n.log10(n.abs(S0*n.conj(S1))[:,::-1])),vmin=-10,vmax=50.0)
    plt.title("%s %s"%(ch0,ch1))
    plt.ylim([0,1500])
    plt.xlim([0,16.0])
    plt.colorbar()

    plt.subplot(122)
    plt.pcolormesh(freq,vrange,n.angle(FR))
    plt.title("%s %s"%(ch0,ch1))
    plt.ylim([0,1500])
    plt.xlim([0,16.0])
    plt.colorbar()
    plt.show()
