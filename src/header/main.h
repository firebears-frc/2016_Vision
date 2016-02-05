#include "jl_cv.h"

#define EX_MODE_EDIT 0
#define EX_MODE_MAXX 1

typedef struct{
	jl_vo_t* vos;
	jl_cv_t* jl_cv;
	struct{
		int x, y;
	}circle;
}ctx_t;
