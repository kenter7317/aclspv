/** 
 * @file 	spirv_prepare_fn.c
 * @brief	is an implementation for an interface
 * */

#include <aclspv/pass.h>

#include <ae2f/c90/StdBool.h>
#include <ae2f/c90/Limits.h>
#include <ae2f/c90/StdInt.h>


/** 
 * @see <aclspv/pass.h> 
 * TODO: implement this 
 * */
ACLSPV_ABI_IMPL e_fn_aclspv_pass aclspv_pass_spirv_prepare_fn(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		) {
	(void)M;
	(void)CTX;
	return FN_ACLSPV_PASS_OK;
}
