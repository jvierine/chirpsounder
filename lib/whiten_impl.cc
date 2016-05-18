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
#include "whiten_impl.h"

namespace gr {
  namespace juha {

    whiten::sptr
    whiten::make(int nfft, int navg)
    {
      return gnuradio::get_initial_sptr
        (new whiten_impl(nfft, navg));
    }

    /*
     * The private constructor
     */
    whiten_impl::whiten_impl(int nfft, int navg)
      : gr::sync_block("whiten",
		       gr::io_signature::make(1, 1, 8),
		       gr::io_signature::make(1, 1, 8))
    {
      fftpoints = nfft;
      n_avg = navg;
      
      spec = (float *)malloc(sizeof(float)*fftpoints);
      oldspec = (float *)malloc(sizeof(float)*fftpoints);
      in = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*fftpoints);
      out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*fftpoints);
      outf = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex)*fftpoints);
      fftplan = fftwf_plan_dft_1d(fftpoints, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
      ifftplan = fftwf_plan_dft_1d(fftpoints, out, outf, FFTW_BACKWARD, FFTW_ESTIMATE);
      window = (float *)malloc(sizeof(float)*fftpoints);

      t_in=0;
      t_out=0;
      wcount=0;
      for(int i=0 ; i<fftpoints; i++)
      {
	window[i] = 1.0;//hanning(i,fftpoints);
	oldspec[i]=1.0;
      }
      scounter=0;
      counter=0;
      clear_all();

      // force gnuradio to always give us a multiple of nfft samples.
      set_output_multiple(nfft);
    }

    void whiten_impl::write_spec()
    {
      for(int i=0; i<fftpoints; i++)
      {
	if(spec[i] != 0.0){
	  oldspec[i]=1.0/sqrtf(spec[i]/((float)n_avg));
	}
      }
      clear_all();
    }

    void whiten_impl::add_to_spec()
    {
      int i;
      fftwf_execute(fftplan);
      for(i=0 ; i<fftpoints ; i++)
      {
        spec[i] = spec[i] + out[i][0]*out[i][0] + out[i][1]*out[i][1];
      }
      scounter++;
      if(scounter == n_avg)
      {
        write_spec();
        clear_all();
        scounter = 0;
      }
    }

    void whiten_impl::filter_to_spec()
    {
      int i;
      for(i=0 ; i<fftpoints ; i++)
      {
        out[i][0] = out[i][0]*oldspec[i];
        out[i][1] = out[i][1]*oldspec[i];
      }
      fftwf_execute(ifftplan);
    }

    void whiten_impl::clear_all()
    {
      int i;
      for(i=0 ; i<fftpoints ; i++)
      {
	spec[i]=0.0;
      }
    }

    float whiten_impl::hanning(int n, int L)
    {
      return(0.5*(1.0-cos(2.0*3.14159265358979311600*((float)n)/((float)L))));
    }


    /*
     * Our virtual destructor.
     */
    whiten_impl::~whiten_impl()
    {
    }

    int whiten_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      int ninput;
      float *inf = (float *) input_items[0];
      float *goutf = (float *) output_items[0];
      int nconsumed;
      float *outff;

      ninput = noutput_items;
      nconsumed = 0;
      
      for(int i=0 ; i<2*ninput ; i+=2)
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
	  filter_to_spec();
	  outff = (float *)outf;
	  for(int j=0 ; j<2*fftpoints ; j+=2)
	  {
	    goutf[j + 2*nconsumed*fftpoints]=outff[j];
	    goutf[j+1 + 2*nconsumed*fftpoints]=outff[j+1];
	  }
	  counter = 0;
	  nconsumed++;
        }
      }
      return(ninput);
    }
  } /* namespace juha */
} /* namespace gr */

