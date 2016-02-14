#include "jl_cv.h"

#define VI_MODE_EDIT 0
#define VI_MODE_MAXX 1

typedef struct{
	jl_vo_t* vos;
	jl_cv_t* jl_cv;
	int item_count;
	jl_cv_rect_t target;
	m_u16_t targetx;
	m_u16_t targety;
	m_u16_t targetz;
	m_i16_t movex, movey;
	m_u16_t imgx, imgy;
	jl_font_t font;
}ctx_t;
