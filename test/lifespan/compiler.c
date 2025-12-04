#include <assert.h>

#include <aclspv.h>
#include <aclspv/compiler.h>

int main(void) {
	h_aclspv_compiler_t compiler;

	aclspv_init_global();
	compiler = aclspv_mk_compiler();
	assert(compiler);
	aclspv_free_compiler(compiler);
	aclspv_stop_global();
	return 0;
}
