#include "jl_cv.h"

/* Static Functions */

static void jl_cv_rgb_to_hsvandgrey__(jl_cv_t* jl_cv) {
	if(jl_cv->flip!=-2) cvFlip(jl_cv->image_rgb, NULL, jl_cv->flip);
	cvCvtColor(jl_cv->image_rgb, jl_cv->image_hsv, CV_RGB2HSV);
	cvCvtColor(jl_cv->image_rgb, jl_cv->gray_image, CV_RGB2GRAY);
	cvSmooth(jl_cv->gray_image, jl_cv->gray_blur, CV_GAUSSIAN, 15,15,0,0);
	jl_cv->convertdone = false;
}

void jl_cv_disp_gray_(jl_cv_t* jl_cv) {
	cvCvtColor(jl_cv->gray_image, jl_cv->image_hsv, CV_GRAY2BGR );
	cvCvtColor(jl_cv->image_hsv, jl_cv->image_hsv, CV_BGR2HSV );
}

static void jl_cv_webcam_get__(jl_cv_t* jl_cv) {
	jl_cv->image_rgb = cvQueryFrame(jl_cv->camera);
	if( jl_cv->image_rgb == NULL ) {
		fprintf(stderr, "couldn't retrieve frame\n" );
		exit(1);
	}
	cvCvtColor(jl_cv->image_rgb, jl_cv->image_rgb, CV_RGBA2RGB );
	cvResize(jl_cv->image_rgb, jl_cv->image_rgb, CV_INTER_LINEAR);
}

static void jl_cv_image_get__(jl_cv_t* jl_cv, str_t fname) {
	if(jl_cv->image_rgb) cvReleaseImage( &(jl_cv->image_rgb) );
	jl_cv->image_rgb = cvLoadImage(fname, 1);
	if( jl_cv->image_rgb == NULL ) {
		fprintf(stderr, "Could not load image file: %s\n", fname );
		exit(1);
	}
	cvResize(jl_cv->image_rgb, jl_cv->image_rgb, CV_INTER_LINEAR);
}

static void jl_cv_hsv_init__(jl_cv_t* jl_cv) {
	jl_cv->image_hsv = cvCreateImage(cvGetSize(jl_cv->image_rgb),8,3);
	jl_cv->gray_image = cvCreateImage(cvGetSize(jl_cv->image_rgb),8,1);
	jl_cv->gray_blur = cvCreateImage(cvGetSize(jl_cv->image_rgb),8,1);
	jl_cv->temp_image = cvCreateImage(cvGetSize(jl_cv->image_rgb),8,1);
	jl_cv->skel_image = cvCreateImage(cvGetSize(jl_cv->image_rgb),8,0);
	jl_cv->erod_image = cvCreateImage(cvGetSize(jl_cv->image_rgb),8,0);
}

static void jl_cv_hsv_filter__(jl_cv_t* jl_cv, u8_t* hsv) {
	cvInRangeS(jl_cv->image_hsv, cvScalar(hsv[0], hsv[1], hsv[2], 255),
		cvScalar(hsv[3], hsv[4], hsv[5], 255), jl_cv->gray_image);
	jl_cv_disp_gray_(jl_cv);
}

void jl_cv_getoutput(jl_cv_t* jl_cv) {
	if(!jl_cv->convertdone) {
		if(jl_cv->output == JL_CV_ORIG) {
//			jl_cv_img_crop(jl_cv, jl_cv->image_rgb, 0);
		}else if(jl_cv->output == JL_CV_CHNG) {
			cvCvtColor(jl_cv->image_hsv, jl_cv->image_rgb, CV_HSV2RGB);
//			jl_cv_img_crop(jl_cv, jl_cv->image_hsv, CV_HSV2RGB);
		}else if(jl_cv->output == JL_CV_GRAY) {
			cvCvtColor(jl_cv->gray_image, jl_cv->image_rgb, CV_GRAY2RGB);
//			jl_cv_img_crop(jl_cv, jl_cv->gray_image, CV_GRAY2RGB);
		}
		jl_cv->convertdone = true;
	}
}

