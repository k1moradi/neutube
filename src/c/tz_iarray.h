/**@file tz_iarray.h
 * @brief routines for int array
 * @author Ting Zhao
 */

#ifndef _TZ_IARRAY_H_
#define _TZ_IARRAY_H_






/*int array*/



#include <stdlib.h>
#include "tz_cdefs.h"
#include "tz_utilities.h"

__BEGIN_DECLS
  
/**@addtogroup array_opr_ Array operations
 * @{
 */

/**@addtogroup array_opr_int int array (tz_iarray.h)
 * @{
 */

/*
 * iarray_max() and iarray_min() find the maximum and minimum in an array 
 * respectively. The index of the first occurrence maximum or minimum is 
 * stored in idx if it is not NULL.
 * iarray_max_m() and iarray_min_m() support masked operation.
 * iarray_max_l() and iarray_min_l() find the maximum and mininum too, but <idx>
 * is the last occurrence. Their masked verions are iarray_max_ml and iarray_min_ml.
 * The masked versions returns 0 and sets <idx> to <length> if no element is 
 * available.
 *
 
 */
int iarray_max(const int* d1, size_t length, size_t* idx);
int iarray_min(const int* d1, size_t length, size_t* idx);
int iarray_max_m(const int *d1, size_t length, const int *mask, size_t *idx);
int iarray_min_m(const int *d1, size_t length, const int *mask, size_t *idx);
int iarray_max_l(const int* d1, size_t length, size_t* idx);
int iarray_min_l(const int* d1, size_t length, size_t* idx);
int iarray_max_ml(const int *d1, size_t length, const int *mask, size_t *idx);
int iarray_min_ml(const int *d1, size_t length, const int *mask, size_t *idx);


/*
 * iarray_abs() turns <d1> to its absolute values and iarray_neg() reverse the signs
 * of <d1>.
 */

int* iarray_abs(int* d1, size_t length);
int* iarray_neg(int* d1, size_t length);


/*
 * iarray_add() stores the sum of <d1> and <d2> into <d1>. iarray_addc stores the
 * sum of <d1> and a scalar <d2> into <d1>. Other arithmetic operations have
 * the same interfaces.
 */
int* iarray_add(int* d1,const int* d2, size_t length);
int* iarray_addc(int*d1,int d2, size_t length);
int* iarray_cadd(int d2, int*d1, size_t length);
int* iarray_sub(int* d1,int* d2, size_t length);
int* iarray_csub(int d2, int*d1, size_t length);
int* iarray_subc(int*d1,int d2, size_t length);
int* iarray_mul(int* d1,int* d2, size_t length);
int* iarray_mulc(int*d1,int d2, size_t length);
int* iarray_div(int* d1,int* d2, size_t length);
int* iarray_div_i(int* d1,int* d2, size_t length);
int* iarray_divc(int*d1,int d2, size_t length);

/*
 * iarray_dot() returns the dot product of <d1> and <d2>.
 */
double iarray_dot(const int *d1, const int *d2, size_t length);

/*
 * iarray_sqr() calculates the square of <d1>. iarray_sqrt() calculates the squre 
 * roots of <d1>.
 */
int* iarray_sqr(int* d1, size_t length);
int* iarray_sqrt(int* d1, size_t length);

/*
 * iarray_scale() scales <d1> so that its minimal value is <min> and maxinum value
 * is <max>.
 */
int* iarray_scale(int *d1, size_t length, int min, int max);

int* iarray_max2(int* d1,const int* d2, size_t length);
int* iarray_min2(int* d1,const int* d2, size_t length);

void iarray_threshold(int *d, size_t length, const int *min, const int *max);

/*
 * These are routines for special usage.
 */
int* iarray_fun1(int* d1,int *d2,int thr,size_t length);
int* iarray_fun1_max(int* d1,int *d2,int thr,size_t length,int *maxv);
int* iarray_fun1_i2(int* d1,int *d2,int thr,size_t length);
int* iarray_fun1_i2_max(int* d1,int *d2,int thr,size_t length,int *maxv);
int* iarray_fun2(int *d1,int *d2,int *d3,size_t length);
int* iarray_fun3(int *d1,int *d2,int *d3,int *d4,size_t length);

