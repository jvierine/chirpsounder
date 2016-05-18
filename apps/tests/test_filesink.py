#!/usr/bin/env python
from gnuradio import gr
import juha


#u.set_subdev_spec("A:0")        
u.set_samp_rate(25e6)
u.set_center_freq(126e6)

# create flowgraph
fg = gr.top_block()
dst = filesink2.filesink("testdata",2*gr.sizeof_short, int(25e6))
fg.connect((u, 0), (dst,0))
fg.start()
while(True):
   time.sleep(1)
