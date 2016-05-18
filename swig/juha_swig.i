/* -*- c++ -*- */

#define JUHA_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "juha_swig_doc.i"

%{
#include "juha/filesink.h"
#include "juha/specsink.h"
#include "juha/cspecsink.h"
#include "juha/chirp_downconvert.h"
#include "juha/whiten.h"
%}


%include "juha/filesink.h"
GR_SWIG_BLOCK_MAGIC2(juha, filesink);
%include "juha/specsink.h"
GR_SWIG_BLOCK_MAGIC2(juha, specsink);
%include "juha/cspecsink.h"
GR_SWIG_BLOCK_MAGIC2(juha, cspecsink);
%include "juha/chirp_downconvert.h"
GR_SWIG_BLOCK_MAGIC2(juha, chirp_downconvert);
%include "juha/whiten.h"
GR_SWIG_BLOCK_MAGIC2(juha, whiten);
