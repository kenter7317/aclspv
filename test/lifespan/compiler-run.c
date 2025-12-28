#include <aclspv.h>
#include <assert.h>
#include <stdlib.h>

#define maincontent	\
	"__kernel void __kernel_name(__global int* _glob1, __global int* _glob2) {"	\
		"*(_glob1) = *(_glob2);"	\
	"}"

int main(void) {
	struct CXUnsavedFile	files[1];
	const char* ARG[] = { "main.cl" };
	aclspv_wrd_t* spirv0, *spirv1;

	files[0].Contents = maincontent;
	files[0].Filename = "main.cl";
	files[0].Length = sizeof(maincontent) - 1;

	aclspv_compile(
			files, 1
			, ARG, 1
			, ae2f_NIL, &spirv0, ae2f_NIL
			);
	aclspv_compile(
			files, 1
			, ARG, 1
			, ae2f_NIL, &spirv1, ae2f_NIL
			);

	assert(spirv0);
	assert(spirv1);

	free(spirv0);
	free(spirv1);

	return 0;
}
