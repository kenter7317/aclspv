#include "./ctx.h"

IMPL_PASS_RET aclspv_pass_assgn_pipelayout(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		) {
	(void)M;
	(void)CTX;

	return FN_ACLSPV_PASS_OK;
}