int* iarray_cumsum(int* d1,size_t length);
int* iarray_cumsum_m(int* d1,size_t length,const int *mask);
int* iarray_cumsum2(int* d1,int width,int height);

int iarray_sum(const int* d1,size_t length);
int iarray_abssum(int* d1,size_t length);
int iarray_sum_h(int* d1,size_t length);
int iarray_centroid(const int* d1,size_t length);
double iarray_centroid_d(const int* d1,size_t length);
void iarray_linsum1(int* d1,int* d2,int width,int height,int bwidth,int bheight);
void iarray_linsum2(int* d2,int width2,int height,int bheight);
void iarray_linsum1_part(const int* d1,int* d2,int width,int bwidth,
    int start, int end);
void iarray_linsum2_part(const int* d1,int* d2,int width2,int height,int bheight, int start, int end);


/*
 * iarraycpy() copies an array from <src> to <dst> + <offset> with <length> 
 * elements. iarraycpy2 copies an array from <src> to <dst> as an 2d array. The
 * size of <src> <width2> and <height2> must not be greater than the size of
 * <dst> <width1> and <height1>. It starts from (<row_offset>, <col_offset>)
 * iin dst.
 */
void iarraycpy(int* dst, const int* src,size_t offset,size_t length);
void iarraycpy2(int* dst,int* src,int width1,int height1,int width2,int height2,
	      int row_offset,int col_offset);
void iarraycpy3(int* d1,int* d2,int width1,int height1,int depth1,int width2,
	      int height2,int depth2,int row_offset,int col_offset,
	      int dep_offset);
 
//void iarray_linsum2(int* d2,int width,int height,int bwidth,int bheight);
int* iarray_blocksum(int *d1,int width,int height,int bwidth,int bheight);
int* iarray_blockmean(int *d1,int width,int height,int bwidth,int bheight,int itype);
int* iarray_shiftdim2(int* d1,int width,int height);
double iarray_sum_d(const int *d1,size_t length);
int iarray_mean(const int* d1,size_t length);
double iarray_mean_d(const int *d1,size_t length);
double iarray_mean_d_m(const int *d1,size_t length, const int *mask);
double iarray_var(const int *d1, size_t length);
double iarray_cov(const int *d1, int *d2, size_t length);
int* iarray_malloc(size_t length);
int* iarray_calloc(size_t length);
int* iarray_malloc2(int width,int height);
int* iarray_malloc3(int width,int height,int depth);
int* iarray_copy(int* d1,size_t length);
void iarray_clean_edge3(int* d1,int width,int height,int depth,int margin);

void iarray_printf(const int *d1, size_t length, const char *format);
void iarray_print2(const int* d1,int width,int height);
void iarray_print(const int* d1, size_t length);
void iarray_printf2(FILE *fp, const int* d1,int width,int height);
void iarray_print3(const int *d1,int width,int height,int depth);

/**
 * Those binary reading and writing function operate on array size with integer
 * type.
 */
int iarray_write(const char* filename,const int *d1, int length);
int* iarray_read(const char *filename, int *length);
int iarray_read2(const char* filename, int *d1, int *length);


size_t iarray_fscanf(FILE *fp, int *d1, size_t length);
int* iarray_load_matrix(const char *filepath, int *d, int *m, int *n);
int* iarray_load_csv(const char *filepath, int *d, int *m, int *n);


/**
 * iarray_qsort(), iarray_myqsort() and iarray_binsearch() only supports array size 
 * with integer type. <idx> does not have to be initialized when it is not NULL.
 */
void iarray_qsort(int *d1,int *idx,int length);
void iarray_myqsort(int *d1, int length);
int iarray_binsearch(int *d1, int length, int value);

double iarray_corrcoef(const int *d1, const int *d2, size_t length);
double iarray_corrcoef_m(const int *d1, const int *d2, size_t length, 
		       const int *mask);
double iarray_uint8_corrcoef(const int *d1, const tz_uint8 *d2, size_t length);



int* iarray_medfilter(const int *in, size_t length, int wndsize, int *out);


int iarray_iszero(const int *d, size_t length);
int* iarray_contarray(size_t start, size_t end, int *d);


void iarray_reverse(int *d, size_t length);

/**@}*/

/**@}*/

__END_DECLS

#endif
