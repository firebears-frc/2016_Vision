#include "header/main.h"

// settings
#define HOSTNAME "roborio-2846-frc.local" /*"10.30.21.108"*/
#define FILENAME "Worlds/SuperBlur.jpeg"
#define VI_WEBCAM 0
#define WINDOWED 1
#define DRAW_TARGET 1
#define DO_PROCESS 1
#define VIDEO_STREAM 0
#define VI_DISPLAY JL_CV_CHNG
#define DESTINATION "/home/pi/2016_Vision/Worlds"

int oldtbiu = 0;
int shape[] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1
};
m_u8_t color[] = { 127, 255, 127, 255 };
//	uint8_t bounds[] = { 38, 2, 233, 70, 8, 236 };
//
//	uint8_t bounds[] = { 38, 2, 234, 78, 8, 236 };
// With Red Light Ring
//	uint8_t bounds[] = { 40, 0, 150, 160, 255, 255 };
// With Green Light Ring
// IronDale
//	uint8_t bounds[] = { 150, 150, 0, 255, 255, 155 };
// Small Engines Room
//	uint8_t bounds[] = { 227, 200, 140, 255, 255, 250 };
	//
//	uint8_t bounds[] = { 85, 130, 0, 90, 230, 255 };
//	uint8_t bounds[] = { 87, 0, 0, 90, 255, 255 };
//	uint8_t bounds[] = { 0, 0, 150, 255, 230, 255 };
//	uint8_t bounds[] = { 30, 0, 220, 85, 15, 255 };
uint8_t bounds[6];
uint8_t haveParameters = 1;

#define MEMTESTER(a, b) //jl_print(a, b) //memtester(a, b  )
void memtester(jl_t* jl, str_t name) {
	int diff = jl_mem_tbiu() - oldtbiu;
	printf("%s %d\n", name, diff);
	oldtbiu = jl_mem_tbiu();
}

static void vi_print_bounds(void) {
	printf("%d, %d, %d, %d, %d, %d\n", bounds[0], bounds[1],
		bounds[2], bounds[3], bounds[4], bounds[5]);
}

static inline void vi_redraw(jl_t* jl) {
#if WINDOWED == 1
	ctx_t* ctx = jl->prg_context;
	double ar;
	double height = jl_gl_ar(jl->jlgr) - .05;

#if DRAW_TARGET == 1
	// Draw target
	MEMTESTER(jl, "Draw target");
	u32_t drawsize = ctx->target.h / 2;
	jl_cv_draw_circle(ctx->jl_cv, (jl_rect_t) {
		ctx->targetx,ctx->targety,drawsize/2,0});
	jl_cv_draw_line(ctx->jl_cv, (jl_cv_line_t) {
		cvPoint(ctx->targetx-drawsize,ctx->targety),
		cvPoint(ctx->targetx+drawsize,ctx->targety)});
	jl_cv_draw_line(ctx->jl_cv, (jl_cv_line_t) {
		cvPoint(ctx->targetx,ctx->targety-drawsize),
		cvPoint(ctx->targetx,ctx->targety+drawsize)});
#endif
	MEMTESTER(jl, "Target drawn");
	// Change to image
	ar = jl_cv_loop_maketx(ctx->jl_cv);
	MEMTESTER(jl, "vosing");
	jlgr_vos_texture(jl->jlgr, &(ctx->vos[0]),
		(jl_rect_t) { 0.f, 0.f, height / ar, height },
		&(ctx->jl_cv->textures[0]), 0, 255);
	MEMTESTER(jl, "Drawing vo");
	jlgr_draw_vo(jl->jlgr, &(ctx->vos[0]), NULL);
	MEMTESTER(jl, "Text draw");
	jlgr_draw_text(jl->jlgr, jl_mem_format(jl, "movex:%d, movey:%d",
			ctx->movex, ctx->movey),
		(jl_vec3_t) { 0., .025, 0. }, ctx->font);
	MEMTESTER(jl, "slider draw");
	if(haveParameters == 0) {
		int i;
		for(i = 0; i < 3; i++)
			jlgr_sprite_draw(jl->jlgr, ctx->slider[i]);
	}
#endif
}

