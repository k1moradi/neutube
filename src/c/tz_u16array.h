/**@file tz_u16array.h
 * @brief routines for tz_uint16 array
 * @author Ting Zhao
 */

#ifndef _TZ_U16ARRAY_H_
#define _TZ_U16ARRAY_H_








#include <stdlib.h>
#include "tz_cdefs.h"
#include "tz_utilities.h"

__BEGIN_DECLS
  
/**@addtogroup array_opr_ Array operations
 * @{
 */

/**@addtogroup array_opr_tz_uint16 tz_uint16 array (tz_u16array.h)
 * @{
 */

/*
 * u16array_max() and u16array_min() find the maximum and minimum in an array 
 * respectively. The index of the first occurrence maximum or minimum is 
 * stored in idx if it is not NULL.
 * u16array_max_m() and u16array_min_m() support masked operation.
 * u16array_max_l() and u16array_min_l() find the maximum and mininum too, but <idx>
 * is the last occurrence. Their masked verions are u16array_max_ml and u16array_min_ml.
 * The masked versions returns 0 and sets <idx> to <length> if no element is 
 * available.
 *
 
 */
tz_uint16 u16array_max(const tz_uint16* d1, size_t length, size_t* idx);
tz_uint16 u16array_min(const tz_uint16* d1, size_t length, size_t* idx);
tz_uint16 u16array_max_m(const tz_uint16 *d1, size_t length, const int *mask, size_t *idx);
tz_uint16 u16array_min_m(const tz_uint16 *d1, size_t length, const int *mask, size_t *idx);
tz_uint16 u16array_max_l(const tz_uint16* d1, size_t length, size_t* idx);
tz_uint16 u16array_min_l(const tz_uint16* d1, size_t length, size_t* idx);
tz_uint16 u16array_max_ml(const tz_uint16 *d1, size_t length, const int *mask, size_t *idx);
tz_uint16 u16array_min_ml(const tz_uint16 *d1, size_t length, const int *mask, size_t *idx);


/*
 * u16array_abs() turns <d1> to its absolute values and u16array_neg() reverse the signs
 * of <d1>.
 */


/*
 * u16array_add() stores the sum of <d1> and <d2> into <d1>. u16array_addc stores the
 * sum of <d1> and a scalar <d2> into <d1>. Other arithmetic operations have
 * the same interfaces.
 */
tz_uint16* u16array_add(tz_uint16* d1,const tz_uint16* d2, size_t length);
tz_uint16* u16array_addc(tz_uint16*d1,tz_uint16 d2, size_t length);
tz_uint16* u16array_cadd(tz_uint16 d2, tz_uint16*d1, size_t length);
tz_uint16* u16array_sub(tz_uint16* d1,tz_uint16* d2, size_t length);
tz_uint16* u16array_csub(tz_uint16 d2, tz_uint16*d1, size_t length);
tz_uint16* u16array_subc(tz_uint16*d1,tz_uint16 d2, size_t length);
tz_uint16* u16array_mul(tz_uint16* d1,tz_uint16* d2, size_t length);
tz_uint16* u16array_mulc(tz_uint16*d1,tz_uint16 d2, size_t length);
tz_uint16* u16array_div(tz_uint16* d1,tz_uint16* d2, size_t length);
tz_uint16* u16array_div_i(tz_uint16* d1,int* d2, size_t length);
tz_uint16* u16array_divc(tz_uint16*d1,tz_uint16 d2, size_t length);

/*
 * u16array_dot() returns the dot product of <d1> and <d2>.
 */
double u16array_dot(const tz_uint16 *d1, const tz_uint16 *d2, size_t length);

/*
 * u16array_sqr() calculates the square of <d1>. u16array_sqrt() calculates the squre 
 * roots of <d1>.
 */
tz_uint16* u16array_sqr(tz_uint16* d1, size_t length);
tz_uint16* u16array_sqrt(tz_uint16* d1, size_t length);

/*
 * u16array_scale() scales <d1> so that its minimal value is <min> and maxinum value
 * is <max>.
 */
tz_uint16* u16array_scale(tz_uint16 *d1, size_t length, tz_uint16 min, tz_uint16 max);

tz_uint16* u16array_max2(tz_uint16* d1,const tz_uint16* d2, size_t length);
tz_uint16* u16array_min2(tz_uint16* d1,const tz_uint16* d2, size_t length);

void u16array_threshold(tz_uint16 *d, size_t length, const tz_uint16 *min, const tz_uint16 *max);

/*
 * These are routines for special usage.
 */
tz_uint16* u16array_fun1(tz_uint16* d1,tz_uint16 *d2,tz_uint16 thr,size_t length);
tz_uint16* u16array_fun1_max(tz_uint16* d1,tz_uint16 *d2,tz_uint16 thr,size_t length,tz_uint16 *maxv);
tz_uint16* u16array_fun1_i2(tz_uint16* d1,int *d2,tz_uint16 thr,size_t length);
tz_uint16* u16array_fun1_i2_max(tz_uint16* d1,int *d2,tz_uint16 thr,size_t length,tz_uint16 *maxv);
tz_uint16* u16array_fun2(tz_uint16 *d1,tz_uint16 *d2,tz_uint16 *d3,size_t length);
tz_uint16* u16array_fun3(tz_uint16 *d1,tz_uint16 *d2,tz_uint16 *d3,tz_uint16 *d4,size_t length);

