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

#ifndef INCLUDED_JUHA_SPECSINK_IMPL_H
#define INCLUDED_JUHA_SPECSINK_IMPL_H

#include <juha/specsink.h>
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>


namespace gr {
  namespace juha {

    class specsink_impl : public specsink
    {
    private:
      int n;
      char *fname;
      FILE *file;
      FILE *logfile;
      int seqnum;     /* file number */
      int fftpoints;
      int nffts;
      int counter;   /* sample counter */
      int scounter; /* spectrum counter */      
      int nignore;
      fftwf_plan fftplan;
      float *specs;
      float *spec, *oldspec, *tmparr;
      float *window;
      fftwf_complex *in;
      fftwf_complex *out;
      void clear_all();
      void add_to_spec();
      void write_spec();
      float hanning(int n, int L);
      
      // Nothing to declare in this block.
      
    public:
      specsink_impl(int fftlen, int n_avg);
      ~specsink_impl();
      
      float *get_spec();
      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace juha
} // namespace gr

#endif /* INCLUDED_JUHA_SPECSINK_IMPL_H */

