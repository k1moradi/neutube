/**@file tz_fftwf.h
 * @brief Routines for fftwf
 * @author Ting Zhao
 */

/* tz_fftw.h.t
 *
 * Initial write: Ting Zhao
 */

#ifndef TZ_fftwf_H_
#define TZ_fftwf_H_

#include "tz_fftw_header.h"

__BEGIN_DECLS

#ifndef R2C_LENGTH
#define R2C_LENGTH(length) ((length)/2+1)
#endif

/* fftwf fftw/fftwf float double/float */

typedef float fftwf_comp;

/* Additional utilites for fftw. */

/* 
 * fftwf_complex_comp() returns the real or imaginary component of each complex
 * number of <array>. The returned value is also an array and the ith element
 * is the:
 *   real component (when comp is 0);
 *   imaginary component (when comp is 1);
 *   amplitude (when comp is 2);
 * of the ith element of <array>. 
 */
fftwf_comp* fftwf_complex_comp(fftwf_complex* array,long length,int comp);

/*
 * fftwf_complete_2d() is obselete.
 */
void fftwf_complete_2d(fftwf_complex* array,int nx,int ny);

/*
 * fftwf_malloc_r2c_1d() allocates memory for output of 1D DFT a real array with
 * <nx> elements. fftwf_malloc_r2c_2d() and fftwf_malloc_r2c_3d() are similar
 * functions for 2D and 3D DFT.
 */
fftwf_complex* fftwf_malloc_r2c_1d(int nx);
fftwf_complex* fftwf_malloc_r2c_2d(int nx,int ny); 
fftwf_complex* fftwf_malloc_r2c_3d(int nx,int ny,int nz);

/*
 * fftwf_pack_c2r_result() removes holes in the c2r transform results. The holes
 * are caused by the padding reqirement of in-place transformation, i.e. a real
 * array needs to be padded by 1 or 2 elements in the last dimension to fit in 
 * the resulted complex transformation. <nlast> is the size of the last
 * dimension and <before> is the product of the all dimensions except the last
 * one.
 */
void fftwf_pack_c2r_result(float *in,int nlast,int nbefore); 


__END_DECLS

#endif
