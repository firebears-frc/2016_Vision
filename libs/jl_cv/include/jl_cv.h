#include "cv.h"
#include "highgui.h"
#include "jl.h"

typedef enum {
	JL_CV_ORIG, // the original image will be output.
	JL_CV_CHNG, // the modified image will be output.
	JL_CV_GRAY, // the modified image will be output. ( Grayscale )
}jl_cv_output_t;

typedef struct {
	CvPoint p1;
	CvPoint p2;
}jl_cv_line_t;

typedef struct{
	jl_t* jlc;
	CvCapture* camera;
	IplImage* image;
	IplImage* image_hsv;
	IplImage* gray_image;
	IplImage* gray_blur;
	IplImage* disp_image; // The image that will be displayed on screen.
	IplImage* temp_image;
	IplImage* skel_image;
	IplImage* erod_image;
	jl_cv_output_t output;
	m_u8_t convertdone;
	jl_tex_t textures[1]; // Textures.
	CvMemStorage* storage;
	IplConvKernel* element;
}jl_cv_t;

// Make & Destroy
jl_cv_t* jl_cv_init(jl_t* jlc);
void jl_cv_kill(jl_cv_t* jl_cv);
// Use the webcam or image
void jl_cv_init_webcam(jl_cv_t* jl_cv, jl_cv_output_t output);
void jl_cv_init_image(jl_cv_t* jl_cv, jl_cv_output_t output, str_t fname);
//
void jl_cv_loop_webcam(jl_cv_t* jl_cv); 
void jl_cv_loop_image(jl_cv_t* jl_cv, str_t fname);
// Apply Filters
void jl_cv_loop_filter(jl_cv_t* jl_cv, u8_t* hsv);
u32_t jl_cv_loop_detect_circle(jl_cv_t* jl_cv, u32_t max,
	jl_rect_t* rtn_circles);
u32_t jl_cv_loop_detect_lines(jl_cv_t* jl_cv, u32_t max_rtn,
	i32_t filter_out, u32_t minlen, jl_cv_line_t* rtn_lines);
// Erosion.
void jl_cv_erode(jl_cv_t* jl_cv);
void jl_cv_skeleton(jl_cv_t* jl_cv, int w, int h, int* values);
void jl_cv_struct_erode(jl_cv_t* jl_cv, int w, int h, int* values);
// Export Ending Image to texture.
double jl_cv_loop_maketx(jl_cv_t* jl_cv);

// - draw -

void jl_cv_draw_circle(jl_cv_t* jl_cv, jl_rect_t circle);
void jl_cv_draw_line(jl_cv_t* jl_cv, jl_cv_line_t line);
