#include "header/main.h"

int oldtbiu = 0;
int shape[] = {
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1
/*	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0,
	0, 1, 1, 1, 0,
	0, 0, 0, 0, 0*/
};
m_u8_t color[] = { 127, 255, 127, 255 };

#define MEMTESTER(a, b) // memtester(a, b  )
/*void memtester(jl_t* jlc, str_t name) {
	int diff = jl_me_tbiu() - oldtbiu;
	printf("%s %d\n", name, diff);
	oldtbiu = jl_me_tbiu();
}*/

static void vi_redraw(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
	double ar;

	// Change to image
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
}

void vi_get_input(ctx_t* ctx) {
//	jl_cv_loop_webcam(ctx->jl_cv);
	jl_cv_loop_image(ctx->jl_cv, "Field_Images/20.jpg");
}

void vi_loop(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
	m_u16_t i;
	jl_rect_t blobs[30];
	uint8_t bounds[] = {20, 200, 90, 40, 255, 180};
	m_u16_t maxw, maxi;

// Read image
	vi_get_input(ctx);
// Filter colors
	jl_cv_loop_filter(ctx->jl_cv, bounds);
// Erode Blobs
	jl_cv_struct_erode(ctx->jl_cv, 5, 5, shape);
// Blob Detect
	ctx->item_count = jl_cv_loop_objectrects(ctx->jl_cv, 30, blobs);
// Find the Best Blob
	for(i = 0; i < ctx->item_count; i++) {
		if(blobs[i].w > maxw) {
			maxw = blobs[i].w;
			maxi = i;
		}
	}
	ctx->target = blobs[maxi];
	ctx->targetx = ctx->target.x + (ctx->target.w / 2);
	ctx->targety = ctx->target.y + (ctx->target.h / 2);
	ctx->targetz = (100 * ctx->imgy / ctx->target.y) - 100;
	ctx->movex = ctx->targetx - (ctx->imgx / 2);
	ctx->movey = ctx->targety - (ctx->imgy / 2);
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
}

void vi_wdns(jl_t* jlc) {
	vi_loop(jlc);
	vi_redraw(jlc);
}

// Called when window is made/resized.
static void vi_resz(jl_t* jlc) {
//	ctx_t* ctx = jlc->uctx;
}

static void vi_exit(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	jl_cv_kill(ctx->jl_cv);
	jl_sg_exit(jlc);
}

static inline void vi_init_modes(jl_t* jlc) {
	//Set mode data
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_DN, vi_wdns);
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_UP, jl_dont);
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_RESZ, vi_resz);
	jl_sg_mode_set(jlc,VI_MODE_EDIT, JL_SG_WM_EXIT, vi_exit);
	jl_sg_mode_switch(jlc, VI_MODE_EDIT, JL_SG_WM_DN); //Leave terminal mode
}

static inline void vi_init_tasks(jl_t* jlc) {
	jl_gr_addicon_slow(jlc);
}

static inline void vi_init_ctx(jl_t* jlc) {
	jlc->uctx = NULL;
	jl_me_alloc(jlc, &jlc->uctx, sizeof(ctx_t), 0);
	ctx_t* ctx = jlc->uctx;
	ctx->jl_cv = jl_cv_init(jlc);
	ctx->font = (jl_font_t) { 0, JL_IMGI_ICON, 0,
		color, .025f };
}

static inline void vi_init_vos(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	ctx->vos = jl_gl_vo_make(jlc, 2);
}

static inline void vi_init_cv(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

//	jl_cv_init_webcam(ctx->jl_cv, JL_CV_ORIG);
	jl_cv_init_image(ctx->jl_cv, JL_CV_CHNG, "Field_Images/0.jpg");
//
	jl_cv_img_size(ctx->jl_cv, &ctx->imgx, &ctx->imgy);
}

void hack_user_init(jl_t* jlc) {
	jl_io_tag_set(jlc, 0, 1, NULL);
	jl_io_offset(jlc, 0, "EXMP");
	jl_gr_draw_msge(jlc, 0, JL_IMGI_ICON, 1, "Initializing");
	jl_io_printc(jlc,"Initializing....\n");
	vi_init_modes(jlc);
	vi_init_tasks(jlc);
	vi_init_ctx(jlc);
	vi_init_vos(jlc);
	vi_init_cv(jlc);
	jl_io_close_block(jlc);
}
