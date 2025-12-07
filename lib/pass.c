#include <aclspv/pass.h>
#include <assert.h>
#include "./pass/ctx.h"

#include <stdlib.h>

#define aclspv_free(a, b)	free(a)

#define Z(a)	0

ACLSPV_ABI_IMPL e_aclspv_passes	
aclspv_runall_module_passes(
		const LLVMModuleRef			h_module, 
		e_fn_aclspv_pass* ae2f_restrict const	wr_res_opt
		)
{
	e_fn_aclspv_pass	codepass	= FN_ACLSPV_PASS_OK;
	e_aclspv_passes		code		= ACLSPV_PASSES_OK;
	a_aclspv_pass_ctx	ctx;

	_aclspv_init_vec(ctx.m_v0);

	unless(h_module) {
		assert(Z(FN_ACLSPV_PASS_MODULE_NIL));
		code		= ACLSPV_PASSES_REWR_KERN_FN;
		codepass	= FN_ACLSPV_PASS_MODULE_NIL;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_rewr_kern_fn(h_module, &ctx))) {
		code = ACLSPV_PASSES_REWR_KERN_FN;
		goto LBL_RET;
	}

	if((codepass = aclspv_pass_set_spec_const_default_val(h_module, &ctx))) {
		code = ACLSPV_PASSES_SET_SPEC_CONST_DEFAULT_VAL;
		goto LBL_RET;
	}

LBL_RET:
	_aclspv_stop_vec(aclspv_free, ctx.m_v0);

	if(wr_res_opt) *wr_res_opt = codepass;
	return code;
}
