#include "ctx.h"
#include <assert.h>

IMPL_PASS_RET aclspv_pass_rewr_loc_ptr(
		const LLVMModuleRef M,
		const h_aclspv_pass_ctx CTX
		) {
	/* Re-use similar collection loop as bitcast pass, but target remaining ptrtoint/inttoptr chains involving local pointers */
	/* Or simply re-run a subset of rep_ptr_bitcast logic filtered to local address space */
	/* For now: stub or call aclspv_pass_rep_ptr_bitcast again if needed */

	assert(M);
	assert(CTX);

	return FN_ACLSPV_PASS_OK;
}
