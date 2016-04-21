#include <cv.h>
#include "highgui.h"
#include "jl.h"

typedef enum {
	JL_CV_ORIG, // the original image will be output.
	JL_CV_CHNG, // the modified image will be output.
	JL_CV_GRAY, // the modified image will be output. ( Grayscale )
}jl_cv_output_t;

typedef enum {
	JL_CV_FLIPX,
	JL_CV_FLIPY,
	JL_CV_FLIPB,
	JL_CV_FLIPN,
}jl_cv_flip_t;

typedef struct {
	CvPoint p1;
	CvPoint p2;
}jl_cv_line_t;

typedef struct {
	int x, y, w, h;
}jl_cv_rect_t;

typedef struct{
	jl_t* jl;
	CvCapture* camera;
	IplImage* image_rgb;
	IplImage* image_hsv;
	IplImage* gray_image;
	IplImage* gray_blur;
	IplImage* temp_image;
	IplImage* skel_image;
	IplImage* erod_image;
	jl_cv_output_t output;
	m_u8_t convertdone;
	m_i8_t flip;
	jl_tex_t textures[1]; // Textures.
	m_u8_t texturesinited;
	CvMemStorage* storage;
	IplConvKernel* element;
	data_t* jpeg;
}jl_cv_t;

// Make & Destroy
jl_cv_t* jl_cv_init(jl_t* jl);
void jl_cv_kill(jl_cv_t* jl_cv);
// Use the webcam or image
void jl_cv_init_webcam(jl_cv_t* jl_cv, jl_cv_output_t output, jl_cv_flip_t f,
	u32_t which);
void jl_cv_init_image(jl_cv_t* jl_cv, jl_cv_output_t output, str_t fname,
	jl_cv_flip_t f);
//
void jl_cv_loop_webcam(jl_cv_t* jl_cv); 
void jl_cv_loop_image(jl_cv_t* jl_cv, str_t fname);
// Apply Filters
void jl_cv_loop_filter(jl_cv_t* jl_cv, u8_t* hsv);
u32_t jl_cv_loop_detect_circle(jl_cv_t* jl_cv, u32_t max,
	jl_rect_t* rtn_circles);
u32_t jl_cv_loop_detect_lines(jl_cv_t* jl_cv, u32_t max_rtn,
	i32_t filter_out, u32_t minlen, jl_cv_line_t* rtn_lines);
u32_t jl_cv_loop_objectrects(jl_cv_t* jl_cv,u32_t max_rtn,jl_cv_rect_t* rtn_rects);
// Erosion.
void jl_cv_erode(jl_cv_t* jl_cv);
void jl_cv_skeleton(jl_cv_t* jl_cv, int w, int h, int* values);
void jl_cv_struct_erode(jl_cv_t* jl_cv, int w, int h, int* values);
// Get values
void jl_cv_get_img(jl_cv_t* jl_cv, m_u16_t* w, m_u16_t* h, m_u8_t** pixels);
// Export Ending Image to texture / data.
double jl_cv_loop_maketx(jl_cv_t* jl_cv);
data_t* jl_cv_loop_makejf(jl_cv_t* jl_cv);

// - draw -

void jl_cv_draw_circle(jl_cv_t* jl_cv, jl_rect_t circle);
void jl_cv_draw_line(jl_cv_t* jl_cv, jl_cv_line_t line);
void jl_cv_draw_rect(jl_cv_t* jl_cv, jl_cv_rect_t rect);
