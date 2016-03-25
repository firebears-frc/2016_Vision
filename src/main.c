#include "header/main.h"

// Test settings
#define TEST 1
#define HOSTNAME "roborio-2846-frc.local" /*"10.30.21.108"*/
#define FILENAME "RedLightRingA.jpeg"
#define VI_WEBCAM 0
#define WINDOWED 1
#define PHOTO_CAPTURE 0

// Release settings
/*#define TEST 0
#define HOSTNAME "roborio-2846-frc.local" //"10.30.21.108"
#define VI_WEBCAM 1
#define WINDOWED 0
#define PHOTO_CAPTURE 0*/

// Don't save JPEGS if not capturing images from a camera.
#if VI_WEBCAM == 0
	#undef PHOTO_CAPTURE
	#define PHOTO_CAPTURE 0
#endif

int oldtbiu = 0;
int shape[] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1
/*	0, 1, 1, 1, 0,
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
	1, 1, 1, 1, 1,
	0, 1, 1, 1, 0*/
/*	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 0*/
};
m_u8_t color[] = { 127, 255, 127, 255 };

#define MEMTESTER(a, b) //memtester(a, b  )
void memtester(jl_t* jlc, str_t name) {
	int diff = jl_mem_tbiu() - oldtbiu;
	printf("%s %d\n", name, diff);
	oldtbiu = jl_mem_tbiu();
}

static inline void vi_redraw(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
#if WINDOWED == 1
	double ar;
#endif

#if WINDOWED == 1
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
	ar = jl_cv_loop_maketx(ctx->jl_cv);
	jl_gr_vos_texture(jlc->jl_gr, &(ctx->vos[0]),
		(jl_rect_t) { 0.f, 0.f, ar, jl_gl_ar(jlc->jl_gr) },
		&(ctx->jl_cv->textures[0]), 0, 255);
	jl_gr_draw_vo(jlc->jl_gr, &(ctx->vos[0]), NULL);
	jl_gr_draw_text(jlc->jl_gr, jl_mem_format(jlc, "x:%d, y:%d, z:%d",
			ctx->targetx, ctx->targety, ctx->targetz),
		(jl_vec3_t) { 0., 0., 0. }, ctx->font);
	jl_gr_draw_text(jlc->jl_gr, jl_mem_format(jlc, "movex:%d, movey:%d",
			ctx->movex, ctx->movey),
		(jl_vec3_t) { 0., .025, 0. }, ctx->font);
	jl_print(jlc, "Drew screen");
#endif
}

void vi_get_input(ctx_t* ctx) {
#if VI_WEBCAM == 1
	jl_cv_loop_webcam(ctx->jl_cv);
#else
	jl_cv_loop_image(ctx->jl_cv, FILENAME);
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
#ifdef OLD_CRAP
	MEMTESTER(jlc, "push_data_start");
		strt push_data = jl_cv_loop_makejf(ctx->jl_cv);
	MEMTESTER(jlc, "push_data_start2");
		jl_nt_push_data(ctx->jl_nt, NT_IMG, push_data->data,
			push_data->size);
	MEMTESTER(jlc, "push_data_finish");
#endif
#if PHOTO_CAPTURE == 1
	data_t* push_data = jl_cv_loop_makejf(ctx->jl_cv);
	time_t mytime;

	mytime = time(NULL);
	jl_file_save(jlc, push_data->data,
		jl_mem_format(jlc, "!Pic %s\b.jpeg", ctime(&mytime)),
		push_data->size);
#endif
}

static inline void vi_process(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
	m_u16_t i = 0;
	jl_cv_rect_t blobs[30];
#if TEST == 1
//	uint8_t bounds[] = { 38, 2, 233, 70, 8, 236 };
//
//	uint8_t bounds[] = { 38, 2, 234, 78, 8, 236 };
// With Red Light Ring
	uint8_t bounds[] = { 40, 0, 150, 160, 255, 255 };
// With Green Light Ring
//	uint8_t bounds[] = { 30, 0, 0, 58, 255, 255 };
//	uint8_t bounds[] = { 0, 0, 200, 255, 255, 255 };
//	uint8_t bounds[] = { 30, 0, 220, 85, 15, 255 };
#else
	uint8_t bounds[] = {
		jl_nt_get_num(ctx->jl_nt, "vision/hue.lo"),
		jl_nt_get_num(ctx->jl_nt, "vision/sat.lo"),
		jl_nt_get_num(ctx->jl_nt, "vision/val.lo"),
		jl_nt_get_num(ctx->jl_nt, "vision/hue.hi"),
		jl_nt_get_num(ctx->jl_nt, "vision/sat.hi"),
		jl_nt_get_num(ctx->jl_nt, "vision/val.hi") };
#endif
	int maxw = 0;
	int maxi = 0;

	jl_print(jlc, "%d/%d/%d %d/%d/%d", bounds[0], bounds[1], bounds[2],
		bounds[3], bounds[4], bounds[5]);
	MEMTESTER(jlc, "loop");
// Read image
	vi_get_input(ctx);
	MEMTESTER(jlc, "read_image");
// Filter colors
	jl_cv_loop_filter(ctx->jl_cv, bounds);
// Erode Blobs
	jl_cv_struct_erode(ctx->jl_cv, 5, 5, shape);
// Blob Detect
	u32_t itemc = jl_cv_loop_objectrects(ctx->jl_cv, 30, blobs);
	ctx->item_count = itemc;
// Find the Best Blob
	if(ctx->item_count == 0) return;
	MEMTESTER(jlc, "blob_detect");
	for(i = 0; i < ctx->item_count; i++) {
		if(blobs[i].w > maxw) {
			maxw = blobs[i].w;
			maxi = i;
		}
		jl_cv_draw_rect(ctx->jl_cv, blobs[i]);
	}
	ctx->target = blobs[maxi];
	ctx->targetx = ctx->target.x + (ctx->target.w / 2);
	ctx->targety = ctx->target.y + (ctx->target.h / 2);
	ctx->targetz = (100 * ctx->imgy / (ctx->target.y+1)) - 100;
	ctx->movex = ctx->targetx - (ctx->imgx / 2);
	ctx->movey = ctx->targety - (ctx->imgy / 2);
	MEMTESTER(jlc, "find_blob");
}

