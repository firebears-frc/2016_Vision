#include "header/main.h"
#define VI_WEBCAM 0

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

#define MEMTESTER(a, b) // memtester(a, b  )
/*void memtester(jl_t* jlc, str_t name) {
	int diff = jl_me_tbiu() - oldtbiu;
	printf("%s %d\n", name, diff);
	oldtbiu = jl_me_tbiu();
}*/

static void ex_redraw(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	jl_gr_draw_vo(jlc, &(ctx->vos[0]), NULL);
	jl_gr_draw_int(jlc, ctx->circle.x,
		(jl_vec3_t) { 0., 0., 0. },
		(jl_font_t) { 0, JL_IMGI_ICON, 0,
			(uint8_t []) { 255, 255 ,255, 255}, .0625f });
}

void ex_down(jl_t* jlc) {
	if(jlc->ctrl.h == 1) jl_gr_togglemenubar(jlc);
}

void vi_get_input(ctx_t* ctx) {
#if VI_WEBCAM == 1
	jl_cv_loop_webcam(ctx->jl_cv);
#else
	jl_cv_loop_image(ctx->jl_cv, "Field_Images/19.jpg");
#endif
}

void ex_loop(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
	double ar;
	int i;
	jl_cv_line_t lines[15];
	uint8_t bounds[] = {20, 200, 90, 40, 255, 180};

	jl_ct_run_event(jlc,
		JL_CT_ALLP(JL_CT_GAME_STRT, JL_CT_COMP_MENU, JL_CT_ANDR_MENU),
		ex_down, jl_dont);
	vi_get_input(ctx);
// Filter colors
	jl_cv_loop_filter(ctx->jl_cv, bounds);
// Erode Blobs
	jl_cv_skeleton(ctx->jl_cv, 5, 5, shape);
// Line Detect
	ctx->circle.x = jl_cv_loop_detect_lines(ctx->jl_cv, 15, 50, 10, lines);
	for(i = 0; i < ctx->circle.x; i++) {
		jl_cv_draw_line(ctx->jl_cv, lines[i]);
	}
	ar = jl_cv_loop_maketx(ctx->jl_cv);
	jl_gr_vos_texture(jlc, &(ctx->vos[0]),
		(jl_rect_t) { 0.f, 0.f, 1.f, ar }, &(ctx->jl_cv->textures[0]),
		0, 255);
}

void ex_wdns(jl_t* jlc) {
	ex_loop(jlc);
	ex_redraw(jlc);
}

// Called when window is made/resized.
static void ex_resz(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;
	jl_rect_t rc1 = { 0.f, 0.f, 1.f, jl_gl_ar(jlc) };
	u8_t colors[] = { 255, 85, 0, 127 };

//	jl_gr_vos_image(jlc, &(ctx->vos[0]), rc1, 1, 0, 0, 255);
	jl_gr_vos_rec(jlc, &(ctx->vos[1]), rc1, colors, 0);
}

static void ex_exit(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	jl_cv_kill(ctx->jl_cv);
	jl_sg_exit(jlc);
}

static inline void ex_init_modes(jl_t* jlc) {
	//Set mode data
	jl_sg_mode_set(jlc,EX_MODE_EDIT, JL_SG_WM_DN, ex_wdns);
	jl_sg_mode_set(jlc,EX_MODE_EDIT, JL_SG_WM_UP, jl_dont);
	jl_sg_mode_set(jlc,EX_MODE_EDIT, JL_SG_WM_RESZ, ex_resz);
	jl_sg_mode_set(jlc,EX_MODE_EDIT, JL_SG_WM_EXIT, ex_exit);
	jl_sg_mode_switch(jlc, EX_MODE_EDIT, JL_SG_WM_DN); //Leave terminal mode
}

static inline void ex_init_tasks(jl_t* jlc) {
	jl_gr_addicon_slow(jlc);
}

static inline void ex_init_ctx(jl_t* jlc) {
	jlc->uctx = NULL;
	jl_me_alloc(jlc, &jlc->uctx, sizeof(ctx_t), 0);
	ctx_t* ctx = jlc->uctx;
	ctx->jl_cv = jl_cv_init(jlc);
}

static inline void ex_init_vos(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

	ctx->vos = jl_gl_vo_make(jlc, 2);
}

static inline void ex_init_cv(jl_t* jlc) {
	ctx_t* ctx = jlc->uctx;

#if VI_WEBCAM == 1
	jl_cv_init_webcam(ctx->jl_cv, JL_CV_ORIG, JL_CV_FLIPY);
#else
	jl_cv_init_image(ctx->jl_cv, JL_CV_CHNG, "Field_Images/0.jpg",
		JL_CV_FLIPN);
#endif
}

void hack_user_init(jl_t* jlc) {
	jl_io_tag_set(jlc, 0, 1, NULL);
	jl_io_offset(jlc, 0, "EXMP");
	jl_gr_draw_msge(jlc, 0, JL_IMGI_ICON, 1, "Initializing");
	jl_io_printc(jlc,"Initializing....\n");
	ex_init_modes(jlc);
	ex_init_tasks(jlc);
	ex_init_ctx(jlc);
	ex_init_vos(jlc);
	ex_init_cv(jlc);
	jl_io_close_block(jlc);
}
