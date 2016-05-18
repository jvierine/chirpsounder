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

#ifndef INCLUDED_JUHA_CSPECSINK_IMPL_H
#define INCLUDED_JUHA_CSPECSINK_IMPL_H

#include <juha/cspecsink.h>
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>

namespace gr {
  namespace juha {

    class cspecsink_impl : public cspecsink
    {
     private:
      int nfft;
      int navg;
      FILE *logfile;
      FILE *file;
      int scounter, seqnum;
      int counter;
      fftwf_plan fftplan0;
      fftwf_plan fftplan1;
      float *tmparr;
      char *fname;
      
      fftwf_complex *in0,*in1;
      fftwf_complex *out0,*out1;
      fftwf_complex *spec; 
      void clear_all();
      void add_to_spec();
      void write_spec();
      
      // Nothing to declare in this block.
      
    public:
      cspecsink_impl(int fftlen, int n_avg);
      ~cspecsink_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace juha
} // namespace gr

#endif /* INCLUDED_JUHA_CSPECSINK_IMPL_H */

