/** 
 * @file 	inline_func.c
 * @brief	is an implementation for an interface
 * */

#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/Limits.h>
#include <ae2f/c90/StdInt.h>

#include "./ctx.h"

/** 
 * @see <aclspv/pass.h> 
 * TODO: implement this 
 * */
ACLSPV_ABI_IMPL e_fn_aclspv_pass aclspv_pass_inline_func(
		const LLVMModuleRef 	M,
		const h_aclspv_pass_ctx	CTX
		) {
	(void)M;
	(void)CTX;
	return FN_ACLSPV_PASS_OK;
}
