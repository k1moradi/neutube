/**@file tz_dmatrix.h
 * @brief Routines for double matrix
 * @author Ting Zhao
 */


/* tz_matrix.h.t
 * 
 * 14-Aug-2007  Initial write:  Ting Zhao
 */
#ifndef _TZ_DMatrix_H_
#define _TZ_DMatrix_H_

#include <stdarg.h>
#include "tz_utilities.h"
#include "tz_mxutils.h"
#include "tz_cdefs.h"

__BEGIN_DECLS

/**@addtogroup matrix_ Matrix operations
 * @{
 */

/**@addtogroup dmatrix_ double matrix operations (tz_dmatrix.h)
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
  double *array;
} DMatrix;

/*utilities of double matrix*/

DMatrix* Make_DMatrix(const dim_type dim[],ndim_type ndim);
DMatrix* Copy_DMatrix(const DMatrix* dm);
void Free_DMatrix(DMatrix *dmatrix);
void Kill_DMatrix(DMatrix* dm);

DMatrix* Make_3d_DMatrix(dim_type width, dim_type height, dim_type depth);

void DMatrix_Copy(DMatrix *des, const DMatrix *src);

DMatrix* Crop_DMatrix(const DMatrix* dm,const int offset[],const dim_type size[],
		DMatrix *out);
void DMatrix_Copy_Block(DMatrix *des, const dim_type des_offset[], const DMatrix *src,
		      const dim_type src_offset[]);

DMatrix* DMatrix_Blocksum(const DMatrix *dm, const dim_type bdim[], DMatrix *dm2);
DMatrix* DMatrix_Blocksum_Part(const DMatrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
DMatrix* DMatrix_Blocksum_Part2(const DMatrix* dm,const dim_type bdim[],const dim_type start[],const dim_type end[]);
DMatrix* DMatrix_Blockmean(DMatrix* dm,const dim_type bdim[],int itype);
DMatrix* DMatrix_Blockmean_Part(DMatrix* dm,const dim_type dim[],const dim_type bdim[],const dim_type start[],const dim_type end[],int itype);
double DMatrix_Max(const DMatrix* dm,dim_type* sub);
double DMatrix_Max_P(const DMatrix *dm, const dim_type *start, const dim_type *end,
		dim_type *sub);
void DMatrix_Clean_Edge(DMatrix* dm);
double DMatrix_Scale(DMatrix* dm);

#define DMatrix_Print Print_DMatrix
void DMatrix_Print(const DMatrix* dm);
void DMatrix_Print_Part(const DMatrix* dm,const dim_type start[],const dim_type end[]);
void DMatrix_Write(const char* filename,const DMatrix* dm);
DMatrix* DMatrix_Read(const char* filename);

DMatrix* darray2dmatrix(double* array,ndim_type ndim,...);


DMatrix DMatrix_View_Array(double* array, ndim_type ndim, ...);

void DMatrix_Set_Zero(DMatrix *dm);

DMatrix* Constant_DMatrix(const dim_type dim[],ndim_type ndim,double value);
DMatrix* DMatrix_Zeros(const dim_type dim[],ndim_type ndim);
DMatrix* DMatrix_Ones(const dim_type dim[],ndim_type ndim);

DMatrix* DMatrix_Add(DMatrix* dm1,const DMatrix*  dm2);
DMatrix* DMatrix_Addc(DMatrix* dm1,double d);
DMatrix* DMatrix_Sub(DMatrix* dm1,const DMatrix*  dm2);
DMatrix* DMatrix_Subc(DMatrix* dm1,double d);
DMatrix* DMatrix_Mul(DMatrix* dm1,const DMatrix*  dm2);
DMatrix* DMatrix_Div(DMatrix* dm1,const DMatrix*  dm2);
DMatrix* DMatrix_Sqr(DMatrix* dm1);
DMatrix* DMatrix_Sqrt(DMatrix* dm2);
DMatrix* DMatrix_Negative(DMatrix *dm);
DMatrix* DMatrix_Max2(DMatrix *dm1, const DMatrix *dm2);
DMatrix* DMatrix_Min2(DMatrix *dm1, const DMatrix *dm2);

DMatrix* DMatrix_Partial_Diff(const DMatrix *dm, ndim_type dim, DMatrix *result);

void DMatrix_Threshold(DMatrix *dm, double threshold);

void DMatrix_Abs_Threshold(DMatrix *dm, double threshold);

DMatrix* DMatrix_Eigen3_Coeff2(DMatrix *a, DMatrix *b, DMatrix *c, DMatrix *result);
DMatrix* DMatrix_Eigen3_Coeff1(DMatrix *a, DMatrix *b, DMatrix *c, DMatrix *d, DMatrix *e, DMatrix *f,
			 DMatrix *result);
DMatrix* DMatrix_Eigen3_Coeff0(DMatrix *a, DMatrix *b, DMatrix *c, DMatrix *d, DMatrix *e, DMatrix *f,
			 DMatrix *result);
DMatrix* DMatrix_Eigen3_Coeffd(DMatrix *a, DMatrix *b, DMatrix *c, DMatrix *d, DMatrix *e, DMatrix *f,
			 DMatrix *result);
DMatrix* DMatrix_Eigen3_Solution_Score(DMatrix *a, DMatrix *b, DMatrix *c, DMatrix *d, 
				 DMatrix *e, DMatrix *f, DMatrix *result);
DMatrix* DMatrix_Eigen3_Curvature(DMatrix *xx, DMatrix *yy, DMatrix *zz, 
			    DMatrix *x, DMatrix *y, DMatrix *z, DMatrix *result);
/**@}*/
/**@}*/

__END_DECLS

#endif
