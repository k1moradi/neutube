/**@file tz_u8matrix.h
 * @brief Routines for uint8_t matrix
 * @author Ting Zhao
 */


/* tz_matrix.h.t
 * 
 * 14-Aug-2007  Initial write:  Ting Zhao
 */
#ifndef _TZ_U8Matrix_H_
#define _TZ_U8Matrix_H_

#include <stdarg.h>
#include "tz_utilities.h"
#include "tz_mxutils.h"
#include "tz_cdefs.h"

__BEGIN_DECLS

/**@addtogroup matrix_ Matrix operations
 * @{
 */

/**@addtogroup u8matrix_ uint8_t matrix operations (tz_u8matrix.h)
 * @{
 */

/**
 * This structure defines a multidimensional matrix. The number of dimensions is ndim.
 * The dimensions are stored in the array dim and the matrix elements are stored
 * in the array in the column-major order, i.e. the lower dimension cycles 
 * faster.
 */ 
typedef struct {
  ndim_t ndim;
  dim_t dim[TZ_MATRIX_MAX_DIM];
  uint8_t *array;
} U8Matrix;

/*utilities of uint8_t matrix*/

U8Matrix* Make_U8Matrix(const dim_type dim[],ndim_type ndim);
U8Matrix* Copy_U8Matrix(const U8Matrix* dm);
void Free_U8Matrix(U8Matrix *u8matrix);
void Kill_U8Matrix(U8Matrix* dm);

U8Matrix* Make_3d_U8Matrix(dim_type width, dim_type height, dim_type depth);

void U8Matrix_Copy(U8Matrix *des, const U8Matrix *src);

U8Matrix* Crop_U8Matrix(const U8Matrix* dm,const int offset[],const dim_type size[],
		U8Matrix *out);
void U8Matrix_Copy_Block(U8Matrix *des, const dim_type des_offset[], const U8Matrix *src,
		      const dim_type src_offset[]);

U8Matrix* U8Matrix_Blocksum(const U8Matrix *dm, const dim_type bdim[], U8Matrix *dm2);
U8Matrix* U8Matrix_Blocksum_Part(const U8Matrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
U8Matrix* U8Matrix_Blocksum_Part2(const U8Matrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
U8Matrix* U8Matrix_Blockmean(U8Matrix* dm,const dim_type bdim[],int itype);
U8Matrix* U8Matrix_Blockmean_Part(U8Matrix* dm,const dim_type dim[],const dim_type bdim[],const dim_type start[],const dim_type end[],int itype);
uint8_t U8Matrix_Max(const U8Matrix* dm,dim_type* sub);
uint8_t U8Matrix_Max_P(const U8Matrix *dm, const dim_type *start, const dim_type *end,
		dim_type *sub);
void U8Matrix_Clean_Edge(U8Matrix* dm);
uint8_t U8Matrix_Scale(U8Matrix* dm);

#define U8Matrix_Print Print_U8Matrix
void U8Matrix_Print(const U8Matrix* dm);
void U8Matrix_Print_Part(const U8Matrix* dm,const dim_type start[],const dim_type end[]);
void U8Matrix_Write(const char* filename,const U8Matrix* dm);
U8Matrix* U8Matrix_Read(const char* filename);

U8Matrix* u8array2u8matrix(uint8_t* array,ndim_type ndim,...);


U8Matrix U8Matrix_View_Array(uint8_t* array, ndim_type ndim, ...);

void U8Matrix_Set_Zero(U8Matrix *dm);

U8Matrix* Constant_U8Matrix(const dim_type dim[],ndim_type ndim,uint8_t value);
U8Matrix* U8Matrix_Zeros(const dim_type dim[],ndim_type ndim);
U8Matrix* U8Matrix_Ones(const dim_type dim[],ndim_type ndim);

U8Matrix* U8Matrix_Add(U8Matrix* dm1,const U8Matrix*  dm2);
U8Matrix* U8Matrix_Addc(U8Matrix* dm1,uint8_t d);
U8Matrix* U8Matrix_Sub(U8Matrix* dm1,const U8Matrix*  dm2);
U8Matrix* U8Matrix_Subc(U8Matrix* dm1,uint8_t d);
U8Matrix* U8Matrix_Mul(U8Matrix* dm1,const U8Matrix*  dm2);
U8Matrix* U8Matrix_Div(U8Matrix* dm1,const U8Matrix*  dm2);
U8Matrix* U8Matrix_Sqr(U8Matrix* dm1);
U8Matrix* U8Matrix_Sqrt(U8Matrix* dm2);
U8Matrix* U8Matrix_Negative(U8Matrix *dm);
U8Matrix* U8Matrix_Max2(U8Matrix *dm1, const U8Matrix *dm2);
U8Matrix* U8Matrix_Min2(U8Matrix *dm1, const U8Matrix *dm2);

U8Matrix* U8Matrix_Partial_Diff(const U8Matrix *dm, ndim_type dim, U8Matrix *result);

void U8Matrix_Threshold(U8Matrix *dm, uint8_t threshold);

void U8Matrix_Abs_Threshold(U8Matrix *dm, uint8_t threshold);

U8Matrix* U8Matrix_Eigen3_Coeff2(U8Matrix *a, U8Matrix *b, U8Matrix *c, U8Matrix *result);
U8Matrix* U8Matrix_Eigen3_Coeff1(U8Matrix *a, U8Matrix *b, U8Matrix *c, U8Matrix *d, U8Matrix *e, U8Matrix *f,
			 U8Matrix *result);
U8Matrix* U8Matrix_Eigen3_Coeff0(U8Matrix *a, U8Matrix *b, U8Matrix *c, U8Matrix *d, U8Matrix *e, U8Matrix *f,
			 U8Matrix *result);
U8Matrix* U8Matrix_Eigen3_Coeffd(U8Matrix *a, U8Matrix *b, U8Matrix *c, U8Matrix *d, U8Matrix *e, U8Matrix *f,
			 U8Matrix *result);
U8Matrix* U8Matrix_Eigen3_Solution_Score(U8Matrix *a, U8Matrix *b, U8Matrix *c, U8Matrix *d, 
				 U8Matrix *e, U8Matrix *f, U8Matrix *result);
U8Matrix* U8Matrix_Eigen3_Curvature(U8Matrix *xx, U8Matrix *yy, U8Matrix *zz, 
			    U8Matrix *x, U8Matrix *y, U8Matrix *z, U8Matrix *result);
/**@}*/
/**@}*/

__END_DECLS

#endif
