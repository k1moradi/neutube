/**@file tz_fftw.h
 * @brief Routines for fftw
 * @author Ting Zhao
 */

/* tz_fftw.h.t
 *
 * Initial write: Ting Zhao
 */

#ifndef TZ_fftw_H_
#define TZ_fftw_H_

#include "tz_fftw_header.h"

__BEGIN_DECLS

#ifndef R2C_LENGTH
#define R2C_LENGTH(length) ((length)/2+1)
#endif

/* fftw fftw/fftwf double double/float */

typedef double fftw_comp;

/* Additional utilites for fftw. */

/* 
 * fftw_complex_comp() returns the real or imaginary component of each complex
 * number of <array>. The returned value is also an array and the ith element
 * is the:
 *   real component (when comp is 0);
 *   imaginary component (when comp is 1);
 *   amplitude (when comp is 2);
 * of the ith element of <array>. 
 */
fftw_comp* fftw_complex_comp(fftw_complex* array,long length,int comp);

/*
 * fftw_complete_2d() is obselete.
 */
void fftw_complete_2d(fftw_complex* array,int nx,int ny);

/*
 * fftw_malloc_r2c_1d() allocates memory for output of 1D DFT a real array with
 * <nx> elements. fftw_malloc_r2c_2d() and fftw_malloc_r2c_3d() are similar
 * functions for 2D and 3D DFT.
 */
fftw_complex* fftw_malloc_r2c_1d(int nx);
fftw_complex* fftw_malloc_r2c_2d(int nx,int ny); 
fftw_complex* fftw_malloc_r2c_3d(int nx,int ny,int nz);

/*
 * fftw_pack_c2r_result() removes holes in the c2r transform results. The holes
 * are caused by the padding reqirement of in-place transformation, i.e. a real
 * array needs to be padded by 1 or 2 elements in the last dimension to fit in 
 * the resulted complex transformation. <nlast> is the size of the last
 * dimension and <before> is the product of the all dimensions except the last
 * one.
 */
void fftw_pack_c2r_result(double *in,int nlast,int nbefore); 


__END_DECLS

#endif