void vi_wdns(jl_t* jlc) {
	jl_print(jlc, "Loop....");
	vi_process(jlc);
	vi_push(jlc);
	vi_redraw(jlc);
}

static void vi_exit(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	jl_cv_kill(ctx->jl_cv);
	jl_sg_exit(jlc);
}

static void vi_mdin(jl_t* jlc) {
#if WINDOWED == 1
	jlgr_loop_set(jlc->jl_gr, vi_wdns, jl_dont, vi_wdns, jl_dont);
#endif
}

static void vi_loop(jl_t* jl) {
#if WINDOWED == 1
	jlgr_loop(jl->jl_gr, NULL, 0);
#endif	
}

static inline void vi_init_modes(jl_t* jlc) {
	//Set mode data
	jl_mode_set(jlc,VI_MODE_EDIT, JL_MODE_INIT, vi_mdin);
	jl_mode_set(jlc,VI_MODE_EDIT, JL_MODE_LOOP, vi_loop);
	jl_mode_set(jlc,VI_MODE_EDIT, JL_MODE_EXIT, vi_exit);
	//Leave terminal mode
	jl_mode_switch(jlc, VI_MODE_EDIT);
}

#if WINDOWED == 1
static inline void vi_init_tasks(jl_gr_t* jl_gr) {
//	jl_gr_addicon_slow(jl_gr);
}
#endif

static inline void vi_init_ctx(jl_t* jlc) {
	jlc->uctx = jl_memi(jlc, sizeof(ctx_t));
	ctx_t* ctx = jlc->uctx;
	ctx->jl_cv = jl_cv_init(jlc);
	ctx->jl_nt = jl_nt_init(jlc, HOSTNAME);
	ctx->font = (jl_font_t) { 0, JL_IMGI_ICON, 0,
		color, .025f };

	jl_print(jlc, "that %p", jlc->uctx);
}

static inline void vi_init_vos(jl_t* jlc) {
#if WINDOWED == 1
	ctx_t* ctx = jlc->uctx;

	ctx->vos = jl_gl_vo_make(jlc->jl_gr, 2);
#endif
}

static inline void vi_init_cv(jl_t* jlc) {
	ctx_t* vi = jlc->uctx;

#if VI_WEBCAM == 1
	jl_cv_init_webcam(vi->jl_cv, JL_CV_CHNG, JL_CV_FLIPY, 1);
#else
	jl_cv_init_image(vi->jl_cv, JL_CV_CHNG, FILENAME, JL_CV_FLIPN);
#endif
	jl_cv_img_size(vi->jl_cv, &vi->imgx, &vi->imgy);
}

static inline void vi_init_net(jl_t* jlc) {
	ctx_t* vi = jlc->uctx;
	jl_nt_push_bool(vi->jl_nt, NT_CALIBRATION, 0);
}

#if WINDOWED == 1
static void vi_init_graphics(jl_t* jl) {
	jl_gr_t* jl_gr = jl->jl_gr;

//	jl_gr_draw_msge(jl_gr, 0, JL_IMGI_ICON, 1, "Initializing");
//	jl_printc(jlc,"Initializing....");
	vi_init_tasks(jl_gr);
	vi_init_vos(jl);
	vi_init_cv(jl);
}
#endif

static void vi_init(jl_t* jlc) {
	jl_print_function(jlc, "2846_Vision");
	vi_init_ctx(jlc);
#if WINDOWED == 1
	jlgr_init(jlc, "2016 Vision", 0, vi_init_graphics);
#endif
	vi_init_modes(jlc);

	vi_init_net(jlc);
	jl_print_return(jlc, "2846_Vision");
}

static void vi_kill(jl_t* jlc) {
	jlgr_kill(jlc->jl_gr);
}

int main(int argc, char* argv[]) {
	return jl_init(vi_init, vi_kill);
}
