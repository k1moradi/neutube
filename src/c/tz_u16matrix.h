/**@file tz_u16matrix.h
 * @brief Routines for uint16_t matrix
 * @author Ting Zhao
 */


/* tz_matrix.h.t
 * 
 * 14-Aug-2007  Initial write:  Ting Zhao
 */
#ifndef _TZ_U16Matrix_H_
#define _TZ_U16Matrix_H_

#include <stdarg.h>
#include "tz_utilities.h"
#include "tz_mxutils.h"
#include "tz_cdefs.h"

__BEGIN_DECLS

/**@addtogroup matrix_ Matrix operations
 * @{
 */

/**@addtogroup u16matrix_ uint16_t matrix operations (tz_u16matrix.h)
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
  uint16_t *array;
} U16Matrix;

/*utilities of uint16_t matrix*/

U16Matrix* Make_U16Matrix(const dim_type dim[],ndim_type ndim);
U16Matrix* Copy_U16Matrix(const U16Matrix* dm);
void Free_U16Matrix(U16Matrix *u16matrix);
void Kill_U16Matrix(U16Matrix* dm);

U16Matrix* Make_3d_U16Matrix(dim_type width, dim_type height, dim_type depth);

void U16Matrix_Copy(U16Matrix *des, const U16Matrix *src);

U16Matrix* Crop_U16Matrix(const U16Matrix* dm,const int offset[],const dim_type size[],
		U16Matrix *out);
void U16Matrix_Copy_Block(U16Matrix *des, const dim_type des_offset[], const U16Matrix *src,
		      const dim_type src_offset[]);

U16Matrix* U16Matrix_Blocksum(const U16Matrix *dm, const dim_type bdim[], U16Matrix *dm2);
U16Matrix* U16Matrix_Blocksum_Part(const U16Matrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
U16Matrix* U16Matrix_Blocksum_Part2(const U16Matrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
U16Matrix* U16Matrix_Blockmean(U16Matrix* dm,const dim_type bdim[],int itype);
U16Matrix* U16Matrix_Blockmean_Part(U16Matrix* dm,const dim_type dim[],const dim_type bdim[],const dim_type start[],const dim_type end[],int itype);
uint16_t U16Matrix_Max(const U16Matrix* dm,dim_type* sub);
uint16_t U16Matrix_Max_P(const U16Matrix *dm, const dim_type *start, const dim_type *end,
		dim_type *sub);
void U16Matrix_Clean_Edge(U16Matrix* dm);
uint16_t U16Matrix_Scale(U16Matrix* dm);

#define U16Matrix_Print Print_U16Matrix
void U16Matrix_Print(const U16Matrix* dm);
void U16Matrix_Print_Part(const U16Matrix* dm,const dim_type start[],const dim_type end[]);
void U16Matrix_Write(const char* filename,const U16Matrix* dm);
U16Matrix* U16Matrix_Read(const char* filename);

U16Matrix* u16array2u16matrix(uint16_t* array,ndim_type ndim,...);


U16Matrix U16Matrix_View_Array(uint16_t* array, ndim_type ndim, ...);

void U16Matrix_Set_Zero(U16Matrix *dm);

U16Matrix* Constant_U16Matrix(const dim_type dim[],ndim_type ndim,uint16_t value);
U16Matrix* U16Matrix_Zeros(const dim_type dim[],ndim_type ndim);
U16Matrix* U16Matrix_Ones(const dim_type dim[],ndim_type ndim);

U16Matrix* U16Matrix_Add(U16Matrix* dm1,const U16Matrix*  dm2);
U16Matrix* U16Matrix_Addc(U16Matrix* dm1,uint16_t d);
U16Matrix* U16Matrix_Sub(U16Matrix* dm1,const U16Matrix*  dm2);
U16Matrix* U16Matrix_Subc(U16Matrix* dm1,uint16_t d);
U16Matrix* U16Matrix_Mul(U16Matrix* dm1,const U16Matrix*  dm2);
U16Matrix* U16Matrix_Div(U16Matrix* dm1,const U16Matrix*  dm2);
U16Matrix* U16Matrix_Sqr(U16Matrix* dm1);
U16Matrix* U16Matrix_Sqrt(U16Matrix* dm2);
U16Matrix* U16Matrix_Negative(U16Matrix *dm);
U16Matrix* U16Matrix_Max2(U16Matrix *dm1, const U16Matrix *dm2);
U16Matrix* U16Matrix_Min2(U16Matrix *dm1, const U16Matrix *dm2);

U16Matrix* U16Matrix_Partial_Diff(const U16Matrix *dm, ndim_type dim, U16Matrix *result);

void U16Matrix_Threshold(U16Matrix *dm, uint16_t threshold);

void U16Matrix_Abs_Threshold(U16Matrix *dm, uint16_t threshold);

U16Matrix* U16Matrix_Eigen3_Coeff2(U16Matrix *a, U16Matrix *b, U16Matrix *c, U16Matrix *result);
U16Matrix* U16Matrix_Eigen3_Coeff1(U16Matrix *a, U16Matrix *b, U16Matrix *c, U16Matrix *d, U16Matrix *e, U16Matrix *f,
			 U16Matrix *result);
U16Matrix* U16Matrix_Eigen3_Coeff0(U16Matrix *a, U16Matrix *b, U16Matrix *c, U16Matrix *d, U16Matrix *e, U16Matrix *f,
			 U16Matrix *result);
U16Matrix* U16Matrix_Eigen3_Coeffd(U16Matrix *a, U16Matrix *b, U16Matrix *c, U16Matrix *d, U16Matrix *e, U16Matrix *f,
			 U16Matrix *result);
U16Matrix* U16Matrix_Eigen3_Solution_Score(U16Matrix *a, U16Matrix *b, U16Matrix *c, U16Matrix *d, 
				 U16Matrix *e, U16Matrix *f, U16Matrix *result);
U16Matrix* U16Matrix_Eigen3_Curvature(U16Matrix *xx, U16Matrix *yy, U16Matrix *zz, 
			    U16Matrix *x, U16Matrix *y, U16Matrix *z, U16Matrix *result);
/**@}*/
/**@}*/

__END_DECLS

#endif
