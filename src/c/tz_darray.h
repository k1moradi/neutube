/**@file tz_darray.h
 * @brief routines for double array
 * @author Ting Zhao
 */

#ifndef _TZ_DARRAY_H_
#define _TZ_DARRAY_H_


#include "tz_fftw.h"



/*double array*/





#include <stdlib.h>
#include "tz_cdefs.h"
#include "tz_utilities.h"

__BEGIN_DECLS
  
/**@addtogroup array_opr_ Array operations
 * @{
 */

/**@addtogroup array_opr_double double array (tz_darray.h)
 * @{
 */

/*
 * darray_max() and darray_min() find the maximum and minimum in an array 
 * respectively. The index of the first occurrence maximum or minimum is 
 * stored in idx if it is not NULL.
 * darray_max_m() and darray_min_m() support masked operation.
 * darray_max_l() and darray_min_l() find the maximum and mininum too, but <idx>
 * is the last occurrence. Their masked verions are darray_max_ml and darray_min_ml.
 * The masked versions returns 0 and sets <idx> to <length> if no element is 
 * available.
 *
 
 *
 *darray_max_n() ignores NaN values. It returns 0.0 and sets <idx> to the invalid
 * array index if all * values are NaN. 
 
 */
double darray_max(const double* d1, size_t length, size_t* idx);
double darray_min(const double* d1, size_t length, size_t* idx);
double darray_max_m(const double *d1, size_t length, const int *mask, size_t *idx);
double darray_min_m(const double *d1, size_t length, const int *mask, size_t *idx);
double darray_max_l(const double* d1, size_t length, size_t* idx);
double darray_min_l(const double* d1, size_t length, size_t* idx);
double darray_max_ml(const double *d1, size_t length, const int *mask, size_t *idx);
double darray_min_ml(const double *d1, size_t length, const int *mask, size_t *idx);

double darray_max_n(const double *d1, size_t length, size_t *idx);


/*
 * darray_abs() turns <d1> to its absolute values and darray_neg() reverse the signs
 * of <d1>.
 */

double* darray_abs(double* d1, size_t length);
double* darray_neg(double* d1, size_t length);


/*
 * darray_add() stores the sum of <d1> and <d2> into <d1>. darray_addc stores the
 * sum of <d1> and a scalar <d2> into <d1>. Other arithmetic operations have
 * the same interfaces.
 */
double* darray_add(double* d1,const double* d2, size_t length);
double* darray_addc(double*d1,double d2, size_t length);
double* darray_cadd(double d2, double*d1, size_t length);
double* darray_sub(double* d1,double* d2, size_t length);
double* darray_csub(double d2, double*d1, size_t length);
double* darray_subc(double*d1,double d2, size_t length);
double* darray_mul(double* d1,double* d2, size_t length);
double* darray_mulc(double*d1,double d2, size_t length);
double* darray_div(double* d1,double* d2, size_t length);
double* darray_div_i(double* d1,int* d2, size_t length);
double* darray_divc(double*d1,double d2, size_t length);

/*
 * darray_dot() returns the dot product of <d1> and <d2>.
 */
double darray_dot(const double *d1, const double *d2, size_t length);

double darray_dot_n(const double *d1, const double *d2, size_t length);
double darray_sum_n(const double* d1, size_t length);
double darray_mean_n(const double* d1, size_t length);
double darray_corrcoef_n(const double *d1, const double *d2, size_t length);
double darray_dot_nw(const double *d1, const double *d2, size_t length);

/*
 * darray_sqr() calculates the square of <d1>. darray_sqrt() calculates the squre 
 * roots of <d1>.
 */
double* darray_sqr(double* d1, size_t length);
double* darray_sqrt(double* d1, size_t length);

/*
 * darray_scale() scales <d1> so that its minimal value is <min> and maxinum value
 * is <max>.
 */
double* darray_scale(double *d1, size_t length, double min, double max);

double* darray_max2(double* d1,const double* d2, size_t length);
double* darray_min2(double* d1,const double* d2, size_t length);

void darray_threshold(double *d, size_t length, const double *min, const double *max);

/*
 * These are routines for special usage.
 */
double* darray_fun1(double* d1,double *d2,double thr,size_t length);
double* darray_fun1_max(double* d1,double *d2,double thr,size_t length,double *maxv);
double* darray_fun1_i2(double* d1,int *d2,double thr,size_t length);
double* darray_fun1_i2_max(double* d1,int *d2,double thr,size_t length,double *maxv);
double* darray_fun2(double *d1,double *d2,double *d3,size_t length);
double* darray_fun3(double *d1,double *d2,double *d3,double *d4,size_t length);

double* darray_cumsum(double* d1,size_t length);
double* darray_cumsum_m(double* d1,size_t length,const int *mask);
double* darray_cumsum2(double* d1,int width,int height);

