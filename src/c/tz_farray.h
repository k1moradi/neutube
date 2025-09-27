/**@file tz_farray.h
 * @brief routines for float array
 * @author Ting Zhao
 */

#ifndef _TZ_FARRAY_H_
#define _TZ_FARRAY_H_


#include "tz_fftwf.h"




/*float array*/




#include <stdlib.h>
#include "tz_cdefs.h"
#include "tz_utilities.h"

__BEGIN_DECLS
  
/**@addtogroup array_opr_ Array operations
 * @{
 */

/**@addtogroup array_opr_float float array (tz_farray.h)
 * @{
 */

/*
 * farray_max() and farray_min() find the maximum and minimum in an array 
 * respectively. The index of the first occurrence maximum or minimum is 
 * stored in idx if it is not NULL.
 * farray_max_m() and farray_min_m() support masked operation.
 * farray_max_l() and farray_min_l() find the maximum and mininum too, but <idx>
 * is the last occurrence. Their masked verions are farray_max_ml and farray_min_ml.
 * The masked versions returns 0 and sets <idx> to <length> if no element is 
 * available.
 *
 
 */
float farray_max(const float* d1, size_t length, size_t* idx);
float farray_min(const float* d1, size_t length, size_t* idx);
float farray_max_m(const float *d1, size_t length, const int *mask, size_t *idx);
float farray_min_m(const float *d1, size_t length, const int *mask, size_t *idx);
float farray_max_l(const float* d1, size_t length, size_t* idx);
float farray_min_l(const float* d1, size_t length, size_t* idx);
float farray_max_ml(const float *d1, size_t length, const int *mask, size_t *idx);
float farray_min_ml(const float *d1, size_t length, const int *mask, size_t *idx);


/*
 * farray_abs() turns <d1> to its absolute values and farray_neg() reverse the signs
 * of <d1>.
 */

float* farray_abs(float* d1, size_t length);
float* farray_neg(float* d1, size_t length);


/*
 * farray_add() stores the sum of <d1> and <d2> into <d1>. farray_addc stores the
 * sum of <d1> and a scalar <d2> into <d1>. Other arithmetic operations have
 * the same interfaces.
 */
float* farray_add(float* d1,const float* d2, size_t length);
float* farray_addc(float*d1,float d2, size_t length);
float* farray_cadd(float d2, float*d1, size_t length);
float* farray_sub(float* d1,float* d2, size_t length);
float* farray_csub(float d2, float*d1, size_t length);
float* farray_subc(float*d1,float d2, size_t length);
float* farray_mul(float* d1,float* d2, size_t length);
float* farray_mulc(float*d1,float d2, size_t length);
float* farray_div(float* d1,float* d2, size_t length);
float* farray_div_i(float* d1,int* d2, size_t length);
float* farray_divc(float*d1,float d2, size_t length);

/*
 * farray_dot() returns the dot product of <d1> and <d2>.
 */
double farray_dot(const float *d1, const float *d2, size_t length);

/*
 * farray_sqr() calculates the square of <d1>. farray_sqrt() calculates the squre 
 * roots of <d1>.
 */
float* farray_sqr(float* d1, size_t length);
float* farray_sqrt(float* d1, size_t length);

/*
 * farray_scale() scales <d1> so that its minimal value is <min> and maxinum value
 * is <max>.
 */
float* farray_scale(float *d1, size_t length, float min, float max);

float* farray_max2(float* d1,const float* d2, size_t length);
float* farray_min2(float* d1,const float* d2, size_t length);

void farray_threshold(float *d, size_t length, const float *min, const float *max);

/*
 * These are routines for special usage.
 */
float* farray_fun1(float* d1,float *d2,float thr,size_t length);
float* farray_fun1_max(float* d1,float *d2,float thr,size_t length,float *maxv);
float* farray_fun1_i2(float* d1,int *d2,float thr,size_t length);
float* farray_fun1_i2_max(float* d1,int *d2,float thr,size_t length,float *maxv);
float* farray_fun2(float *d1,float *d2,float *d3,size_t length);
float* farray_fun3(float *d1,float *d2,float *d3,float *d4,size_t length);

float* farray_cumsum(float* d1,size_t length);
float* farray_cumsum_m(float* d1,size_t length,const int *mask);
float* farray_cumsum2(float* d1,int width,int height);