static void jl_cv_setf(jl_cv_t* jl_cv, jl_cv_flip_t f) {
	if(f == JL_CV_FLIPX)
		jl_cv->flip = 0;
	else if(f == JL_CV_FLIPY)
		jl_cv->flip = 1;
	else if(f == JL_CV_FLIPB)
		jl_cv->flip = -1;
	else if(f == JL_CV_FLIPN)
		jl_cv->flip = -2;
}

/* Export Functions */

jl_cv_t* jl_cv_init(jl_t* jl) {
	jl_cv_t* jl_cv = jl_memi(jl, sizeof(jl_cv_t));
	jl_cv->jl = jl;
	jl_cv->image_rgb = NULL;
	jl_cv->storage = cvCreateMemStorage(0);
	jl_cv->element = cvCreateStructuringElementEx(3, 3, 0, 0,
		CV_SHAPE_CROSS, NULL);
	jl_cv->jpeg = jl_data_make(0);
	jl_cv->texturesinited = 0;
	return jl_cv;
}

void jl_cv_kill(jl_cv_t* jl_cv) {
	cvReleaseCapture(&(jl_cv->camera));
}

/**
 * Initialize a webcam for streaming.
 * @param jl_cv: The library context.
 * @param output: What should be displayed after the filters are applied.
 * @param f: How the image should be rotated.
 * @param which: Which webcam to open ( 0 = built-in, 1 = external, unless no
	built-in camera then, 0 = external)
**/
void jl_cv_init_webcam(jl_cv_t* jl_cv, jl_cv_output_t output, jl_cv_flip_t f,
	u32_t which)
{
	jl_cv_setf(jl_cv, f);
	jl_cv->camera = cvCaptureFromCAM(which); // open the camera id

	// If webcam can't be opened, then fail
	if( jl_cv->camera == NULL ) {
		jl_print(jl_cv->jl, "Failed to open camera #%d", which);
		exit(1);
	}
	cvSetCaptureProperty(jl_cv->camera, CV_CAP_PROP_BRIGHTNESS, 0.);
	cvSetCaptureProperty(jl_cv->camera, CV_CAP_PROP_SATURATION, .5);
	cvSetCaptureProperty(jl_cv->camera, CV_CAP_PROP_FRAME_WIDTH, 200.);
	cvSetCaptureProperty(jl_cv->camera, CV_CAP_PROP_FRAME_HEIGHT, 200.);
	jl_cv_webcam_get__(jl_cv);
	jl_cv_hsv_init__(jl_cv);
	jl_cv->output = output;
}

void jl_cv_init_image(jl_cv_t* jl_cv, jl_cv_output_t output, str_t fname,
	jl_cv_flip_t f)
{
	jl_cv_setf(jl_cv, f);
	jl_cv_image_get__(jl_cv, fname);
	jl_cv_hsv_init__(jl_cv);
	jl_cv->output = output;
}

void jl_cv_loop_webcam(jl_cv_t* jl_cv) {
	// Retrieve the image from camera ID:0 then store in image
	jl_cv_webcam_get__(jl_cv);
	jl_cv_rgb_to_hsvandgrey__(jl_cv);
}

void jl_cv_loop_image(jl_cv_t* jl_cv, str_t fname) {
	// Load the image from a file, then store in image
	jl_cv_image_get__(jl_cv, fname);
	jl_cv_rgb_to_hsvandgrey__(jl_cv);
}

void jl_cv_loop_filter(jl_cv_t* jl_cv, u8_t* hsv) {
	jl_cv_hsv_filter__(jl_cv, hsv);
}

