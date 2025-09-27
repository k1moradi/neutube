/**@file tz_i64array.h
 * @brief routines for int64_t array
 * @author Ting Zhao
 */

#ifndef _TZ_I64ARRAY_H_
#define _TZ_I64ARRAY_H_








#include <stdlib.h>
#include "tz_cdefs.h"
#include "tz_utilities.h"

__BEGIN_DECLS
  
/**@addtogroup array_opr_ Array operations
 * @{
 */

/**@addtogroup array_opr_int64_t int64_t array (tz_i64array.h)
 * @{
 */

/*
 * i64array_max() and i64array_min() find the maximum and minimum in an array 
 * respectively. The index of the first occurrence maximum or minimum is 
 * stored in idx if it is not NULL.
 * i64array_max_m() and i64array_min_m() support masked operation.
 * i64array_max_l() and i64array_min_l() find the maximum and mininum too, but <idx>
 * is the last occurrence. Their masked verions are i64array_max_ml and i64array_min_ml.
 * The masked versions returns 0 and sets <idx> to <length> if no element is 
 * available.
 *
 
 */
int64_t i64array_max(const int64_t* d1, size_t length, size_t* idx);
int64_t i64array_min(const int64_t* d1, size_t length, size_t* idx);
int64_t i64array_max_m(const int64_t *d1, size_t length, const int *mask, size_t *idx);
int64_t i64array_min_m(const int64_t *d1, size_t length, const int *mask, size_t *idx);
int64_t i64array_max_l(const int64_t* d1, size_t length, size_t* idx);
int64_t i64array_min_l(const int64_t* d1, size_t length, size_t* idx);
int64_t i64array_max_ml(const int64_t *d1, size_t length, const int *mask, size_t *idx);
int64_t i64array_min_ml(const int64_t *d1, size_t length, const int *mask, size_t *idx);


/*
 * i64array_abs() turns <d1> to its absolute values and i64array_neg() reverse the signs
 * of <d1>.
 */


/*
 * i64array_add() stores the sum of <d1> and <d2> into <d1>. i64array_addc stores the
 * sum of <d1> and a scalar <d2> into <d1>. Other arithmetic operations have
 * the same interfaces.
 */
int64_t* i64array_add(int64_t* d1,const int64_t* d2, size_t length);
int64_t* i64array_addc(int64_t*d1,int64_t d2, size_t length);
int64_t* i64array_cadd(int64_t d2, int64_t*d1, size_t length);
int64_t* i64array_sub(int64_t* d1,int64_t* d2, size_t length);
int64_t* i64array_csub(int64_t d2, int64_t*d1, size_t length);
int64_t* i64array_subc(int64_t*d1,int64_t d2, size_t length);
int64_t* i64array_mul(int64_t* d1,int64_t* d2, size_t length);
int64_t* i64array_mulc(int64_t*d1,int64_t d2, size_t length);
int64_t* i64array_div(int64_t* d1,int64_t* d2, size_t length);
int64_t* i64array_div_i(int64_t* d1,int* d2, size_t length);
int64_t* i64array_divc(int64_t*d1,int64_t d2, size_t length);

/*
 * i64array_dot() returns the dot product of <d1> and <d2>.
 */
double i64array_dot(const int64_t *d1, const int64_t *d2, size_t length);

/*
 * i64array_sqr() calculates the square of <d1>. i64array_sqrt() calculates the squre 
 * roots of <d1>.
 */
int64_t* i64array_sqr(int64_t* d1, size_t length);
int64_t* i64array_sqrt(int64_t* d1, size_t length);

/*
 * i64array_scale() scales <d1> so that its minimal value is <min> and maxinum value
 * is <max>.
 */
int64_t* i64array_scale(int64_t *d1, size_t length, int64_t min, int64_t max);

int64_t* i64array_max2(int64_t* d1,const int64_t* d2, size_t length);
int64_t* i64array_min2(int64_t* d1,const int64_t* d2, size_t length);

void i64array_threshold(int64_t *d, size_t length, const int64_t *min, const int64_t *max);

/*
 * These are routines for special usage.
 */
int64_t* i64array_fun1(int64_t* d1,int64_t *d2,int64_t thr,size_t length);
int64_t* i64array_fun1_max(int64_t* d1,int64_t *d2,int64_t thr,size_t length,int64_t *maxv);
int64_t* i64array_fun1_i2(int64_t* d1,int *d2,int64_t thr,size_t length);
int64_t* i64array_fun1_i2_max(int64_t* d1,int *d2,int64_t thr,size_t length,int64_t *maxv);
int64_t* i64array_fun2(int64_t *d1,int64_t *d2,int64_t *d3,size_t length);
int64_t* i64array_fun3(int64_t *d1,int64_t *d2,int64_t *d3,int64_t *d4,size_t length);