float farray_sum(const float* d1,size_t length);
float farray_abssum(float* d1,size_t length);
float farray_sum_h(float* d1,size_t length);
float farray_centroid(const float* d1,size_t length);
double farray_centroid_d(const float* d1,size_t length);
void farray_linsum1(float* d1,float* d2,int width,int height,int bwidth,int bheight);
void farray_linsum2(float* d2,int width2,int height,int bheight);
void farray_linsum1_part(const float* d1,float* d2,int width,int bwidth,
    int start, int end);
void farray_linsum2_part(const float* d1,float* d2,int width2,int height,int bheight, int start, int end);


/*
 * farraycpy() copies an array from <src> to <dst> + <offset> with <length> 
 * elements. farraycpy2 copies an array from <src> to <dst> as an 2d array. The
 * size of <src> <width2> and <height2> must not be greater than the size of
 * <dst> <width1> and <height1>. It starts from (<row_offset>, <col_offset>)
 * iin dst.
 */
void farraycpy(float* dst, const float* src,size_t offset,size_t length);
void farraycpy2(float* dst,float* src,int width1,int height1,int width2,int height2,
	      int row_offset,int col_offset);
void farraycpy3(float* d1,float* d2,int width1,int height1,int depth1,int width2,
	      int height2,int depth2,int row_offset,int col_offset,
	      int dep_offset);
 
//void farray_linsum2(float* d2,int width,int height,int bwidth,int bheight);
float* farray_blocksum(float *d1,int width,int height,int bwidth,int bheight);
float* farray_blockmean(float *d1,int width,int height,int bwidth,int bheight,int itype);
float* farray_shiftdim2(float* d1,int width,int height);
double farray_sum_d(const float *d1,size_t length);
float farray_mean(const float* d1,size_t length);
double farray_mean_d(const float *d1,size_t length);
double farray_mean_d_m(const float *d1,size_t length, const int *mask);
double farray_var(const float *d1, size_t length);
double farray_cov(const float *d1, float *d2, size_t length);
float* farray_malloc(size_t length);
float* farray_calloc(size_t length);
float* farray_malloc2(int width,int height);
float* farray_malloc3(int width,int height,int depth);
float* farray_copy(float* d1,size_t length);
void farray_clean_edge3(float* d1,int width,int height,int depth,int margin);

void farray_printf(const float *d1, size_t length, const char *format);
void farray_print2(const float* d1,int width,int height);
void farray_print(const float* d1, size_t length);
void farray_printf2(FILE *fp, const float* d1,int width,int height);
void farray_print3(const float *d1,int width,int height,int depth);

/**
 * Those binary reading and writing function operate on array size with integer
 * type.
 */
int farray_write(const char* filename,const float *d1, int length);
float* farray_read(const char *filename, int *length);
int farray_read2(const char* filename, float *d1, int *length);


size_t farray_fscanf(FILE *fp, float *d1, size_t length);
float* farray_load_matrix(const char *filepath, float *d, int *m, int *n);
float* farray_load_csv(const char *filepath, float *d, int *m, int *n);


/**
 * farray_qsort(), farray_myqsort() and farray_binsearch() only supports array size 
 * with integer type. <idx> does not have to be initialized when it is not NULL.
 */
void farray_qsort(float *d1,int *idx,int length);
void farray_myqsort(float *d1, int length);
int farray_binsearch(float *d1, int length, float value);

double farray_corrcoef(const float *d1, const float *d2, size_t length);
double farray_corrcoef_m(const float *d1, const float *d2, size_t length, 
		       const int *mask);
double farray_uint8_corrcoef(const float *d1, const tz_uint8 *d2, size_t length);


/**
 * fft related routines only supports array size with integer type.
 */
fftwf_complex* farray_fft(float *d1, int length,int in_place,int preserve);
float* farray_ifft(fftwf_complex *c, int length,int in_place,int preserve,int normalize);
float* farray_convolve(float *d1, int length1,float *d2, int length2,int reflect,
		    float *convolution);
float* farray_avgsmooth(const float* in, size_t length, int wndsize, float *out);
float* farray_curvature(const float* in, size_t length, float *out);
float* farray_cendiff(const float* in, size_t length, float *out);


float* farray_medfilter(const float *in, size_t length, int wndsize, float *out);


int farray_iszero(const float *d, size_t length);
float* farray_contarray(size_t start, size_t end, float *d);


void farray_reverse(float *d, size_t length);

/**@}*/

/**@}*/

__END_DECLS

#endif