void vi_get_input(ctx_t* ctx) {
#if VI_WEBCAM == 1
	jl_cv_loop_webcam(ctx->jl_cv);
#else
	jl_cv_loop_image(ctx->jl_cv, FILENAME);
#endif
}

static inline void vi_stream_video(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;	
	m_u8_t* pixels = NULL;
	m_u16_t w = 0;
	m_u16_t h = 0;

	jl_cv_get_img(ctx->jl_cv, &w, &h, &pixels);
	jl_nt_push_data(ctx->jl_nt, NT_PIXELS, pixels, w * h * 3);
}

void vi_take_picture(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;
	data_t* push_data = jl_cv_loop_makejf(ctx->jl_cv);
	time_t mytime;

	mytime = time(NULL);
	jl_file_save(jl, push_data->data,
		jl_mem_format(jl, DESTINATION "/%s.jpeg", ctime(&mytime)),
		push_data->size);
}

static inline void vi_push(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;

	MEMTESTER(jl, "nt start");
	jl_nt_push_num(ctx->jl_nt, NT_DISTANCE, (double)(ctx->movey));
	jl_nt_push_num(ctx->jl_nt, NT_ANGLE, (double)(ctx->movex));
	jl_nt_push_num(ctx->jl_nt, NT_FPS, (double)(1./jl->time.psec));
	jl_nt_push_num(ctx->jl_nt, NT_SIZE, (double)(ctx->size));
	MEMTESTER(jl, "nt end");
#if VIDEO_STREAM == 1
	MEMTESTER(jl, "video stream begin");
	vi_stream_video(jl);
//	jl_cv_get_img(ctx->jl_cv, NULL, NULL, NULL);
	MEMTESTER(jl, "video stream end");
#endif
}

static inline void vi_process(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;

	MEMTESTER(jl, "loop");
// Read image
	vi_get_input(ctx);
	MEMTESTER(jl, "read_image");
// Filter colors
	jl_cv_loop_filter(ctx->jl_cv, bounds);
// Erode Blobs
//	jl_cv_struct_erode(ctx->jl_cv, 5, 5, shape);
// Blob Detect
	jl_cv_loop_bigobject(ctx->jl_cv, &ctx->target);

	ctx->size = ctx->target.h;
//	ctx->target = blobs[maxi];
	ctx->targetx = ctx->target.x + (ctx->target.w / 2);
	ctx->targety = ctx->target.y + (ctx->target.h / 2);
	ctx->movex = ctx->targetx - (ctx->imgx / 2);
	ctx->movey = ctx->targety - (ctx->imgy / 2);

	jl_print(jl, "FPS = %f, items = %d, moxex = %d, movey = %d, h = %d",
		(double)(1./jl->time.psec), ctx->item_count, ctx->movex,
		ctx->movey, ctx->size);

	MEMTESTER(jl, "find_blob");
}

void vi_wdns(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;
	if(haveParameters == 0) {
		bounds[0] = (uint8_t)(ctx->hsv_lo[0] * 255.);
		bounds[1] = (uint8_t)(ctx->hsv_lo[1] * 255.);
		bounds[2] = (uint8_t)(ctx->hsv_lo[2] * 255.);
		bounds[3] = (uint8_t)(ctx->hsv_hi[0] * 255.);
		bounds[4] = (uint8_t)(ctx->hsv_hi[1] * 255.);
		bounds[5] = (uint8_t)(ctx->hsv_hi[2] * 255.);
		jl_print(jl, "To reproduce results: %d %d %d %d %d %d", bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);
	}
#if DO_PROCESS == 1
	vi_process(jl);
#else
	vi_get_input(jl->prg_context);
#endif
	//
	// Take a picte
	//vi_take_picture(jl);
	vi_push(jl);
	vi_redraw(jl);
}

static void vi_exit(jl_t* jl) {
	exit(0);
//	ctx_t* ctx = jl->prg_context;

//	jl_cv_kill(ctx->jl_cv);
}

#if WINDOWED == 1
static void vi_resz(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;
	float ar = jl_gl_ar(jl->jlgr);
	jl_rect_t rect[] = {
		{ 0., ar - .05, 1./3., .05 },
		{ 1./3., ar - .05, 1./3., .05 },
		{ 2./3., ar - .05, 1./3., .05 }};
	int i;
	if(haveParameters == 0) {
		for(i = 0; i < 3; i++)
			jlgr_sprite_resize(jl->jlgr, ctx->slider[i], &rect[i]);
	}
}
#endif

