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
#include "cspecsink_impl.h"

namespace gr {
  namespace juha {

    cspecsink::sptr
    cspecsink::make(int fftlen, int n_avg)
    {
      return gnuradio::get_initial_sptr
        (new cspecsink_impl(fftlen, n_avg));
    }

    /*
     * The private constructor
     */
    cspecsink_impl::cspecsink_impl(int fftlen, int n_avg)
      : gr::sync_block("cspecsink",
              gr::io_signature::make(2, 2, 8),
              gr::io_signature::make(0, 0, 0))
    {
      nfft=fftlen;
      navg=n_avg;
      spec = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*nfft);
      in0 = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*nfft);
      in1 = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*nfft);
      out0 = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*nfft);
      out1 = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*nfft);
      fftplan0 = fftwf_plan_dft_1d(nfft, in0, out0, FFTW_FORWARD, FFTW_ESTIMATE);
      fftplan1 = fftwf_plan_dft_1d(nfft, in1, out1, FFTW_FORWARD, FFTW_ESTIMATE);
      fname = (char *)malloc(sizeof(char)*1024);
      logfile = fopen("timestamps.log","w+");
      seqnum = 1;
      clear_all();
      counter = 0;
      scounter = 0;
      printf("itemsize %d %d \n",nfft,navg);
    }
    void cspecsink_impl::clear_all()
    {
      int i;
      for(i=0 ; i<nfft ; i++)
      {
	    spec[i][0]=0.0;
	    spec[i][1]=0.0;
	  }
    }

    /*  a*conj(b) = (ax+iay)(bx-iby)  */
    /*  ax*bx + ay*by                 */
    /*  i(ax*by + ay*bx)              */
    void cspecsink_impl::add_to_spec()
    {
      int i;
      fftwf_execute(fftplan0);
      fftwf_execute(fftplan1);
      for(i=0 ; i<nfft ; i++)
      {
        spec[i][0] = spec[i][0] + out0[i][0]*out1[i][0] + out0[i][1]*out1[i][1];
        spec[i][1] = spec[i][1] - out0[i][0]*out1[i][1] + out0[i][1]*out1[i][0];
      }

      scounter++;
      
      if(scounter == navg)
      {
        write_spec();
        clear_all();
        scounter = 0;
      }
    }

    void cspecsink_impl::write_spec()
    {
      struct timeval tv;
      gettimeofday(&tv, NULL);  
      sprintf(fname,"spec-%06d.sdf",seqnum);
      file = fopen(fname,"w+");
      
      fprintf(logfile,"%s %1.20lf\n",fname,((double)tv.tv_sec)+(double)tv.tv_usec*1.0/1e6);
      fflush(logfile);
      
      int outc = fwrite((float *)spec,2*sizeof(float),nfft,file);
      if(outc != nfft)
        throw std::runtime_error ("could not write all samples...");
      printf(".");
      fflush(stdout);
      seqnum++;
      fclose(file);
      clear_all();
    }


    /*
     * Our virtual destructor.
     */
    cspecsink_impl::~cspecsink_impl()
    {
    }

    int
    cspecsink_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      float *inf0 = (float *) input_items[0];
      float *inf1 = (float *) input_items[1];
      int i;
      
      for(i=0 ; i<2*noutput_items ; i+=2)
      {
        if(counter < nfft)
        {
          in0[counter][0] = inf0[i];
          in0[counter][1] = inf0[i+1];
          in1[counter][0] = inf1[i];
          in1[counter][1] = inf1[i+1];
          counter++;
        }
        if(counter == nfft)
        {
          add_to_spec();
          counter = 0;
        }
      }
      return noutput_items;
    }

  } /* namespace juha */
} /* namespace gr */

