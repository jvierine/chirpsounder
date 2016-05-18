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

#ifndef INCLUDED_JUHA_WHITEN_IMPL_H
#define INCLUDED_JUHA_WHITEN_IMPL_H

#include <juha/whiten.h>
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>

namespace gr {
  namespace juha {

    class whiten_impl : public whiten
    {
     private:
      fftwf_plan fftplan;
      fftwf_plan ifftplan;
      float *specs;
      float *spec, *oldspec, *tmparr;
      float *window;
      fftwf_complex *in;
      fftwf_complex *out;
      fftwf_complex *outf;
      int fftpoints;
      int scounter;
      int counter;
      int n_avg;

      int t_in, t_out, wcount;
      
      void write_spec();
      void add_to_spec();
      void filter_to_spec();
      void clear_all();
      float hanning(int n, int L);
      
      // Nothing to declare in this block.

     public:
      whiten_impl(int nfft, int navg);
      ~whiten_impl();
      
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
	
          // int general_work(int noutput_items,
	  // 	       gr_vector_int &ninput_items,
	  // 	       gr_vector_const_void_star &input_items,
	  // 	       gr_vector_void_star &output_items);
    //  double relative_rate();
    };

  } // namespace juha
} // namespace gr

#endif /* INCLUDED_JUHA_WHITEN_IMPL_H */