double darray_sum(const double* d1,size_t length);
double darray_abssum(double* d1,size_t length);
double darray_sum_h(double* d1,size_t length);
double darray_centroid(const double* d1,size_t length);
double darray_centroid_d(const double* d1,size_t length);
void darray_linsum1(double* d1,double* d2,int width,int height,int bwidth,int bheight);
void darray_linsum2(double* d2,int width2,int height,int bheight);
void darray_linsum1_part(const double* d1,double* d2,int width,int bwidth,
    int start, int end);
void darray_linsum2_part(const double* d1,double* d2,int width2,int height,int bheight, int start, int end);


double darray_sqsum(const double *d1, size_t length);
double darray_norm(const double *d1, size_t length);
double darray_simscore(double *d1, double *d2, size_t length);

/*
 * darraycpy() copies an array from <src> to <dst> + <offset> with <length> 
 * elements. darraycpy2 copies an array from <src> to <dst> as an 2d array. The
 * size of <src> <width2> and <height2> must not be greater than the size of
 * <dst> <width1> and <height1>. It starts from (<row_offset>, <col_offset>)
 * iin dst.
 */
void darraycpy(double* dst, const double* src,size_t offset,size_t length);
void darraycpy2(double* dst,double* src,int width1,int height1,int width2,int height2,
	      int row_offset,int col_offset);
void darraycpy3(double* d1,double* d2,int width1,int height1,int depth1,int width2,
	      int height2,int depth2,int row_offset,int col_offset,
	      int dep_offset);
 
//void darray_linsum2(double* d2,int width,int height,int bwidth,int bheight);
double* darray_blocksum(double *d1,int width,int height,int bwidth,int bheight);
double* darray_blockmean(double *d1,int width,int height,int bwidth,int bheight,int itype);
double* darray_shiftdim2(double* d1,int width,int height);
double darray_sum_d(const double *d1,size_t length);
double darray_mean(const double* d1,size_t length);
double darray_mean_d(const double *d1,size_t length);
double darray_mean_d_m(const double *d1,size_t length, const int *mask);
double darray_var(const double *d1, size_t length);
double darray_cov(const double *d1, double *d2, size_t length);
double* darray_malloc(size_t length);
double* darray_calloc(size_t length);
double* darray_malloc2(int width,int height);
double* darray_malloc3(int width,int height,int depth);
double* darray_copy(double* d1,size_t length);
void darray_clean_edge3(double* d1,int width,int height,int depth,int margin);

void darray_printf(const double *d1, size_t length, const char *format);
void darray_print2(const double* d1,int width,int height);
void darray_print(const double* d1, size_t length);
void darray_printf2(FILE *fp, const double* d1,int width,int height);
void darray_print3(const double *d1,int width,int height,int depth);

/**
 * Those binary reading and writing function operate on array size with integer
 * type.
 */
int darray_write(const char* filename,const double *d1, int length);
double* darray_read(const char *filename, int *length);
int darray_read2(const char* filename, double *d1, int *length);


size_t darray_fscanf(FILE *fp, double *d1, size_t length);
double* darray_load_matrix(const char *filepath, double *d, int *m, int *n);
double* darray_load_csv(const char *filepath, double *d, int *m, int *n);


/**
 * darray_qsort(), darray_myqsort() and darray_binsearch() only supports array size 
 * with integer type. <idx> does not have to be initialized when it is not NULL.
 */
void darray_qsort(double *d1,int *idx,int length);
void darray_myqsort(double *d1, int length);
int darray_binsearch(double *d1, int length, double value);

double darray_corrcoef(const double *d1, const double *d2, size_t length);
double darray_corrcoef_m(const double *d1, const double *d2, size_t length, 
		       const int *mask);
double darray_uint8_corrcoef(const double *d1, const tz_uint8 *d2, size_t length);


/**
 * fft related routines only supports array size with integer type.
 */
fftw_complex* darray_fft(double *d1, int length,int in_place,int preserve);
double* darray_ifft(fftw_complex *c, int length,int in_place,int preserve,int normalize);
double* darray_convolve(double *d1, int length1,double *d2, int length2,int reflect,
		    double *convolution);
double* darray_avgsmooth(const double* in, size_t length, int wndsize, double *out);
double* darray_curvature(const double* in, size_t length, double *out);
double* darray_cendiff(const double* in, size_t length, double *out);


double* darray_medfilter(const double *in, size_t length, int wndsize, double *out);


int darray_iszero(const double *d, size_t length);
double* darray_contarray(size_t start, size_t end, double *d);


void darray_reverse(double *d, size_t length);

/**@}*/

/**@}*/

__END_DECLS

#endif
