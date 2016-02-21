#include "header/main.h"
//#define TEST
#ifdef TEST
//	#define HOSTNAME "10.0.0.5"
	#define HOSTNAME "10.30.21.108"
	#define VI_WEBCAM 1
	#define HEADLESS
#else
	#define HOSTNAME "10.30.21.108"
//	#define HOSTNAME "roborio-2846-frc.local"
	#define VI_WEBCAM 0
	#define HEADLESS
#endif

int oldtbiu = 0;
int shape[] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1
};
m_u8_t color[] = { 127, 255, 127, 255 };

#define MEMTESTER(a, b) //memtester(a, b  )
void memtester(jl_t* jlc, str_t name) {
	int diff = jl_me_tbiu() - oldtbiu;
	printf("%s %d\n", name, diff);
	oldtbiu = jl_me_tbiu();
}

static inline void vi_redraw(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
#ifndef HEADLESS
	double ar;
#endif

	// Draw target
	u32_t drawsize = ctx->target.h / 2;
	jl_cv_draw_circle(ctx->jl_cv, (jl_rect_t) {
		ctx->targetx,ctx->targety,drawsize/2,0});
	jl_cv_draw_line(ctx->jl_cv, (jl_cv_line_t) {
		cvPoint(ctx->targetx-drawsize,ctx->targety),
		cvPoint(ctx->targetx+drawsize,ctx->targety)});
	jl_cv_draw_line(ctx->jl_cv, (jl_cv_line_t) {
		cvPoint(ctx->targetx,ctx->targety-drawsize),
		cvPoint(ctx->targetx,ctx->targety+drawsize)});
	// Change to image
#ifndef HEADLESS
	ar = jl_cv_loop_maketx(ctx->jl_cv);
	jl_gr_vos_texture(jlc, &(ctx->vos[0]),
		(jl_rect_t) { 0.f, 0.f, ar, jl_gl_ar(jlc) },
		&(ctx->jl_cv->textures[0]), 0, 255);
	jl_gr_draw_vo(jlc, &(ctx->vos[0]), NULL);
	jl_gr_draw_text(jlc, jl_me_format(jlc, "x:%d, y:%d, z:%d",
			ctx->targetx, ctx->targety, ctx->targetz),
		(jl_vec3_t) { 0., 0., 0. }, ctx->font);
	jl_gr_draw_text(jlc, jl_me_format(jlc, "movex:%d, movey:%d",
			ctx->movex, ctx->movey),
		(jl_vec3_t) { 0., .025, 0. }, ctx->font);
#endif
}

void vi_get_input(ctx_t* ctx) {
#if VI_WEBCAM == 1
	jl_cv_loop_webcam(ctx->jl_cv);
#else
	jl_cv_loop_image(ctx->jl_cv, "Field_Images/20.jpg");
#endif
}

static inline void vi_push(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	MEMTESTER(jlc, "nt start");
	jl_nt_push_num(ctx->jl_nt, NT_DISTANCE,
		(double)ctx->targetz);
	jl_nt_push_num(ctx->jl_nt, NT_ANGLE, (double)ctx->movex);
	jl_nt_push_num(ctx->jl_nt, NT_FPS,
		(double)(1./jlc->time.psec));
//	if(jl_nt_pull_bool(ctx->jl_nt, NT_CALIBRATION)) {
//		jl_io_print(jlc, "on");
	MEMTESTER(jlc, "push_data_start");
		strt push_data = jl_cv_loop_makejf(ctx->jl_cv);
	MEMTESTER(jlc, "push_data_start2");
		jl_nt_push_data(ctx->jl_nt, NT_IMG, push_data->data,
			push_data->size);
	MEMTESTER(jlc, "push_data_finish");
//	}else{
//		jl_io_print(jlc, "off");
//	}
}

