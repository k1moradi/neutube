/**@file tz_imatrix.h
 * @brief Routines for int matrix
 * @author Ting Zhao
 */


/* tz_matrix.h.t
 * 
 * 14-Aug-2007  Initial write:  Ting Zhao
 */
#ifndef _TZ_IMatrix_H_
#define _TZ_IMatrix_H_

#include <stdarg.h>
#include "tz_utilities.h"
#include "tz_mxutils.h"
#include "tz_cdefs.h"

__BEGIN_DECLS

/**@addtogroup matrix_ Matrix operations
 * @{
 */

/**@addtogroup imatrix_ int matrix operations (tz_imatrix.h)
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
  int *array;
} IMatrix;

/*utilities of int matrix*/

IMatrix* Make_IMatrix(const dim_type dim[],ndim_type ndim);
IMatrix* Copy_IMatrix(const IMatrix* dm);
void Free_IMatrix(IMatrix *imatrix);
void Kill_IMatrix(IMatrix* dm);

IMatrix* Make_3d_IMatrix(dim_type width, dim_type height, dim_type depth);

void IMatrix_Copy(IMatrix *des, const IMatrix *src);

IMatrix* Crop_IMatrix(const IMatrix* dm,const int offset[],const dim_type size[],
		IMatrix *out);
void IMatrix_Copy_Block(IMatrix *des, const dim_type des_offset[], const IMatrix *src,
		      const dim_type src_offset[]);

IMatrix* IMatrix_Blocksum(const IMatrix *dm, const dim_type bdim[], IMatrix *dm2);
IMatrix* IMatrix_Blocksum_Part(const IMatrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
IMatrix* IMatrix_Blocksum_Part2(const IMatrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
IMatrix* IMatrix_Blockmean(IMatrix* dm,const dim_type bdim[],int itype);
IMatrix* IMatrix_Blockmean_Part(IMatrix* dm,const dim_type dim[],const dim_type bdim[],const dim_type start[],const dim_type end[],int itype);
int IMatrix_Max(const IMatrix* dm,dim_type* sub);
int IMatrix_Max_P(const IMatrix *dm, const dim_type *start, const dim_type *end,
		dim_type *sub);
void IMatrix_Clean_Edge(IMatrix* dm);
int IMatrix_Scale(IMatrix* dm);

#define IMatrix_Print Print_IMatrix
void IMatrix_Print(const IMatrix* dm);
void IMatrix_Print_Part(const IMatrix* dm,const dim_type start[],const dim_type end[]);
void IMatrix_Write(const char* filename,const IMatrix* dm);
IMatrix* IMatrix_Read(const char* filename);

IMatrix* iarray2imatrix(int* array,ndim_type ndim,...);

IMatrix* darray2imatrix(double *array,ndim_type ndim,...);


IMatrix IMatrix_View_Array(int* array, ndim_type ndim, ...);

void IMatrix_Set_Zero(IMatrix *dm);

IMatrix* Constant_IMatrix(const dim_type dim[],ndim_type ndim,int value);
IMatrix* IMatrix_Zeros(const dim_type dim[],ndim_type ndim);
IMatrix* IMatrix_Ones(const dim_type dim[],ndim_type ndim);

IMatrix* IMatrix_Add(IMatrix* dm1,const IMatrix*  dm2);
IMatrix* IMatrix_Addc(IMatrix* dm1,int d);
IMatrix* IMatrix_Sub(IMatrix* dm1,const IMatrix*  dm2);
IMatrix* IMatrix_Subc(IMatrix* dm1,int d);
IMatrix* IMatrix_Mul(IMatrix* dm1,const IMatrix*  dm2);
IMatrix* IMatrix_Div(IMatrix* dm1,const IMatrix*  dm2);
IMatrix* IMatrix_Sqr(IMatrix* dm1);
IMatrix* IMatrix_Sqrt(IMatrix* dm2);
IMatrix* IMatrix_Negative(IMatrix *dm);
IMatrix* IMatrix_Max2(IMatrix *dm1, const IMatrix *dm2);
IMatrix* IMatrix_Min2(IMatrix *dm1, const IMatrix *dm2);

IMatrix* IMatrix_Partial_Diff(const IMatrix *dm, ndim_type dim, IMatrix *result);

void IMatrix_Threshold(IMatrix *dm, int threshold);

void IMatrix_Abs_Threshold(IMatrix *dm, int threshold);

IMatrix* IMatrix_Eigen3_Coeff2(IMatrix *a, IMatrix *b, IMatrix *c, IMatrix *result);
IMatrix* IMatrix_Eigen3_Coeff1(IMatrix *a, IMatrix *b, IMatrix *c, IMatrix *d, IMatrix *e, IMatrix *f,
			 IMatrix *result);
IMatrix* IMatrix_Eigen3_Coeff0(IMatrix *a, IMatrix *b, IMatrix *c, IMatrix *d, IMatrix *e, IMatrix *f,
			 IMatrix *result);
IMatrix* IMatrix_Eigen3_Coeffd(IMatrix *a, IMatrix *b, IMatrix *c, IMatrix *d, IMatrix *e, IMatrix *f,
			 IMatrix *result);
IMatrix* IMatrix_Eigen3_Solution_Score(IMatrix *a, IMatrix *b, IMatrix *c, IMatrix *d, 
				 IMatrix *e, IMatrix *f, IMatrix *result);
IMatrix* IMatrix_Eigen3_Curvature(IMatrix *xx, IMatrix *yy, IMatrix *zz, 
			    IMatrix *x, IMatrix *y, IMatrix *z, IMatrix *result);
/**@}*/
/**@}*/

__END_DECLS

#endif
