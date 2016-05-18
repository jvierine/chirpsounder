import gdf
import numpy
import stuffr
import matplotlib.pyplot as plt
import sys
import os
import h5py

indir=sys.argv[1]
indir0="%s/000"%(indir)
indir1="%s/001"%(indir)
outdir="%s-fix"%(indir)
outdir0="%s-fix/000"%(indir)
outdir1="%s-fix/001"%(indir)

tst = numpy.genfromtxt("%s/timestamps.log"%(indir0),usecols=(1))[1]-1.0

print "outdir %s"%(outdir)
try:
    os.makedirs(outdir)
    os.makedirs(outdir0)
    os.makedirs(outdir1)
except:
    pass

g0 = gdf.new_gdf(indir0,dtype=numpy.float32,itemsize=8)
g1 = gdf.new_gdf(indir1,dtype=numpy.float32,itemsize=8)

sample_rate=1e5
# 1 MHz
#sample_phase_correction=((-5.62874951e-02/1e6)/1e6)*(1e6/sample_rate)
# 2.4 MHz
#-2.33895247e-02
sample_phase_correction=-2.34008948e-01/1e6#-2.33895247e-02/1e5#((--2.33895247e-02/1e6)/1e6)*(1e6/sample_rate)
#raw_phase_correction=-5.62874951e-02/(1e6*(1e6/sample_rate))
idx=0
flen=1e6
#phase_correction=raw_phase_correction*flen
clen=1e6
diff=390873
mi = -1
phases = numpy.zeros([numpy.floor(g0["max_n"]/flen)],dtype=numpy.float)
N=(g0["max_n"]/flen)-5
A=numpy.zeros([N-2,2],dtype=numpy.float32)
A[:,0]=1.0
A[:,1]=numpy.linspace(-N/2.0, N/2.0, num=N-2)

result=numpy.zeros([N],dtype=numpy.complex64)

file_idx=1
timestamp_file0=open("%s/timestamps.log"%(outdir0),"w")
timestamp_file1=open("%s/timestamps.log"%(outdir1),"w")
f = h5py.File("%s/sampler_config.hdf5"%(outdir),"w")
f["version"]=2.0
f["dtype"]="<f4"
f["itemsize"]=9
f["t0"]=0
f["n_channels"]=2
f["sample_rate"]=1e5
f.close()
for i in numpy.arange(N):
    z0 = gdf.read_vec(g0,i*flen+flen+diff,flen)
    z1 = numpy.array(gdf.read_vec(g1,i*flen+flen,flen)*numpy.exp(1.0j*(numpy.arange(float(flen))+float(i)*float(flen))*sample_phase_correction),dtype=numpy.complex64)
#    z1 = numpy.array(z1*numpy.exp(1.0j*(numpy.arange(float(flen)) + float(i))*sample_phase_correction),dtype=numpy.complex64)

    z0.tofile("%s/data-%06d.gdf"%(outdir0,file_idx))
    z1.tofile("%s/data-%06d.gdf"%(outdir1,file_idx))
    timestamp_file0.write("data-%06d.gdf %1.2f 0 0\n"%(file_idx,tst+i*flen/sample_rate))
    timestamp_file1.write("data-%06d.gdf %1.2f 0 0\n"%(file_idx,tst+i*flen/sample_rate))
    file_idx=file_idx+1
    cc = numpy.fft.ifft(numpy.fft.fft(z0)*numpy.conj(numpy.fft.fft(z1)))
    mi = numpy.argmax(numpy.abs(cc))
    angle = numpy.angle(cc[mi])#*numpy.exp(1.0j*phase_correction*float(i)))
    phases[i]=angle
    absv=numpy.abs(cc[mi])
    result[i]=cc[mi]#*numpy.exp(1.0j*phase_correction*float(i))
#    if i%1000 == 0:
    print "mi %d angle %1.2f %1.2f"%(mi,angle,absv)
timestamp_file0.close()
timestamp_file1.close()

