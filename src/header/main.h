#include "jl_cv.h"
#include "jl_nt.h"

#define VI_MODE_EDIT 0
#define VI_MODE_MAXX 1

#define NT_DISTANCE "vision/distance"
#define NT_ANGLE "vision/angle"
#define NT_FPS "vision/fps"
#define NT_SIZE "vision/size"

typedef struct{
	jl_vo_t* vos;
	jl_cv_t* jl_cv;
	jl_nt_t* jl_nt;
	m_u32_t item_count;
	jl_cv_rect_t target;
	m_u16_t targetx;
	m_u16_t targety;
	m_i16_t movex, movey;
	m_u16_t imgx, imgy;
	m_u16_t size;
	jl_font_t font;
}ctx_t;