tz_uint16* u16array_cumsum(tz_uint16* d1,size_t length);
tz_uint16* u16array_cumsum_m(tz_uint16* d1,size_t length,const int *mask);
tz_uint16* u16array_cumsum2(tz_uint16* d1,int width,int height);

tz_uint16 u16array_sum(const tz_uint16* d1,size_t length);
tz_uint16 u16array_abssum(tz_uint16* d1,size_t length);
tz_uint16 u16array_sum_h(tz_uint16* d1,size_t length);
tz_uint16 u16array_centroid(const tz_uint16* d1,size_t length);
double u16array_centroid_d(const tz_uint16* d1,size_t length);
void u16array_linsum1(tz_uint16* d1,tz_uint16* d2,int width,int height,int bwidth,int bheight);
void u16array_linsum2(tz_uint16* d2,int width2,int height,int bheight);
void u16array_linsum1_part(const tz_uint16* d1,tz_uint16* d2,int width,int bwidth,
    int start, int end);
void u16array_linsum2_part(const tz_uint16* d1,tz_uint16* d2,int width2,int height,int bheight, int start, int end);


/*
 * u16arraycpy() copies an array from <src> to <dst> + <offset> with <length> 
 * elements. u16arraycpy2 copies an array from <src> to <dst> as an 2d array. The
 * size of <src> <width2> and <height2> must not be greater than the size of
 * <dst> <width1> and <height1>. It starts from (<row_offset>, <col_offset>)
 * iin dst.
 */
void u16arraycpy(tz_uint16* dst, const tz_uint16* src,size_t offset,size_t length);
void u16arraycpy2(tz_uint16* dst,tz_uint16* src,int width1,int height1,int width2,int height2,
	      int row_offset,int col_offset);
void u16arraycpy3(tz_uint16* d1,tz_uint16* d2,int width1,int height1,int depth1,int width2,
	      int height2,int depth2,int row_offset,int col_offset,
	      int dep_offset);
 
//void u16array_linsum2(tz_uint16* d2,int width,int height,int bwidth,int bheight);
tz_uint16* u16array_blocksum(tz_uint16 *d1,int width,int height,int bwidth,int bheight);
tz_uint16* u16array_blockmean(tz_uint16 *d1,int width,int height,int bwidth,int bheight,int itype);
tz_uint16* u16array_shiftdim2(tz_uint16* d1,int width,int height);
double u16array_sum_d(const tz_uint16 *d1,size_t length);
tz_uint16 u16array_mean(const tz_uint16* d1,size_t length);
double u16array_mean_d(const tz_uint16 *d1,size_t length);
double u16array_mean_d_m(const tz_uint16 *d1,size_t length, const int *mask);
double u16array_var(const tz_uint16 *d1, size_t length);
double u16array_cov(const tz_uint16 *d1, tz_uint16 *d2, size_t length);
tz_uint16* u16array_malloc(size_t length);
tz_uint16* u16array_calloc(size_t length);
tz_uint16* u16array_malloc2(int width,int height);
tz_uint16* u16array_malloc3(int width,int height,int depth);
tz_uint16* u16array_copy(tz_uint16* d1,size_t length);
void u16array_clean_edge3(tz_uint16* d1,int width,int height,int depth,int margin);

void u16array_printf(const tz_uint16 *d1, size_t length, const char *format);
void u16array_print2(const tz_uint16* d1,int width,int height);
void u16array_print(const tz_uint16* d1, size_t length);
void u16array_printf2(FILE *fp, const tz_uint16* d1,int width,int height);
void u16array_print3(const tz_uint16 *d1,int width,int height,int depth);

/**
 * Those binary reading and writing function operate on array size with integer
 * type.
 */
int u16array_write(const char* filename,const tz_uint16 *d1, int length);
tz_uint16* u16array_read(const char *filename, int *length);
int u16array_read2(const char* filename, tz_uint16 *d1, int *length);



/**
 * u16array_qsort(), u16array_myqsort() and u16array_binsearch() only supports array size 
 * with integer type. <idx> does not have to be initialized when it is not NULL.
 */
void u16array_qsort(tz_uint16 *d1,int *idx,int length);
void u16array_myqsort(tz_uint16 *d1, int length);
int u16array_binsearch(tz_uint16 *d1, int length, tz_uint16 value);

double u16array_corrcoef(const tz_uint16 *d1, const tz_uint16 *d2, size_t length);
double u16array_corrcoef_m(const tz_uint16 *d1, const tz_uint16 *d2, size_t length, 
		       const int *mask);
double u16array_uint8_corrcoef(const tz_uint16 *d1, const tz_uint8 *d2, size_t length);



tz_uint16* u16array_medfilter(const tz_uint16 *in, size_t length, int wndsize, tz_uint16 *out);



void u16array_reverse(tz_uint16 *d, size_t length);

/**@}*/

/**@}*/

__END_DECLS

#endif