int64_t* i64array_cumsum(int64_t* d1,size_t length);
int64_t* i64array_cumsum_m(int64_t* d1,size_t length,const int *mask);
int64_t* i64array_cumsum2(int64_t* d1,int width,int height);

int64_t i64array_sum(const int64_t* d1,size_t length);
int64_t i64array_abssum(int64_t* d1,size_t length);
int64_t i64array_sum_h(int64_t* d1,size_t length);
int64_t i64array_centroid(const int64_t* d1,size_t length);
double i64array_centroid_d(const int64_t* d1,size_t length);
void i64array_linsum1(int64_t* d1,int64_t* d2,int width,int height,int bwidth,int bheight);
void i64array_linsum2(int64_t* d2,int width2,int height,int bheight);
void i64array_linsum1_part(const int64_t* d1,int64_t* d2,int width,int bwidth,
    int start, int end);
void i64array_linsum2_part(const int64_t* d1,int64_t* d2,int width2,int height,int bheight, int start, int end);


/*
 * i64arraycpy() copies an array from <src> to <dst> + <offset> with <length> 
 * elements. i64arraycpy2 copies an array from <src> to <dst> as an 2d array. The
 * size of <src> <width2> and <height2> must not be greater than the size of
 * <dst> <width1> and <height1>. It starts from (<row_offset>, <col_offset>)
 * iin dst.
 */
void i64arraycpy(int64_t* dst, const int64_t* src,size_t offset,size_t length);
void i64arraycpy2(int64_t* dst,int64_t* src,int width1,int height1,int width2,int height2,
	      int row_offset,int col_offset);
void i64arraycpy3(int64_t* d1,int64_t* d2,int width1,int height1,int depth1,int width2,
	      int height2,int depth2,int row_offset,int col_offset,
	      int dep_offset);
 
//void i64array_linsum2(int64_t* d2,int width,int height,int bwidth,int bheight);
int64_t* i64array_blocksum(int64_t *d1,int width,int height,int bwidth,int bheight);
int64_t* i64array_blockmean(int64_t *d1,int width,int height,int bwidth,int bheight,int itype);
int64_t* i64array_shiftdim2(int64_t* d1,int width,int height);
double i64array_sum_d(const int64_t *d1,size_t length);
int64_t i64array_mean(const int64_t* d1,size_t length);
double i64array_mean_d(const int64_t *d1,size_t length);
double i64array_mean_d_m(const int64_t *d1,size_t length, const int *mask);
double i64array_var(const int64_t *d1, size_t length);
double i64array_cov(const int64_t *d1, int64_t *d2, size_t length);
int64_t* i64array_malloc(size_t length);
int64_t* i64array_calloc(size_t length);
int64_t* i64array_malloc2(int width,int height);
int64_t* i64array_malloc3(int width,int height,int depth);
int64_t* i64array_copy(int64_t* d1,size_t length);
void i64array_clean_edge3(int64_t* d1,int width,int height,int depth,int margin);

void i64array_printf(const int64_t *d1, size_t length, const char *format);
void i64array_print2(const int64_t* d1,int width,int height);
void i64array_print(const int64_t* d1, size_t length);
void i64array_printf2(FILE *fp, const int64_t* d1,int width,int height);
void i64array_print3(const int64_t *d1,int width,int height,int depth);

/**
 * Those binary reading and writing function operate on array size with integer
 * type.
 */
int i64array_write(const char* filename,const int64_t *d1, int length);
int64_t* i64array_read(const char *filename, int *length);
int i64array_read2(const char* filename, int64_t *d1, int *length);



/**
 * i64array_qsort(), i64array_myqsort() and i64array_binsearch() only supports array size 
 * with integer type. <idx> does not have to be initialized when it is not NULL.
 */
void i64array_qsort(int64_t *d1,int *idx,int length);
void i64array_myqsort(int64_t *d1, int length);
int i64array_binsearch(int64_t *d1, int length, int64_t value);

double i64array_corrcoef(const int64_t *d1, const int64_t *d2, size_t length);
double i64array_corrcoef_m(const int64_t *d1, const int64_t *d2, size_t length, 
		       const int *mask);
double i64array_uint8_corrcoef(const int64_t *d1, const tz_uint8 *d2, size_t length);



int64_t* i64array_medfilter(const int64_t *in, size_t length, int wndsize, int64_t *out);



void i64array_reverse(int64_t *d, size_t length);

/**@}*/

/**@}*/

__END_DECLS

#endif
