#include <stdio.h>
#include <aclspv/compiler.h>
#include <aclspv.h>

int main(void) {
	aclspv_init_global();

	h_aclspv_compiler_t		compiler;
	compiler = aclspv_mk_compiler();
	struct CXUnsavedFile	main_c;
	const char KERNEL[] =
		"__kernel void add(__constant int* a, __global int* b) {"
			"*(b) = *(a);"
		"}";
	main_c.Filename = "main.c";
	main_c.Length = sizeof(KERNEL);
	main_c.Contents = KERNEL;

#if 0
	h_aclspv_obj_t obj = aclspv_compile(&compiler, "main.c", &main_c, 1
			, NULL, 0);

	aclspv_obj_stop(obj);
#endif

	aclspv_free_compiler(compiler);
	aclspv_stop_global();
	return 0;
}