/**
 * Detect circles.
 * @param jl_cv: JL_CV context.
 * @param max_rtn: The number of jl_rect_t's in the array. jl_rect_t circles['x'];
 * @param rtn_circles: The rectangle array that is where the circles are output.
 * @returns number of circles
**/
u32_t jl_cv_loop_detect_circle(jl_cv_t* jl_cv, u32_t max_rtn,
	jl_rect_t* rtn_circles)
{
	CvSeq* circles;
	int i, count;

	circles = cvHoughCircles(
		jl_cv->gray_image,		// The image
		jl_cv->storage,			// Output of found circles
		CV_HOUGH_GRADIENT,		// Only option
		4,				// image:accumulator resolution
		jl_cv->gray_image->width/4,	// Min. dist. between circles
		200,				// 2nd threshold in canny()
		100,				// Smaller for more(bad)circles.
		0,				// Minimum circle radius
		jl_cv->gray_image->height/4	// Maximum circle radius
	);
	count = circles->total >= max_rtn ? max_rtn : circles->total;
	for(i = 0; i < count; i++){
		float* p = (float*)cvGetSeqElem(circles, i);
		rtn_circles[i] = (jl_rect_t){ p[0], p[1], p[2], p[2] };
	}
	jl_cv_disp_gray_(jl_cv);
	return count;
}

/**
 * Detect lines.
 * @param jl_cv: JL_CV context.
 * @param filter_out: The higher this number, the less lines are outputed.
 * @param minlen: The minimum length the outputed lines may be.
 * @returns number of lines.
**/
u32_t jl_cv_loop_detect_lines(jl_cv_t* jl_cv, u32_t max_rtn,
	i32_t filter_out, u32_t minlen, jl_cv_line_t* rtn_lines)
{
	CvSeq* lines;
	int i, count;

	lines = cvHoughLines2(
		jl_cv->gray_image,	// The image
		jl_cv->storage,		// Output of found line segments
		CV_HOUGH_PROBABILISTIC, // Find line segments, not lines
		1,			// Distance resolution (in pixels)
		CV_PI/25,		// Angle resolution (in radians)
		filter_out,		// Accumulator threshold parameter
// 2 Lines to comment out on rpi
		minlen,			// Minimum line length
		minlen*2,		// Max gap between line seg.s to join.
//
		0, CV_PI		// Default Range in C++
	);
	count = lines->total >= max_rtn ? max_rtn : lines->total;
	for(i = 0; i < count; i++){
		CvPoint* p = (CvPoint*)cvGetSeqElem(lines, i);
		rtn_lines[i].p1 = p[0], rtn_lines[i].p2 = p[1];
	}
	jl_cv_disp_gray_(jl_cv);
	return count;
}

/**
 * Find the width and height of detected objects.
**/
u32_t jl_cv_loop_objectrects(jl_cv_t* jl_cv,u32_t max_rtn,jl_cv_rect_t* rtn_rects){
	int i, total, count;
	CvSeq *contours = NULL;
	CvRect rect;

	jl_cv_disp_gray_(jl_cv);
	total = cvFindContours(
		jl_cv->gray_image,	// The image
		jl_cv->storage,		// Output of found contours
		&contours,		// ptr to 1st contour
		sizeof(CvContour),
		CV_RETR_LIST,
		CV_CHAIN_APPROX_SIMPLE,
		cvPoint(0,0)
	);
	count = total >= max_rtn ? max_rtn : total;
	for(i = 0; i < count; i++) {
		rect = cvBoundingRect(contours, 0);
		rtn_rects[i] = (jl_cv_rect_t) {
			rect.x, rect.y, rect.width, rect.height };
		contours = (CvSeq *)(contours->h_next);
	}
	return total;
}

void jl_cv_erode(jl_cv_t* jl_cv) {
	cvErode(jl_cv->gray_image, jl_cv->gray_image, NULL, 1);
}

