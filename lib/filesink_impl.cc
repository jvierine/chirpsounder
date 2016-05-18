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
#include "filesink_impl.h"

namespace gr {
  namespace juha {

    filesink::sptr
    filesink::make(char *prefix, size_t size, int filesize)
    {
      return gnuradio::get_initial_sptr(new filesink_impl(prefix, size, filesize));
    }

    /*
     * The private constructor
     */
    filesink_impl::filesink_impl(char *prefix, size_t size, int fsize)
      : gr::sync_block("filesink",
                       gr::io_signature::make(1, 1, size),
                       gr::io_signature::make(0, 0, 0))
    {
      filesize=fsize;
      seqnum=1;
      itemsize=size;
      nwritten=0;
      totwritten=0;
      writing=1;
      host_endian=0;
      first=1;
      overflow=0;
      startt=0;
      sr=1;

      printf("making directory %s\n",prefix);
      if(strlen(prefix)>0)
      {
        mkdir(prefix,0777);
        dirname = (char *)malloc(strlen(prefix)+1);
        strcpy(dirname, prefix);
      }else{
        printf("Error, empty directory name");
        exit(1);
        dirname=NULL;
      }
      fname = (char *)malloc(2048);
      tmpfname = (char *)malloc(8192);
      
      printf("creating timestamps file \n");
      if(dirname == NULL)
      {
        sprintf(tmpfname,"timestamps.log");
      }else{
        sprintf(tmpfname,"%s/timestamps.log",dirname);
      }
      logfile = fopen(tmpfname,"w");

      printf("creating file %s/data-%06d.gdf\n",dirname,seqnum);
      sprintf(tmpfname,"%s/data-%06d.gdf",dirname,seqnum);
      file = fopen(tmpfname,"w");
      printf(".");
      seqnum++;

      fflush(logfile);
      fflush(file);
      fflush(stdout);
    }

    /*
     * Our virtual destructor.
     */
    filesink_impl::~filesink_impl()
    {
      void **status;
      status = NULL;
      if(file != NULL)
        fclose(file);
      
      writing=0;
      fflush(logfile);
      fclose(logfile);
      fflush(stdout);
    }

    void filesink_impl::write_to_file2(void *input, int read_count)
    {
      int of;
      struct timeval tv;
      char *byte_input;

      byte_input = (char *)input;
      
      /* 
         loop while everything has been written to disk, 
         
         nwritten, number of samples written into the current file
         i, number of samples written in this call 
         read_count, number of samples that have to be written in this call.
      */
      int i=0;
      while(i < read_count)
      {
        /*
          if the amount of data left in buffer is less than 
          what still fits in the file, then write the remainder 
          to the file
        */
        if( (read_count - i) <= (filesize-nwritten) )
        {
          int outc = fwrite(&byte_input[i*(int)itemsize], itemsize, read_count-i, file);
          if(outc != (read_count-i))
          {
            printf("didn't write everything!");
            exit(1);
          }
          nwritten += (read_count - i);
          i += (read_count-i);
        } 
        else 
        {
          /* 
             otherwise, we write just enough to fill the file and leave
             the rest to the next file.
          */
          int outc = fwrite(&byte_input[i*(int)itemsize], itemsize, (filesize-nwritten), file);
          i += (filesize-nwritten);
          fflush(file);
          fclose(file);

          sprintf(tmpfname,"%s/data-%06d.gdf",dirname,seqnum);
          file = fopen(tmpfname,"wb+");
          printf(".");
          fflush(stdout);
          seqnum++;
          gettimeofday(&tv, NULL);
          fprintf(logfile,"%s %1.20lf %1.3g %d\n",tmpfname,((double)tv.tv_sec)+(double)tv.tv_usec*1.0/1e6,0.0,0); 
          fflush(logfile);
          nwritten = 0;
        }
      }
    }


    int filesink_impl::get_overflow()
    {
      return(overflow);
    }

    void filesink_impl::detect_overflow(uint64_t start, uint64_t end)
    {
      std::vector<gr::tag_t> rx_time_tags;
      get_tags_in_range(rx_time_tags, 0, start, end, pmt::string_to_symbol("rx_time"));
      
      //print all tags
      BOOST_FOREACH(const gr::tag_t &rx_time_tag, rx_time_tags)
      {
        const uint64_t offset = rx_time_tag.offset;
        const pmt::pmt_t &value = rx_time_tag.value;
        
        uint64_t tt0_sec = pmt::to_uint64(pmt::tuple_ref(value, 0));
        double tt0_frac = pmt::to_double(pmt::tuple_ref(value, 1));
        double tt0 = ((double)tt0_sec) + tt0_frac;
        
        printf("Dropped packet. TODO: insert zero samples!\n"); 
        exit(0);
        overflow++;
      }
    }

    void filesink_impl::get_rx_time(int n)
    {
      struct timeval tv;

      std::vector<gr::tag_t> rx_time_tags;
      get_tags_in_range(rx_time_tags, 0, 0, n, pmt::string_to_symbol("rx_time"));
      
      double tt0, t0_frac; 
      uint64_t t0_sec;

      //print all tags
      BOOST_FOREACH(const gr::tag_t &rx_time_tag, rx_time_tags)
      {
        const uint64_t offset = rx_time_tag.offset;
        const pmt::pmt_t &value = rx_time_tag.value;

        t0_sec = pmt::to_uint64(pmt::tuple_ref(value, 0));
        t0_frac = pmt::to_double(pmt::tuple_ref(value, 1));
        tt0 = ((double)t0_sec) + t0_frac;
        t0 = t0_sec*((uint64_t)sr) + (uint64_t)(t0_frac*((double)sr));
        printf("offset %ld %1.2f %1.20f\n", offset, (double)t0_sec, t0_frac);
      }
      gettimeofday(&tv, NULL);
      sprintf(tmpfname,"%s/data",dirname);
      if(startt == 0)
      {
        fprintf(logfile,"%s-%06d.gdf %1.20lf 0 0\n",tmpfname,1,tt0);
      }
      else
        fprintf(logfile,"%s-%06d.gdf %1.20lf 0 0\n",tmpfname,1,(double)startt_s);
      
      first = 0;
    }


    int
    filesink_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      int of;
      struct timeval tv;
      char *byte_ptr;

      byte_ptr = (char *)input_items[0];

      if(first)
        get_rx_time(noutput_items);
      else
        detect_overflow(nitems_read(0),nitems_read(0)+noutput_items);
      
      /* while we wait for starting time, do nothing.. */
      if(nitems_read(0)+noutput_items+t0 < startt)
        return(noutput_items);
      else if(nitems_read(0) + t0 < startt && nitems_read(0)+noutput_items+t0 > startt)
      {
        int startidx = startt - (nitems_read(0)+t0);
        int n = noutput_items-startidx;
        write_to_file2((void *)&byte_ptr[startidx*itemsize],n);
      }
      else
      {    
        write_to_file2((void *)input_items[0],noutput_items);
      }
      return(noutput_items);
    }
  } /* namespace juha */
} /* namespace gr */

