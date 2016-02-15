#include "jl_ntcore.h"

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

void jl_ntcore_push_num(jl_ntcore_t* jl_ntcore, str_t name, f32_t value) {
	NT_SetEntryDouble(name, strlen(name), value, 1);
}

void jl_ntcore_push_bool(jl_ntcore_t* jl_ntcore, str_t name, u8_t value) {
	NT_SetEntryBoolean(name, strlen(name), value, 1);
}

void jl_ntcore_push_data(jl_ntcore_t* jl_ntcore, str_t name, void* data,
	u32_t datasize)
{
	NT_SetEntryRaw(name, strlen(name), data, datasize, 1);
}

u8_t jl_ntcore_pull_bool(jl_ntcore_t* jl_ntcore, str_t name) {
	int boolean;
	NT_GetEntryBoolean(name, strlen(name), NULL, &boolean);
	return boolean;
}

void jl_ntcore_kill(jl_ntcore_t* jl_ntcore) {
	NT_StopClient();
//	dlclose(jl_ntcore->ntcore);
}
