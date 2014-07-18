
/*
Copyright (c) 2012, MAURO BIANCO, UGO VARETTO, SWISS NATIONAL SUPERCOMPUTING CENTRE (CSCS)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Swiss National Supercomputing Centre (CSCS) nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL MAURO BIANCO, UGO VARETTO, OR 
SWISS NATIONAL SUPERCOMPUTING CENTRE (CSCS), BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define MACRO_IMPL(z, n, _)                                             \
  {                                                                     \
    const int ntx = 1;                                                  \
    const int nty = 32;                                                 \
    const int ntz = 8;                                                  \
    dim3 threads(ntx, nty, ntz);                                        \
                                                                        \
    int nx = field ## n.halos[0].r_length(-1);                          \
    int ny = field ## n.halos[1].r_length(0);                           \
    int nz = field ## n.halos[2].r_length(0);                           \
                                                                        \
    int nbx = (nx + ntx - 1) / ntx ;                                    \
    int nby = (ny + nty - 1) / nty ;                                    \
    int nbz = (nz + ntz - 1) / ntz ;                                    \
    dim3 blocks(nbx, nby, nbz);                                         \
                                                                        \
    if (nbx!=0 && nby!=0 && nbz!=0) {                                   \
        m_unpackXLKernel_generic<<<blocks, threads, 0, XL_stream>>>     \
        (field ## n.ptr,                                                \
         reinterpret_cast<typename FOTF_T ## n ::value_type**>(d_msgbufTab_r), \
         wrap_argument(d_msgsize_r+27* n ),                               \
         *(reinterpret_cast<const gridtools::array<gridtools::halo_descriptor,3>*>(&field ## n)), \
         ny, nz,                                                        \
         (field ## n.halos[0].begin()-field ## n.halos[0].minus())      \
         + (field ## n.halos[1].begin())*field ## n.halos[0].total_length() \
         + (field ## n.halos[2].begin())*field ## n.halos[0].total_length() *field ## n.halos[1].total_length(), 0); \
    }                                                                   \
  }


BOOST_PP_REPEAT(noi, MACRO_IMPL, all)
#undef MACRO_IMPL
