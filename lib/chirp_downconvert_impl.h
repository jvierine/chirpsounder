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

#ifndef INCLUDED_JUHA_CHIRP_DOWNCONVERT_IMPL_H
#define INCLUDED_JUHA_CHIRP_DOWNCONVERT_IMPL_H

#include <juha/chirp_downconvert.h>
#include <pthread.h>
#include <fftw3.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TABL 8192
#define OUTBUFFSIZE 4096
#define DET_BUFF_SIZE 4000
#define NOUTBUFF 1

typedef struct complex_float_str {
  float re;
  float im;
} complex_float;

enum ChirpCloseFileEnum  { WRITE_TO_FILE = 0, CLOSE_FILE = 1, DO_NOTHING = 2, FINALIZE_FILE = 3 };

namespace gr {
  namespace juha {
    class chirp_downconvert_impl : public chirp_downconvert
    {
    private:
      
      // Nothing to declare in this block.
      void complex_mul(complex_float *a, complex_float *res);
      void complex_add(complex_float *a, complex_float *res);
      void complex_scale1(complex_float *z);
      int first;
      int done;
      int waiting;
      int overflow;
      
      uint64_t t0_sec;
      double t0_frac;
      double t0;
      
      uint64_t t0_u64;
      uint64_t startt_u64;
      uint64_t sr_u64;
      uint64_t dur_u64;
      uint64_t sample_count;
      
      double chirpt;
      
      double sr;
      double dt;
      int dec;
      double f0;
      double dur;
      double rate;
      double startt;
      char fname[2048];

      double lag;
      complex_float outs;
      int closealive;
      
      complex_float outbuff[OUTBUFFSIZE];
      int obuffi;
      int buffi;
      complex_float sintab[TABL];
      int ncurr;
      
      ChirpCloseFileEnum closefile;

      pthread_t write_thread;
      pthread_attr_t attr;
      pthread_mutex_t fileclose_mutex;
      pthread_cond_t file_can_be_closed;
      
      uint64_t last_check;
      uint64_t last_check_i;
      
      int f;
      // read the rx_time tag
      void get_rx_time();
      // time when chirp start, and duration
      

      int get_overflow();
      void detect_overflow(uint64_t a, uint64_t b);
      
      //      void set_samp_rate(double s);
      

      
      void add_and_advance_phasor(complex_float *a, complex_float *mean);
      
      void write_phasor(complex_float *out);

     public:
      void file_closer();            
      void set_timing(double t, double dur);
      void set_chirp_par(double f0, double rate);
      void set_samp_rate(double s);
      void set_f0(double f);
      int is_done();
      void set_fname(char *fname);
      void set_dec(int dec);
      double get_lag();


      chirp_downconvert_impl();
      ~chirp_downconvert_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace juha
} // namespace gr

#endif /* INCLUDED_JUHA_CHIRP_DOWNCONVERT_IMPL_H */

