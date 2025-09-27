/**@file tz_dimage_lib.h
 * @brief routines for double image
 * @author Ting Zhao
 */

#ifndef _TZ_DIMAGE_LIB_H_
#define _TZ_DIMAGE_LIB_H_

/* <T1 DIMAGE> <T2 DMatrix> <T3 DOUBLE> <T4 D> 
   <t5 fftw> <T6 Double> <t7 darray> */

#include "tz_cdefs.h"

#include "tz_fftw_header.h"

#include "image_lib.h"
#include "tz_dmatrix.h"
#include "tz_geo3d_scalar_field.h"
#include "tz_object_3d.h"

__BEGIN_DECLS


 /**@brief Cumulative sum of an image.
 * Cumsum_Image_D() calculates cumulative sum of an image. It returns a
 * double array whose ith element is the sum of the values of 0 - i pixels in
 * the image.
 */
double* Cumsum_Image_D(const Image *image);


#if defined(HAVE_LIBFFTW)




/**@brief Fast fourier transform.
 * Image_FFT_D() calculates the fourier transform of an image. It returns an
 * array of complex numbers. Image_IFFT_D calculates the inverse fourier
 * transform of a complex array in the 2D space. The function assumes the array
 * has size <width> x <height>. 
 */
fftw_complex* Image_FFT_D(Image *image);
double *Image_IFFT_D(fftw_complex* fimage,int width,int height);

#endif

/**@brief Convolve an image.
 * 
 */
double* Convolve_Image_D(Image *image1, Image *image2,int reflect);
double* Correlate_Image_D(Image* image1,Image* image2);
double* Normcorr_Image_D(Image *image1, Image *image2);


DMatrix* Mexihat_2D_D(double sigma, DMatrix *filter);
DMatrix* Filter_Image_D(Image *image, const DMatrix *filter, DMatrix *out);
DMatrix* Filter_Image_Fast_D(Image *image, DMatrix *filter, DMatrix *out, int pad);



double* Get_Double_Array_Pad(Stack *stack);


#if defined(HAVE_LIBFFTW)




fftw_complex* Stack_FFT_D(Stack *stack);
double* Stack_IFFT_D(fftw_complex* fstack,int width,int height,int depth);

#endif

double* Convolve_Stack_D(Stack* stack1,Stack* stack2,int reflect);
double* Correlate_Padstack_D(Stack* stack1,Stack* stack2);
double* Correlate_Stack_Part_D(Stack* stack1,Stack* stack2,int start[],int end[]);
DMatrix* Normcorr_Stack_D(Stack *stack1, Stack *stack2,int std,double *max_corr);
DMatrix* Normcorr_Stack_Part_D(Stack *stack1,Stack *stack2,int std,int start[],int end[]);
double Align_Stack_D(Stack* stack1,Stack* stack2,int offset[],double *unnorm_maxcorr);
double Align_Stack_C_D(Stack* stack1, Stack* stack2, const int config[],
			int offset[], double *unnorm_maxcorr);
double Align_Stack_MR_D(Stack* stack1, Stack* stack2, int intv[], int fine,
			 const int *config, int offset[], double *unnorm_maxcorr);



DMatrix* Ring_Filter_D(double r1, double r2, DMatrix *filter);
DMatrix* Gaussian_2D_Filter_D(const double *sigma, DMatrix *filter);
DMatrix* Mexihat_3D1_D(double sigma, DMatrix *filter, ndim_t dt);
DMatrix* Mexihat_2D_D(double sigma, DMatrix *filter);
DMatrix* Mexihat_3D_D(double sigma, DMatrix *filter);
DMatrix* Gaussian_3D_Filter_D(const double *sigma, DMatrix *filter);
DMatrix* Gaussian_3D_Filter_2x_D(const double *sigma, DMatrix *filter);
DMatrix* Gaussian_Deriv_3D_Filter_D(const double *sigma, double theta, 
				    double psi, DMatrix *filter);
DMatrix* Gaussian_3D_D2_Filter_D(const double *sigma, int dim[2], DMatrix *filter);
DMatrix* Geo3d_Scalar_Field_To_Filter_D(const Geo3d_Scalar_Field *field);

DMatrix* Filter_Stack_D(const Stack *stack, const DMatrix *filter, DMatrix *out);
DMatrix* Filter_Stack_Fast_D(const Stack *stack, const DMatrix *filter, DMatrix *out, int pad);
DMatrix* Filter_Stack_Block_D(const Stack *stack, const DMatrix *filter, 
			      DMatrix *out);
DMatrix* Filter_Stack_Slice_D(const Stack *stack, const DMatrix *filter, DMatrix *out);

DMatrix* Smooth_Stack_Fast_D(const Stack *stack, int wx, int wy, int wz, 
    DMatrix *out);

void Correct_Filter_Stack_D(const DMatrix *filter, DMatrix *stack);

DMatrix* El_Stack_D(const Stack *stack, const double *scale, DMatrix *out);
DMatrix* El_Stack_L_D(const Stack *stack, const double *scale, DMatrix *out);

Stack *Stack_Line_Paint_D(const Stack *stack, double *scale, int option);
DMatrix* Stack_Pixel_Feature_D(const Stack *stack, const double *scale, 
			       const Object_3d *pts, DMatrix *result);



DMatrix* Get_Double_Matrix3(const Stack *stack);


__END_DECLS
#endif
