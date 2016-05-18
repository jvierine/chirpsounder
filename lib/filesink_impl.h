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

#ifndef INCLUDED_JUHA_FILESINK_IMPL_H
#define INCLUDED_JUHA_FILESINK_IMPL_H

#include <juha/filesink.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace gr {
  namespace juha {

    class filesink_impl : public filesink
    {
    private:
      int filesize;
      size_t itemsize;
      int nwritten;
      int totwritten;
      char *fname;
      char *dirname;
      char *tmpfname;
      int seqnum;
      int writing;
      int host_endian;
      int first;
      int overflow;
      int sr;
      
      FILE *file;
      FILE *logfile;
      FILE *closeFile;
      FILE *nextFile;
      
      uint64_t t0;
      uint64_t startt;
      int startt_s;
      
      // this is a more general routine for all
      // item size, but has no endian conversion
      void write_to_file2(void *input, int read_count);
      void detect_overflow(uint64_t start, uint64_t end);
      void get_rx_time(int n);

      // Nothing to declare in this block.

     public:
      filesink_impl(char *prefix, size_t size, int filesize);
      ~filesink_impl();
      int get_overflow();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace juha
} // namespace gr

#endif /* INCLUDED_JUHA_FILESINK_IMPL_H */