static void vi_mdin(jl_t* jl) {
#if WINDOWED == 1
	jlgr_loop_set(jl->jlgr, vi_wdns, jl_dont, vi_wdns, vi_resz);
#endif
}

static void vi_loop(jl_t* jl) {
#if WINDOWED == 1
	int i;
	ctx_t* ctx = jl_get_context(jl);

	jlgr_loop(jl->jlgr);
	if(haveParameters == 0) {
		for(i = 0; i < 3; i++)
			jlgr_sprite_loop(jl->jlgr, ctx->slider[i]);
	}
#else
	vi_wdns(jl);
#endif	
}

static inline void vi_init_modes(jl_t* jl) {
	//Set mode data
	jl_mode_set(jl, VI_MODE_EDIT, (jl_mode_t) { vi_mdin,vi_loop,vi_exit });
	//Leave terminal mode
	jl_mode_switch(jl, VI_MODE_EDIT);
}

#if WINDOWED == 1
static inline void vi_init_tasks(jlgr_t* jlgr) {
//	jlgr_menu_addicon_slow(jlgr);
}
#endif

static inline void vi_init_ctx(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;
	ctx->jl_cv = jl_cv_init(jl);
	ctx->jl_nt = jl_nt_init(jl, HOSTNAME);
	ctx->font = (jl_font_t) { 0, JL_IMGI_ICON, 0, color, .025f };

	jl_print(jl, "that %p", jl->prg_context);
}

static inline void vi_init_vos(jl_t* jl) {
#if WINDOWED == 1
	ctx_t* ctx = jl->prg_context;

	ctx->vos = jl_gl_vo_make(jl->jlgr, 2);
#endif
}

static inline void vi_init_cv(jl_t* jl) {
	ctx_t* vi = jl->prg_context;

#if VI_WEBCAM == 1
	jl_cv_init_webcam(vi->jl_cv, VI_DISPLAY, JL_CV_FLIPY, 0);
#else
	jl_cv_init_image(vi->jl_cv, VI_DISPLAY, FILENAME, JL_CV_FLIPN);
#endif
	jl_cv_get_img(vi->jl_cv, &vi->imgx, &vi->imgy, NULL);
	jl_nt_push_num(vi->jl_nt, "video_stream/resw", vi->imgx);
	jl_nt_push_num(vi->jl_nt, "video_stream/resh", vi->imgy);
}

static inline void vi_init_sliders(jl_t* jl) {
	ctx_t* ctx = jl->prg_context;
	float ar = jl_gl_ar(jl->jlgr);
	int i;
	jl_rect_t rect[] = {
		{ 0., ar - .05, 1./3., .05 },
		{ 1./3., ar - .05, 1./3., .05 },
		{ 2./3., ar - .05, 1./3., .05 }};
	for(i = 0; i < 3; i++)
		ctx->slider[i] = jlgr_gui_slider(jl->jlgr, rect[i], 2,
			&ctx->hsv_lo[i], &ctx->hsv_hi[i]);
}

#if WINDOWED == 1
static void vi_init_graphics(jl_t* jl) {
	jlgr_t* jlgr = jl->jlgr;

	vi_init_tasks(jlgr);
	vi_init_vos(jl);
	vi_init_cv(jl);
	if(haveParameters == 0) vi_init_sliders(jl);
}
#endif

static void vi_init(jl_t* jl) {
	jl_print_function(jl, "2846_Vision");
	vi_init_ctx(jl);
#if WINDOWED == 1
	jlgr_init(jl, 0, vi_init_graphics);
#else
	vi_init_cv(jl);
#endif
	vi_init_modes(jl);
	jl_print_return(jl, "2846_Vision");
}

static void vi_kill(jl_t* jl) {
	jlgr_kill(jl->jlgr);
}

int main(int argc, char* argv[]) {
	if(argc != 7) {
		haveParameters = 0;
	}else{
		int i;
		for(i = 0; i < 6; i++) {
			int x;
			sscanf(argv[i + 1], "%d", &x);
			bounds[i] = x;
		}
		vi_print_bounds();
	}
	return jl_start(vi_init, vi_kill, "2016 Vision", sizeof(ctx_t));
}
