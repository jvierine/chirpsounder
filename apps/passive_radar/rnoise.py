import gdf
import numpy
import stuffr
import matplotlib.pyplot as plt
import sys

g0 = gdf.new_gdf("%s/000"%(sys.argv[1]),dtype=numpy.float32,itemsize=8)
g1 = gdf.new_gdf("%s/001"%(sys.argv[1]),dtype=numpy.float32,itemsize=8)

sample_rate=1e5

sample_phase_correction=-2.34008948e-01/1e6#-2.33895247e-02#-2.33895247e-02/1e5#0#-2.31912278e-02/1e5 #-2.33977216e-01/1e6 #((2.33847857e-02/1e6)/1e6)*(1e6/sample_rate)
# 2.4 MHz 
#raw_phase_correction=0#2.33847857e-01/(1e6*(1e6/sample_rate))
# 1 MHz raw_phase_correction=-5.62874951e-02/(1e6*(1e6/sample_rate))
idx=0
flen=1e5
#phase_correction=raw_phase_correction*flen
diff=1e6-794893
mi = -1
phases = numpy.zeros([numpy.floor(g0["max_n"]/flen)],dtype=numpy.float)
N=200#(g0["max_n"]/flen)-5
A=numpy.zeros([N-2,2],dtype=numpy.float32)
A[:,0]=1.0
A[:,1]=numpy.arange(N-2)#numpy.linspace(0.0, N, num=N-2)

#result=numpy.zeros([N],dtype=numpy.complex64)

for i in numpy.arange(N):
    z0 = gdf.read_vec(g0,i*flen+flen+diff,flen)
    z1 = gdf.read_vec(g1,i*flen+flen,flen)*numpy.exp(1.0j*(numpy.arange(float(flen))+float(i)*float(flen))*sample_phase_correction)
#    phcorr = 
#    print phcorr[0:5]
 #   z1 = z1*phcorr
#    stuffr.plot_cts(phcorr)
    
    cc = numpy.fft.ifft(numpy.fft.fft(z0)*numpy.conj(numpy.fft.fft(z1)))
    mi = numpy.argmax(numpy.abs(cc))
    angle = numpy.angle(cc[mi])#*numpy.exp(1.0j*float(i)*2.3191e-2))
    phases[i]=angle
    absv=numpy.abs(cc[mi])
    print "mi %d angle %1.2f %1.2f"%(mi,angle,absv)

phasem = numpy.angle(numpy.exp(1j*phases[0:(N-1)])/numpy.exp(1j*phases[1:N]))
plt.plot(numpy.real(numpy.exp(1j*phases[0:N])),"blue")
plt.plot(numpy.imag(numpy.exp(1j*phases[0:N])),"red")
plt.ylim([-1.2,1.2])
plt.show()
#stuffr.plot_cts(phases)
#plt.plot(phasem)
#plt.show()
print len(phasem)," ",A.shape
xhat = numpy.linalg.lstsq(A,phasem[1:(N-1)])
print numpy.median(phasem)," ",xhat

