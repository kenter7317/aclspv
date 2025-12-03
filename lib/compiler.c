#include <aclspv/compiler.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ae2f/c90/StdInt.h>


#include "./compiler.h"

ACLSPV_ABI_IMPL
e_aclspv_init_compiler	aclspv_init_compiler(h_aclspv_compiler_t i_compiler){
	assert(i_compiler);
	unless(i_compiler) return ACLSPV_INIT_COMPILER_ARG_WAS_NULL;
	i_compiler->mh_idx = clang_createIndex(0, 0);
	return ACLSPV_INIT_COMPILER_OK;
}

ACLSPV_ABI_IMPL
void	aclspv_stop_compiler(h_aclspv_compiler_t s_compiler) {
	unless(s_compiler) return;
	clang_disposeIndex(s_compiler->mh_idx);
}

ACLSPV_ABI_IMPL
h_aclspv_obj_t	aclspv_compile(
		h_aclspv_compiler_t	h_compiler,
		const char* const	rd_srcpath,
		const  struct CXUnsavedFile* const ae2f_restrict	rd_unsaved,
		const size_t						c_unsaved_len,
		const char* ae2f_restrict const * ae2f_restrict const	rd_argv_opt,
		const size_t						c_argc
		)
{
	unless(h_compiler && rd_unsaved && c_unsaved_len && rd_srcpath && rd_srcpath[0]) {
		assert(!"argument is not enough");
		return 0;
	}

	if((rd_argv_opt) && c_argc) {
		const char* ae2f_restrict * ae2f_restrict 
			argv = malloc(sizeof(void*) * (c_argc + compiler_argc));

		unless(argv) {
			assert(!"allocation has failed");
			return 0;
		}

		argv[0] = compiler_arg0; 
		argv[1] = compiler_arg1;
		argv[2] = compiler_arg2;
		argv[3] = compiler_arg3;
		argv[4] = compiler_arg4;
		argv[5] = compiler_arg5;
		argv[6] = compiler_arg6;

		memcpy(
				(void*)(argv + compiler_argc)
				, (const void*)rd_argv_opt
				, c_argc * sizeof(void*)
		      );

#define argc	(compiler_argc + c_argc)



		free((void*)argv);
#undef	argc	
	} else {

#define	argc	compiler_argc
		const char*	ae2f_restrict argv[] = {
			compiler_arg0,
			compiler_arg1,
			compiler_arg2,
			compiler_arg3,
			compiler_arg4,
			compiler_arg5,
			compiler_arg6
		};
		extern char	STATIC_ASSERT[sizeof(argv) / sizeof(argv[0]) == argc ? 1 : -1];
#undef	argc
	}

}