void jl_cv_skeleton(jl_cv_t* jl_cv, int w, int h, int* values) {
//	m_u8_t done;
//	do {
		cvMorphologyEx(jl_cv->gray_image, jl_cv->temp_image, 
			jl_cv->erod_image, jl_cv->element, 3, 1);
		cvNot(jl_cv->temp_image, jl_cv->temp_image);
		cvAnd(jl_cv->gray_image, jl_cv->temp_image, jl_cv->temp_image, NULL);
		cvOr(jl_cv->skel_image, jl_cv->temp_image, jl_cv->skel_image, NULL);

/*
//	while(1) {
		cvErode(jl_cv->gray_image, jl_cv->temp_image, jl_cv->element, 4);
		cvDilate(jl_cv->temp_image, jl_cv->temp_image, jl_cv->element, 4);
		cvErode(jl_cv->temp_image, jl_cv->temp_image, jl_cv->element, 1);
		cvSub(jl_cv->gray_image, jl_cv->temp_image, jl_cv->temp_image,
			NULL);
		cvOr(jl_cv->skel_image, jl_cv->temp_image, jl_cv->skel_image, NULL);
		cvCopy(jl_cv->erod_image, jl_cv->gray_image, jl_cv->skel_image );

//		double max;
//		cvMinMaxLoc(jl_cv->gray_image, 0, &max, NULL, NULL, NULL);
//		if(max == 0) break;
//	}
	jl_cv_disp_gray_(jl_cv);
//	jl_cv_erode(jl_cv);*/
//	cvDilate(jl_cv->gray_image, jl_cv->gray_image, NULL, 1);
//	cvSubtract()
//	cvOr(jl_cv->gray_image, jl_cv->gray_image);
}

void jl_cv_struct_erode(jl_cv_t* jl_cv, int w, int h, int* values) {
	IplConvKernel* kernel = cvCreateStructuringElementEx(w, h, 0, 0,
		CV_SHAPE_CUSTOM, values);
	cvErode(jl_cv->gray_image, jl_cv->gray_image, kernel, 1);
	cvDilate(jl_cv->gray_image, jl_cv->gray_image, kernel, 1);
	cvErode(jl_cv->gray_image, jl_cv->gray_image, kernel, 1);
}

/**
 * Get width, height and pixels of an image.
**/
void jl_cv_get_img(jl_cv_t* jl_cv, m_u16_t* w, m_u16_t* h, m_u8_t** pixels) {
	if(w) *w = jl_cv->image_rgb->width;
	if(h) *h = jl_cv->image_rgb->height;
	if(pixels) {
		jl_cv_getoutput(jl_cv);
		*pixels = (void*)jl_cv->image_rgb->imageData;
	}
}

/**
 * Make a texture from the image in the opencv buffer.
 * @param jl_cv: JL_CV context.
 * @returns: The y aspect ratio of the image ( y / x).
**/
double jl_cv_loop_maketx(jl_cv_t* jl_cv) {
	jl_cv_getoutput(jl_cv);
	//
	if(jl_cv->texturesinited == 0) {
		jl_gl_pbo_new(jl_cv->jl->jlgr, &(jl_cv->textures[0]),
			(void*)jl_cv->image_rgb->imageData,
			jl_cv->image_rgb->width,
			jl_cv->image_rgb->height, 3);
		jl_cv->texturesinited = 1;
	}
	// Update the output image in a texture.
	jl_gl_pbo_set(jl_cv->jl->jlgr, &(jl_cv->textures[0]),
		(void*)jl_cv->image_rgb->imageData,
		jl_cv->image_rgb->width,
		jl_cv->image_rgb->height, 3);
	return ((double)jl_cv->image_rgb->height) /
		((double)jl_cv->image_rgb->width);
}

/**
 * Get JPEG file data from the current image.  The returned variable should not
 *	be freed.
 * @param jl_cv: JL_CV context.
 * @returns the data.
**/
data_t* jl_cv_loop_makejf(jl_cv_t* jl_cv) {
	jl_t* jl = jl_cv->jl;
	data_t* jpeg = jl_vi_make_jpeg(jl, 100, 
		(void*)jl_cv->image_rgb->imageData, jl_cv->image_rgb->width,
		jl_cv->image_rgb->height);

	jl_cv_getoutput(jl_cv);
	// Clear the final string.
	jl_data_clear(jl, jl_cv->jpeg);
	// Add data.
	jl_data_insert_data(jl, jl_cv->jpeg, jpeg->data, jpeg->size);
	// Free temporary string.
	jl_data_free(jpeg);
	return jl_cv->jpeg;
}
