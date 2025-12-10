#include "./ctx.h"


IMPL_PASS_RET aclspv_pass_inline_entp(
		const LLVMModuleRef	M,
		const h_aclspv_pass_ctx	CTX
		)
{
	IGNORE(M);
	IGNORE(CTX);

	return FN_ACLSPV_PASS_OK;
}
