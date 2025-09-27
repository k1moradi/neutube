/**@file tz_fimage_lib.h
 * @brief routines for float image
 * @author Ting Zhao
 */

#ifndef _TZ_FIMAGE_LIB_H_
#define _TZ_FIMAGE_LIB_H_

/* <T1 DIMAGE> <T2 DMatrix> <T3 DOUBLE> <T4 D> 
   <t5 fftw> <T6 Double> <t7 darray> */

#include "tz_cdefs.h"

#include "tz_fftw_header.h"

#include "image_lib.h"
#include "tz_fmatrix.h"
#include "tz_geo3d_scalar_field.h"
#include "tz_object_3d.h"

__BEGIN_DECLS


 /**@brief Cumulative sum of an image.
 * Cumsum_Image_F() calculates cumulative sum of an image. It returns a
 * float array whose ith element is the sum of the values of 0 - i pixels in
 * the image.
 */
float* Cumsum_Image_F(const Image *image);




#if defined(HAVE_LIBFFTWF)


/**@brief Fast fourier transform.
 * Image_FFT_F() calculates the fourier transform of an image. It returns an
 * array of complex numbers. Image_IFFT_F calculates the inverse fourier
 * transform of a complex array in the 2D space. The function assumes the array
 * has size <width> x <height>. 
 */
fftwf_complex* Image_FFT_F(Image *image);
float *Image_IFFT_F(fftwf_complex* fimage,int width,int height);

#endif

/**@brief Convolve an image.
 * 
 */
float* Convolve_Image_F(Image *image1, Image *image2,int reflect);
float* Correlate_Image_F(Image* image1,Image* image2);
float* Normcorr_Image_F(Image *image1, Image *image2);


FMatrix* Mexihat_2D_F(double sigma, FMatrix *filter);
FMatrix* Filter_Image_F(Image *image, const FMatrix *filter, FMatrix *out);
FMatrix* Filter_Image_Fast_F(Image *image, FMatrix *filter, FMatrix *out, int pad);



float* Get_Float_Array_Pad(Stack *stack);




#if defined(HAVE_LIBFFTWF)


fftwf_complex* Stack_FFT_F(Stack *stack);
float* Stack_IFFT_F(fftwf_complex* fstack,int width,int height,int depth);

#endif

float* Convolve_Stack_F(Stack* stack1,Stack* stack2,int reflect);
float* Correlate_Padstack_F(Stack* stack1,Stack* stack2);
float* Correlate_Stack_Part_F(Stack* stack1,Stack* stack2,int start[],int end[]);
FMatrix* Normcorr_Stack_F(Stack *stack1, Stack *stack2,int std,float *max_corr);
FMatrix* Normcorr_Stack_Part_F(Stack *stack1,Stack *stack2,int std,int start[],int end[]);
float Align_Stack_F(Stack* stack1,Stack* stack2,int offset[],float *unnorm_maxcorr);
float Align_Stack_C_F(Stack* stack1, Stack* stack2, const int config[],
			int offset[], float *unnorm_maxcorr);
float Align_Stack_MR_F(Stack* stack1, Stack* stack2, int intv[], int fine,
			 const int *config, int offset[], float *unnorm_maxcorr);



FMatrix* Ring_Filter_F(double r1, double r2, FMatrix *filter);
FMatrix* Gaussian_2D_Filter_F(const double *sigma, FMatrix *filter);
FMatrix* Mexihat_3D1_F(double sigma, FMatrix *filter, ndim_t dt);
FMatrix* Mexihat_2D_F(double sigma, FMatrix *filter);
FMatrix* Mexihat_3D_F(double sigma, FMatrix *filter);
FMatrix* Gaussian_3D_Filter_F(const double *sigma, FMatrix *filter);
FMatrix* Gaussian_3D_Filter_2x_F(const double *sigma, FMatrix *filter);
FMatrix* Gaussian_Deriv_3D_Filter_F(const double *sigma, double theta, 
				    double psi, FMatrix *filter);
FMatrix* Gaussian_3D_D2_Filter_F(const double *sigma, int dim[2], FMatrix *filter);
FMatrix* Geo3d_Scalar_Field_To_Filter_F(const Geo3d_Scalar_Field *field);

FMatrix* Filter_Stack_F(const Stack *stack, const FMatrix *filter, FMatrix *out);
FMatrix* Filter_Stack_Fast_F(const Stack *stack, const FMatrix *filter, FMatrix *out, int pad);
FMatrix* Filter_Stack_Block_F(const Stack *stack, const FMatrix *filter, 
			      FMatrix *out);
FMatrix* Filter_Stack_Slice_F(const Stack *stack, const FMatrix *filter, FMatrix *out);

FMatrix* Smooth_Stack_Fast_F(const Stack *stack, int wx, int wy, int wz, 
    FMatrix *out);

void Correct_Filter_Stack_F(const FMatrix *filter, FMatrix *stack);

FMatrix* El_Stack_F(const Stack *stack, const double *scale, FMatrix *out);
FMatrix* El_Stack_L_F(const Stack *stack, const double *scale, FMatrix *out);

Stack *Stack_Line_Paint_F(const Stack *stack, double *scale, int option);
FMatrix* Stack_Pixel_Feature_F(const Stack *stack, const double *scale, 
			       const Object_3d *pts, FMatrix *result);



FMatrix* Get_Float_Matrix3(const Stack *stack);


__END_DECLS
#endif
