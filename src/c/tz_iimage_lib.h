/**@file tz_iimage_lib.h
 * @brief routines for int image
 * @author Ting Zhao
 */

#ifndef _TZ_IIMAGE_LIB_H_
#define _TZ_IIMAGE_LIB_H_

/* <T1 DIMAGE> <T2 DMatrix> <T3 DOUBLE> <T4 D> 
   <t5 fftw> <T6 Double> <t7 darray> */

#include "tz_cdefs.h"

#include "image_lib.h"
#include "tz_imatrix.h"
#include "tz_geo3d_scalar_field.h"
#include "tz_object_3d.h"

__BEGIN_DECLS






IMatrix* Get_Int_Matrix3(const Stack *stack);


__END_DECLS
#endif
