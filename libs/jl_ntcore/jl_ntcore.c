#include "jl_ntcore.h"
#define HOSTNAME ""
#define HOSTNAME ""

/*static inline void jl_ntcore_loadntcore(jl_ntcore_t* jl_ntcore) {
	if(!(jl_ntcore->ntcore = dlopen("libntcore.so", RTLD_NOW | RTLD_GLOBAL))) {
		jl_io_print(jl_ntcore->jlc, "failed to load .so");
		jl_io_print(jl_ntcore->jlc, "%s", dlerror());
		jl_sg_exit(jl_ntcore->jlc);
	}
}*/

jl_ntcore_t* jl_ntcore_init(jl_t* jlc, str_t hostname) {
	jl_ntcore_t* jl_ntcore = NULL;
	jl_me_alloc(jlc, (void**)&jl_ntcore, sizeof(jl_ntcore_t), 0);
	jl_ntcore->jlc = jlc;
	// Load the library
	// jl_ntcore_loadntcore(jl_ntcore);
	// Start client
	NT_StartClient(hostname, 1735);
	return jl_ntcore;
}

void jl_ntcore_kill(jl_ntcore_t* jl_ntcore) {
	NT_StopClient();
	dlclose(jl_ntcore->ntcore);
}
