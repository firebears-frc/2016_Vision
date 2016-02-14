#include "include/ntcore.h"
#include <dlfcn.h>
#include "jl.h"

typedef struct{
	jl_t* jlc;
	void* ntcore;
}jl_ntcore_t;

jl_ntcore_t* jl_ntcore_init(jl_t* jlc, str_t hostname);