static inline void vi_loop(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
	m_u16_t i = 0;
	jl_cv_rect_t blobs[30];
#ifdef TEST
	uint8_t bounds[] = { 20, 200, 90, 40, 255, 180 };
#else
	uint8_t bounds[] = {
		jl_nt_pull_num(ctx->jl_nt, "Preferences/vision.hue.lo", 20),
		jl_nt_pull_num(ctx->jl_nt, "Preferences/vision.sat.lo", 200),
		jl_nt_pull_num(ctx->jl_nt, "Preferences/vision.val.lo", 90),
		jl_nt_pull_num(ctx->jl_nt, "Preferences/vision.hue.hi", 40),
		jl_nt_pull_num(ctx->jl_nt, "Preferences/vision.sat.hi", 255),
		jl_nt_pull_num(ctx->jl_nt, "Preferences/vision.val.hi", 180) };
#endif
	int maxw = 0, maxi = 0;

	MEMTESTER(jlc, "loop");
// Read image
	vi_get_input(ctx);
	MEMTESTER(jlc, "read_image");
// Filter colors
	jl_cv_loop_filter(ctx->jl_cv, bounds);
// Erode Blobs
	jl_cv_struct_erode(ctx->jl_cv, 5, 5, shape);
// Blob Detect
	ctx->item_count = jl_cv_loop_objectrects(ctx->jl_cv, 30, blobs);
// Find the Best Blob
	MEMTESTER(jlc, "blob_detect");
	for(i = 0; i < ctx->item_count; i++) {
		if(blobs[i].w > maxw) {
			maxw = blobs[i].w;
			maxi = i;
		}
//		jl_cv_draw_rect(ctx->jl_cv, blobs[i]);
	}
	ctx->target = blobs[maxi];
	ctx->targetx = ctx->target.x + (ctx->target.w / 2);
	ctx->targety = ctx->target.y + (ctx->target.h / 2);
	ctx->targetz = (100 * ctx->imgy / ctx->target.y) - 100;
	ctx->movex = ctx->targetx - (ctx->imgx / 2);
	ctx->movey = ctx->targety - (ctx->imgy / 2);
	MEMTESTER(jlc, "find_blob");
}

void vi_wdns(jl_t* jlc) {
	jl_io_printc(jlc, "Run Frame");
	vi_loop(jlc);
	jl_io_printc(jlc, "Vi push");
	vi_push(jlc);
#ifndef HEADLESS
	vi_redraw(jlc);
#endif
}

// Called when window is made/resized.
static void vi_resz(jl_t* jlc) {
}

static void vi_exit(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	jl_cv_kill(ctx->jl_cv);
	jl_sg_exit(jlc);
}

static inline void vi_init_modes(jl_t* jlc) {
	//Set mode data
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_LOOP, vi_wdns);
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_RESZ, vi_resz);
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_EXIT, vi_exit);
	//Leave terminal mode
	jl_sg_mode_switch(jlc, VI_MODE_EDIT, JL_SG_WM_LOOP);
}

#ifndef HEADLESS
static inline void vi_init_tasks(jl_gr_t* jl_gr) {
	jl_gr_addicon_slow(jl_gr);
}
#endif

static inline void vi_init_ctx(jl_t* jlc) {
	jlc->uctx = NULL;
	jl_me_alloc(jlc, &jlc->uctx, sizeof(ctx_t), 0);
	ctx_t* ctx = jlc->uctx;
	ctx->jl_cv = jl_cv_init(jlc);
	ctx->jl_nt = jl_nt_init(jlc, HOSTNAME);
	ctx->font = (jl_font_t) { 0, JL_IMGI_ICON, 0,
		color, .025f };
}

static inline void vi_init_vos(jl_t* jlc) {
#ifndef HEADLESS
	ctx_t* ctx = jlc->uctx;

	ctx->vos = jl_gl_vo_make(jlc->jl_gr, 2);
#endif
}

static inline void vi_init_cv(jl_t* jlc) {
	ctx_t* vi = jlc->uctx;

#if VI_WEBCAM == 1
	jl_cv_init_webcam(vi->jl_cv, JL_CV_ORIG, JL_CV_FLIPY);
#else
	jl_cv_init_image(vi->jl_cv, JL_CV_CHNG, "Field_Images/0.jpg",
		JL_CV_FLIPN);
#endif
	jl_cv_img_size(vi->jl_cv, &vi->imgx, &vi->imgy);
}

static inline void vi_init_net(jl_t* jlc) {
	ctx_t* vi = jlc->uctx;
	jl_nt_push_bool(vi->jl_nt, NT_CALIBRATION, 0);
}

void vi_init(jl_t* jlc) {
	jl_io_function(jlc, "2846_Vision");
#ifndef HEADLESS
	jl_gr_draw_msge(jlc, 0, JL_IMGI_ICON, 1, "Initializing");
	jl_io_printc(jlc,"Initializing....");
#endif
	vi_init_modes(jlc);
	vi_init_ctx(jlc);
#ifndef HEADLESS
	vi_init_tasks(jlc->jl_gr);
	vi_init_vos(jlc);
#endif
	vi_init_cv(jlc);
	vi_init_net(jlc);
	jl_io_return(jlc, "2846_Vision");
}

int main(int argc, char* argv[]) {
	jl_init(vi_init);
	return -1;
}
