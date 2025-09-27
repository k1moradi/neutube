/**@file tz_fmatrix.h
 * @brief Routines for float matrix
 * @author Ting Zhao
 */


/* tz_matrix.h.t
 * 
 * 14-Aug-2007  Initial write:  Ting Zhao
 */
#ifndef _TZ_FMatrix_H_
#define _TZ_FMatrix_H_

#include <stdarg.h>
#include "tz_utilities.h"
#include "tz_mxutils.h"
#include "tz_cdefs.h"

__BEGIN_DECLS

/**@addtogroup matrix_ Matrix operations
 * @{
 */

/**@addtogroup fmatrix_ float matrix operations (tz_fmatrix.h)
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
  float *array;
} FMatrix;

/*utilities of float matrix*/

FMatrix* Make_FMatrix(const dim_type dim[],ndim_type ndim);
FMatrix* Copy_FMatrix(const FMatrix* dm);
void Free_FMatrix(FMatrix *fmatrix);
void Kill_FMatrix(FMatrix* dm);

FMatrix* Make_3d_FMatrix(dim_type width, dim_type height, dim_type depth);

void FMatrix_Copy(FMatrix *des, const FMatrix *src);

FMatrix* Crop_FMatrix(const FMatrix* dm,const int offset[],const dim_type size[],
		FMatrix *out);
void FMatrix_Copy_Block(FMatrix *des, const dim_type des_offset[], const FMatrix *src,
		      const dim_type src_offset[]);

FMatrix* FMatrix_Blocksum(const FMatrix *dm, const dim_type bdim[], FMatrix *dm2);
FMatrix* FMatrix_Blocksum_Part(const FMatrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
FMatrix* FMatrix_Blocksum_Part2(const FMatrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
FMatrix* FMatrix_Blockmean(FMatrix* dm,const dim_type bdim[],int itype);
FMatrix* FMatrix_Blockmean_Part(FMatrix* dm,const dim_type dim[],const dim_type bdim[],const dim_type start[],const dim_type end[],int itype);
float FMatrix_Max(const FMatrix* dm,dim_type* sub);
float FMatrix_Max_P(const FMatrix *dm, const dim_type *start, const dim_type *end,
		dim_type *sub);
void FMatrix_Clean_Edge(FMatrix* dm);
float FMatrix_Scale(FMatrix* dm);

#define FMatrix_Print Print_FMatrix
void FMatrix_Print(const FMatrix* dm);
void FMatrix_Print_Part(const FMatrix* dm,const dim_type start[],const dim_type end[]);
void FMatrix_Write(const char* filename,const FMatrix* dm);
FMatrix* FMatrix_Read(const char* filename);

FMatrix* farray2fmatrix(float* array,ndim_type ndim,...);

FMatrix* darray2fmatrix(double *array,ndim_type ndim,...);


FMatrix FMatrix_View_Array(float* array, ndim_type ndim, ...);

void FMatrix_Set_Zero(FMatrix *dm);

FMatrix* Constant_FMatrix(const dim_type dim[],ndim_type ndim,float value);
FMatrix* FMatrix_Zeros(const dim_type dim[],ndim_type ndim);
FMatrix* FMatrix_Ones(const dim_type dim[],ndim_type ndim);

FMatrix* FMatrix_Add(FMatrix* dm1,const FMatrix*  dm2);
FMatrix* FMatrix_Addc(FMatrix* dm1,float d);
FMatrix* FMatrix_Sub(FMatrix* dm1,const FMatrix*  dm2);
FMatrix* FMatrix_Subc(FMatrix* dm1,float d);
FMatrix* FMatrix_Mul(FMatrix* dm1,const FMatrix*  dm2);
FMatrix* FMatrix_Div(FMatrix* dm1,const FMatrix*  dm2);
FMatrix* FMatrix_Sqr(FMatrix* dm1);
FMatrix* FMatrix_Sqrt(FMatrix* dm2);
FMatrix* FMatrix_Negative(FMatrix *dm);
FMatrix* FMatrix_Max2(FMatrix *dm1, const FMatrix *dm2);
FMatrix* FMatrix_Min2(FMatrix *dm1, const FMatrix *dm2);

FMatrix* FMatrix_Partial_Diff(const FMatrix *dm, ndim_type dim, FMatrix *result);

void FMatrix_Threshold(FMatrix *dm, float threshold);

void FMatrix_Abs_Threshold(FMatrix *dm, float threshold);

FMatrix* FMatrix_Eigen3_Coeff2(FMatrix *a, FMatrix *b, FMatrix *c, FMatrix *result);
FMatrix* FMatrix_Eigen3_Coeff1(FMatrix *a, FMatrix *b, FMatrix *c, FMatrix *d, FMatrix *e, FMatrix *f,
			 FMatrix *result);
FMatrix* FMatrix_Eigen3_Coeff0(FMatrix *a, FMatrix *b, FMatrix *c, FMatrix *d, FMatrix *e, FMatrix *f,
			 FMatrix *result);
FMatrix* FMatrix_Eigen3_Coeffd(FMatrix *a, FMatrix *b, FMatrix *c, FMatrix *d, FMatrix *e, FMatrix *f,
			 FMatrix *result);
FMatrix* FMatrix_Eigen3_Solution_Score(FMatrix *a, FMatrix *b, FMatrix *c, FMatrix *d, 
				 FMatrix *e, FMatrix *f, FMatrix *result);
FMatrix* FMatrix_Eigen3_Curvature(FMatrix *xx, FMatrix *yy, FMatrix *zz, 
			    FMatrix *x, FMatrix *y, FMatrix *z, FMatrix *result);
/**@}*/
/**@}*/

__END_DECLS

#endif
