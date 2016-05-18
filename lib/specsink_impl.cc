/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "specsink_impl.h"

namespace gr {
  namespace juha {

    specsink::sptr
    specsink::make(int fftlen, int n_avg)
    {
      return gnuradio::get_initial_sptr
        (new specsink_impl(fftlen, n_avg));
    }

    /*
     * The private constructor
     */
    specsink_impl::specsink_impl(int fftlen, int n_avg)
      : gr::sync_block("specsink",
              gr::io_signature::make(1, 1, 8),
              gr::io_signature::make(0, 0, 0))
    {
      fftpoints = fftlen;
      nffts = n_avg;
      
      spec = (float *)malloc(sizeof(float)*fftpoints);
      oldspec = (float *)malloc(sizeof(float)*fftpoints);
      in = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*fftpoints);
      out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*fftpoints);
      fftplan = fftwf_plan_dft_1d(fftpoints, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
      fname = (char *)malloc(sizeof(char)*1024);
      window = (float *)malloc(sizeof(float)*fftlen);
      for(int i=0 ; i<fftlen; i++)
      {
	window[i] = hanning(i,fftlen);
      }
      logfile = fopen("timestamps.log","w+");
      seqnum = 1;
      clear_all();
      counter = 0;
      scounter = 0;
      printf("itemsize %d %d %d\n",fftpoints,nffts);

    }

    float specsink_impl::hanning(int n, int L)
    {
      return(0.5*(1.0-cos(2.0*3.14159265358979311600*((float)n)/((float)L))));
    }

    void specsink_impl::clear_all()
    {
      int i;
      for(i=0 ; i<fftpoints ; i++)
      {
	    spec[i]=0.0;
	  }
    }

    void specsink_impl::write_spec()
    {
      struct timeval tv;
      
      gettimeofday(&tv, NULL);  
      sprintf(fname,"spec-%06d.sdf",seqnum);
      file = fopen(fname,"w+");
      
      fprintf(logfile,"%s %1.20lf\n",fname,((double)tv.tv_sec)+(double)tv.tv_usec*1.0/1e6);
      fflush(logfile);
      
      int outc = fwrite(spec,sizeof(float),fftpoints,file);
      if(outc != fftpoints)
        throw std::runtime_error ("could not write all samples...");

      for(int i=0; i<fftpoints; i++)
      {
	oldspec[i]=1.0/sqrtf(spec[i]/((float)nffts));
      }

      printf(".");
      fflush(stdout);
      seqnum++;
      fclose(file);
      clear_all();
    }


    void specsink_impl::add_to_spec()
    {
      int i;
      fftwf_execute(fftplan);
      for(i=0 ; i<fftpoints ; i++)
      {
        spec[i] = spec[i] + out[i][0]*out[i][0] + out[i][1]*out[i][1];
      }
      scounter++;
      
      if(scounter == nffts)
      {
        write_spec();
        clear_all();
        scounter = 0;
      }
    }

    float *specsink_impl::get_spec()
    {
      return(oldspec);
    }


    /*
     * Our virtual destructor.
     */
    specsink_impl::~specsink_impl()
    {
    }

    int
    specsink_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      float *inf = (float *) input_items[0];
      int i;
      
      for(i=0 ; i<2*noutput_items ; i+=2)
      {
        if(counter < fftpoints)
        {
          in[counter][0] = window[counter]*inf[i];
          in[counter][1] = window[counter]*inf[i+1];
          counter++;
        }
        if(counter == fftpoints)
        {
          add_to_spec();
          counter = 0;
        }
      }
      return noutput_items;
    }

  } /* namespace juha */
} /* namespace gr */

